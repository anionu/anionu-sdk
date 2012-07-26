#include "SurveillanceMode.h"
#include "SurveillanceMultipartPacketizer.h"

#include "Sourcey/Spot/IChannel.h"
#include "Sourcey/Spot/IEnvironment.h"
#include "Sourcey/Spot/IConfiguration.h"
#include "Sourcey/Spot/IStreamingManager.h"
#include "Sourcey/Spot/ISynchronizer.h"
#include "Sourcey/CryptoProvider.h"
#include "Sourcey/Anionu/Event.h"


using namespace std;
using namespace Poco;
using namespace Sourcey::Media;


namespace Sourcey {
namespace Spot {


SurveillanceMode::SurveillanceMode(IEnvironment& env, IChannel& channel) :
	IMode(env, channel, "Surveillance Mode"),
	_isConfiguring(false)
{
	log() << "Creating" << endl;	
}


SurveillanceMode::~SurveillanceMode()
{	
	log() << "Destroying" << endl;	
}


void SurveillanceMode::initialize() 
{
	log() << "Initializing" << endl;	

	loadConfig();
	
	_env.streaming().InitializeStreamingSession += delegate(this, &SurveillanceMode::onInitializeStreamingSession);	
	_env.streaming().InitializeStreamingConnection += delegate(this, &SurveillanceMode::onInitializeStreamingConnection);	
}


void SurveillanceMode::uninitialize() 
{
	log() << "UnInitializing" << endl;
	
	_env.streaming().InitializeStreamingSession -= delegate(this, &SurveillanceMode::onInitializeStreamingSession);
	_env.streaming().InitializeStreamingConnection -= delegate(this, &SurveillanceMode::onInitializeStreamingConnection);

	// FIXME: For some reason delegates added across the
	// process boundary are causing the SignalBase to
	// crash when cleanup() is called from the other side.
	//_env.streaming().InitializeStreamingSession.cleanup();
}


void SurveillanceMode::loadConfig()
{
	log() << "Loading Config: " << _channel.name() << endl;

	_motionDetector.options().motionThreshold = _config.getInt("MotionThreshold", 10000);			// 10000 [50 - 50000]
	_motionDetector.options().minPixelValue = _config.getInt("MinPixelValue", 30);					// 30 [0 - 255]
	_motionDetector.options().preSurveillanceDelay = _config.getInt("PreSurveillanceDelay", 0);		// 0 seconds
	_motionDetector.options().postMotionEndedDelay = _config.getInt("PostTriggeredDelay", 3);		// 3 seconds
	_motionDetector.options().minTriggeredDuration = _config.getInt("MinTriggeredDuration", 5);		// 5 seconds
	_motionDetector.options().maxTriggeredDuration = _config.getInt("MaxTriggeredDuration", 10);	// 60 seconds (1 min)
	_motionDetector.options().stableMotionNumFrames = _config.getInt("StableMotionNumFrames", 3);	// 3 frames
	_motionDetector.options().stableMotionLifetime = _config.getDouble("StableMotionLifetime", 1.0);// 1 (High)
	_motionDetector.options().captureFPS = _config.getInt("CaptureFPS", 10);						// 10 frames per second
	_synchronizeVideos = _config.getBool("SynchronizeVideos", true);
}


void SurveillanceMode::activate() 
{
	log() << "Enabling:"
		<< "\n\tPre Surveillance Delay: " << _motionDetector.options().preSurveillanceDelay
		<< "\n\tPost Triggered Delay: " << _motionDetector.options().postMotionEndedDelay
		<< "\n\tMinimum Motion Triggered Duration: " << _motionDetector.options().minTriggeredDuration
		<< "\n\tMaximum Motion Triggered Duration: " << _motionDetector.options().maxTriggeredDuration
		//<< "\n\tStable Motion Frames: " << _motionDetector.options().stableMotionNumFrames
		//<< "\n\tStable Motion Duration: " << _motionDetector.options().stableMotionLifetime
		<< endl;
	
	try
	{
		startMotionDetector();
		IMode::activate();
	}
	catch (Exception& exc)
	{
		log("error") << "Error: " << exc.displayText() << endl;
		setState(this, ModeState::Error);
		throw exc;
	}
}


void SurveillanceMode::deactivate() 
{
	log() << "Disabling" << endl;
	stopRecording();
	stopMotionDetector();
	IMode::deactivate();
}


bool SurveillanceMode::startMotionDetector()
{
	FastMutex::ScopedLock lock(_mutex); 

	if (_motionDetector.isRunning()) {
		log("warn") << "The motion detector is already running" << endl;
		return false;
	}

	Media::VideoCapture* video = _channel.videoCapture(true);
		
	_motionStream.attach(video, false);

	// Attach the motion detector to the stream.
	_motionStream.attach(&_motionDetector, 1, false);
	_motionDetector.StateChange += delegate(this, &SurveillanceMode::onMotionStateChange);

	_motionStream.start();

	return true;
}


bool SurveillanceMode::stopMotionDetector()
{
	FastMutex::ScopedLock lock(_mutex); 

	if (_motionDetector.isRunning()) {
		_motionDetector.StateChange -= delegate(this, &SurveillanceMode::onMotionStateChange);
		//_motionDetector.stop();
		_motionStream.close();
		return true;
	}		

	log("warn") << "Cannot stop the motion detector because it is not running." << endl;
	_motionStream.close();
	return false;
}


void SurveillanceMode::onMotionStateChange(void* sender, Media::MotionDetectorState& state, const Media::MotionDetectorState&)
{
	log() << "Motion State Changed: " << state.toString() << endl;

	// Discard events while configuring.
	{
		FastMutex::ScopedLock lock(_mutex); 
		if (_isConfiguring)
			return;
	}

	switch (state.id()) {
		case Media::MotionDetectorState::Idle:	
		break;

		case Media::MotionDetectorState::Waiting:

			// Stop recording.
			// Always make this call just to be sure.
			stopRecording();

		break;

		case Media::MotionDetectorState::Vigilant:
		break;

		case Media::MotionDetectorState::Triggered:

			// Create a Motion Detected event via the
			// Anionu API to notify account users.
			Anionu::Event event(Anionu::Event::High, "Motion Detected", "Motion detected on channel: " + _channel.name());
			env().broadcastEvent(event);
			
			// Start recording.
			startRecording();

		break;
	}
}

	
bool SurveillanceMode::startRecording()
{	
	FastMutex::ScopedLock lock(_mutex); 

	assert(_recordingInfo.token.empty());

	Media::RecorderOptions options;
	env().media().initRecorderOptions(_channel, options);
	//params.duration = time(0) + _segmentDuration;
	RecordingInfo* info = env().media().startRecording(_channel, options);
	if (info) {
		info->synchronize = _synchronizeVideos;
		_recordingInfo = RecordingInfo(*info);
		//_recordingInfo.encoder->StateChange += delegate(this, &SurveillanceMode::onEncoderStateChange);
		log() << "Started Recording: " << _recordingInfo.token << endl;
		return true;
	}
	
	return false;
}


bool SurveillanceMode::stopRecording()
{
	FastMutex::ScopedLock lock(_mutex); 

	if (!_recordingInfo.token.empty()) {
		//_recordingInfo.encoder->StateChange -= delegate(this, &SurveillanceMode::onEncoderStateChange);
		env().media().stopRecording(_recordingInfo.token);
		log() << "Stopped Recording: " << _recordingInfo.token << endl;
		_recordingInfo.token = "";
		_recordingInfo.encoder = NULL;
		return true;
	}
	return false;
}


/*
void SurveillanceMode::onEncoderStateChange(void* sender, Media::EncoderState& state, const Media::EncoderState& oldState)
{
	log() << "Recorder State Changed: " << state.toString() << endl;
	IEncoder* encoder = reinterpret_cast<IEncoder*>(sender);

	switch (state.id()) {

		case Media::EncoderState::Stopped:
			//FastMutex::ScopedLock lock(_mutex); 

			// Start a new recording segment if the mode is 
			// still enabled.
			//if (!isDisabled() &&
			//	encoder == _recordingInfo.encoder) {
			//	encoder->StateChange -= delegate(this, &SurveillanceMode::onEncoderStateChange);
				
			// Synchronize video's if required
			//if (_synchronizeVideos) {

			Media::RecorderOptions& options = static_cast<Media::RecorderOptions&>(encoder->options());

			Spot::Job job;
			job.type = "Video";
			job.path = options.ofile;					

			log() << "Synchronizing Video: " << job.path << endl;
			env().synchronizer() >> job;

			//}
			//}
		break;
	}
}
*/

	
void SurveillanceMode::onInitializeStreamingSession(void*, IStreamingSession& session, bool& handled)
{
	log() << "Initialize Media Stream: " << session.token() << endl;

	if (_mediaToken == session.token()) {
		if (!_mediaTokenTimeout.expired()) {
			log() << "Creating Configuration Media Stream: " << _mediaToken << endl;
						
			// Attach the video input.
			Media::VideoCapture* video = _channel.videoCapture(true);
			//session.stream().attach(video, false);

			// Set the input format for the encoder.
			AllocateOpenCVInputFormat(video, session.options().iformat);
			session.options().iformat.video.pixfmt = PixelFormat::GRAY8;

			// Initialize a new motion detector using existing
			// options.
			//Media::MotionDetector* detector = new Media::MotionDetector(_motionDetector.options());
			//session.stream().attach(detector, 3, true);
			session.stream().attach(&_motionDetector, 3, false);
			
			session.stream().StateChange += delegate(this, &SurveillanceMode::onSessionStreamStateChange);

			// Let the session manager know we have completely
			// overridden session stream creation.
			handled = true;
	
			// While the isConfiguring flag is set all motion
			// detector states will be ignored. The flag will 
			// be unset when the session stream is closed.
			_isConfiguring = false;
		}
		else {
			log() << "Configuration Media Stream Timed Out" << endl;
			throw Exception("Surveillance Mode media preview has timed out.");
		}
	}
}


void SurveillanceMode::onSessionStreamStateChange(void* sender, PacketStreamState& state, const PacketStreamState&)
{
	PacketStream* stream = reinterpret_cast<PacketStream*>(sender); 
			
	log() << "Configuration Media Session State Changed: " << state.toString() << endl;

	if (state.id() == PacketStreamState::Closing) {
		stream->StateChange -= delegate(this, &SurveillanceMode::onSessionStreamStateChange);

		// Live configuration is over, life as normal.
		_isConfiguring = false;

		/*
		// Apply the updated configuration detector
		// options to our main detector.
		Media::MotionDetector* detector = stream->getProcessor<Media::MotionDetector>();
		assert(detector);
		if (detector)
			_motionDetector.setOptions(detector->options());
			*/
	}
}


void SurveillanceMode::onInitializeStreamingConnection(void*, IStreamingSession& session, ConnectionStream& connection, bool& handled)
{
	log() << "Initialize Media Connection: " << session.token() << endl;

	if (_mediaToken == session.token()) {
			
		//Media::MotionDetector* detector = session.stream().getProcessor<Media::MotionDetector>();
		//assert(detector);
		//if (detector) {
			// Override the connection packetizer if we are using HTTP.
			// HTTP is prefferable for the configuration session so we
			// can display surveillance metadata.
			// NOTE: We have not completely overridden the connection
			// stream creation, further encoders and packetizers may
			// be added depending on streaming configuration.
			if (session.options().protocol == "HTTP")
				//connection.attach(new SurveillanceMultipartPacketizer(*detector), 20, true);
				connection.attach(new SurveillanceMultipartPacketizer(_motionDetector), 20, true);
		//}
	}
}


bool SurveillanceMode::isConfigurable() const
{	
	return true;
}


bool SurveillanceMode::hasParsableConfig(Symple::Form& form) const
{
	return form.hasField(".Surveillance Mode.", true);
}


void SurveillanceMode::buildConfigForm(Symple::Form& form, Symple::FormElement& element, bool baseScope)
{
	log() << "Creating Config Form" << endl;

	Symple::FormField field;
	
	if (baseScope) {
		element.setHint(
			"This form enables you to configure the default settings for Surveillance Mode. "
			"Any settings configured here may be overridden on a per channel basis (see Channel Configuration)."
		);
	}

	// When configuring at channel scope we create a media element
	// which will display live changes as the channel is configured.
	// This is achieved by adding a "media" type field with a locally
	// generated media session token as the value. When a media session
	// request is received matching our token we create the video stream.
	else {	
		element.setHint(
			"This form enables you to configure your surveillance and motion detection settings in real-time. "
			"Keep an eye on the motion levels and motion detector state; "
			"when the motion levels exceed the threshold the alarm will trigger."
			"Use the help icons below for more assistance."
		);

		field = element.addField("media", _config.getDefaultScope("Preview"), "Motion Preview");
		_mediaToken = CryptoProvider::generateRandomKey(32);
		_mediaTokenTimeout.setDelay(10000); // 10 secs to redeem
		_mediaTokenTimeout.start();
		field.setHint(
			"The video above displays live changes as you modify the form below.\r\n"
			"Remote video feeds may be delayed by a couple of seconds. "
		);
		field.setValue(_mediaToken);
	}

	field = element.addField("text", _config.getScoped("MotionThreshold", baseScope), "Motion Threshold");	
	field.setHint(
		"The 'Motion Threshold' determines the sensitivity of the motion detector. "
		"Motion is detected if the 'Motion Level' exceeds the 'Motion Threshold' in any given frame."
	);
	field.setValue(_motionDetector.options().motionThreshold);
	if (!baseScope)
		field.setLive(true);

	field = element.addField("text", _config.getScoped("PreSurveillanceDelay", baseScope), "Pre Surveillance Mode Delay");	
	field.setHint(
		"This is the delay time (in seconds) before motion detection will actually commence after activating Surveillance Mode. "
		"This should be set to the amount of time you need to vacate the room/premises after mode activation."
	);
	field.setValue(_motionDetector.options().preSurveillanceDelay);
	if (!baseScope)
		field.setLive(true);

	field = element.addField("text", _config.getScoped("MinTriggeredDuration", baseScope), "Min Triggered Duration");
	field.setHint(
		"This is minimum duration of time that the motion detector can remain in the 'Triggered' state. "
		"Also, each time motion is detected while in the 'Triggered' state the timer will be extended by 'Min Triggered Duration' seconds."
	);
	field.setValue(_motionDetector.options().minTriggeredDuration);
	if (!baseScope)
		field.setLive(true);

	field = element.addField("text", _config.getScoped("MaxTriggeredDuration", baseScope), "Max Triggered Duration");	
	field.setHint(
		"This is the maximum amount of time that the motion detector can remain in the 'Triggered' state. "
		"In turn it also controls the maximum duration of any videos recorded in Surveillance Mode. "
		"If the 'Max Triggered Duration' is reached, any videos will stop recording and the system will wait "
		"for the 'Post Triggered Delay' duration of time before motion detection will recommence."
	);
	field.setValue(_motionDetector.options().maxTriggeredDuration);
	if (!baseScope)
		field.setLive(true);

	field = element.addField("text", _config.getScoped("PostTriggeredDelay", baseScope), "Post Triggered Delay");	
	field.setHint(
		"This is the delay time (in seconds) after the motion detector's 'Triggered' state has ended before motion detection will recommence."
	);
	field.setValue(_motionDetector.options().postMotionEndedDelay);
	if (!baseScope)
		field.setLive(true);

	field = element.addField("text", _config.getScoped("StableMotionNumFrames", baseScope), "Stable Motion Frames");
	field.setHint(
		"In order to avoid false alarms the motion detector must detect a 'Stable Motion Frames' of motion frames before the alarm is triggered. "
		"This is calculated as follows; if motion is detected on a 'Stable Motion Frames' number of frames before a 'Motion Lifetime' duration of time expires then the alarm will trigger."    
	);
	field.setValue(_motionDetector.options().stableMotionNumFrames);	
	if (!baseScope)
		field.setLive(true);

	field = element.addField("text", _config.getScoped("StableMotionLifetime", baseScope), "Stable Motion Lifetime");	
	field.setHint(
		"The duration (in seconds) in which motion pixels are remembered by the system. "
		"This setting directly affects how long motion frames remain valid. " 
		"The 'Motion Lifetime' value works as follows; if a 'Stable Motion Frames' "
		"of frames exceeding the 'Motion Threshold' are detected within a 'Motion Lifetime' "
		"duration of time then the alarm will trigger." 
	);
	field.setValue(_motionDetector.options().stableMotionLifetime);
	if (!baseScope)
		field.setLive(true);	

	// Enable Video Synchronization
	field = element.addField("boolean", _config.getScoped("SynchronizeVideos", baseScope), "Enable Video Synchronization");	
	field.setHint(
		"Do you want to upload / synchronize recorded videos with your Anionu account?"
	);
	field.setValue(_synchronizeVideos);	
}


void SurveillanceMode::parseConfigForm(Symple::Form& form, Symple::FormElement& element)
{
	log() << "Parsing Config Form" << endl;
	
	Symple::FormField field;
	
	field = element.getField("Surveillance Mode.MotionThreshold", true);
	if (field.valid()) {
		int value = field.intValue();
		if (value < 50 ||
			value > 50000) {
			field.setError("The sensitivity setting must be a number between 50 and 50000.");
		}
		else
			_env.config().setInt(field.id(), value);
	}
	
	field = element.getField("Surveillance Mode.PreSurveillanceDelay", true);
	if (field.valid())
		_env.config().setInt(field.id(), field.intValue());

	field = element.getField("Surveillance Mode.PostTriggeredDelay", true);
	if (field.valid())
		_env.config().setInt(field.id(), field.intValue());

	field = element.getField("Surveillance Mode.MinTriggeredDuration", true);
	if (field.valid())
		_env.config().setInt(field.id(), field.intValue());

	field = element.getField("Surveillance Mode.MaxTriggeredDuration", true);
	if (field.valid())
		_env.config().setInt(field.id(), field.intValue());

	field = element.getField("Surveillance Mode.StableMotionNumFrames", true);
	if (field.valid())
		_env.config().setInt(field.id(), field.intValue());

	field = element.getField("Surveillance Mode.StableMotionLifetime", true);
	if (field.valid())
		_env.config().setInt(field.id(), field.intValue());

	field = element.getField("Surveillance Mode.SynchronizeVideos", true);
	if (field.valid())
		_env.config().setBool(field.id(), field.boolValue());

	loadConfig();
}


void SurveillanceMode::printInformation(std::ostream& s) 
{
	s << "<h2>About Surveillance Mode</h2>";
	s << "<p>Surveillance Mode provides Spot with advanced motion detection capabilities and notifications. ";
	
	s << "<h2>Using Surveillance Mode</h2>";
	s << "<p>Once Surveillance Mode is configured and enabled Spot will begin detecting motion on the current channel.</p>";
	s << "<p>When motion is detected two things will happen:</p>";
	s << "<ul><li>A video will be recorded and optionally synchronized with your Anionu account.</li>"; 
	s << "<ul><li>A 'Motion Detected' event will be created which will trigger notifications.</li>";
	
	//recording videos during intervals of motion

	/* a video will be recorded until motion ends, and a 'Motion Detected' event will be created which will trigger notifications.
	s << "Surveillance Mode will also alert you when motion is detected and record videos during intervals of motion.</p>";  

	s << "<h2>What happens when motion is detected?</h2>";
	s << "<p>If recording capabilities are enabled, time-stamped videos will be recorded during intervals of motion and immediately uploaded to your Anionu.com account for safe storage. ";
	s << "A Motion Detected event will also be triggered, automatically notifying the recipients of all Motion Detected events on your account. ";
	s << "In order to configure who receives these notifications and by what method(mobile phone, email, twitter) refer to the 'Event Notifications' section of your Anionu.com dashboard.\n";
	s << "To achieve a higher level of security you might also opt to send these notifications to a third party security company who can respond to the alarms.</p>";

	s << "<h2>How do I configure Surveillance Mode?</h2>";
	s << "<p>The motion detector's sensitivity option can be configured on a per channel basis from the Channels section of the Settings page. ";
	s << "All other operating parameters can be configured from the Surveillance Mode section.</p>";
	*/
}


} } // namespace Sourcey::Spot