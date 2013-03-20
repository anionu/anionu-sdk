#include "SurveillanceMode.h"
#include "SurveillanceMultipartPacketizer.h"
#include "Sourcey/Spot/IChannel.h"
#include "Sourcey/Spot/IEnvironment.h"
#include "Sourcey/Spot/IConfiguration.h"
#include "Sourcey/Spot/ISession.h"
#include "Sourcey/Spot/IStreamingManager.h"
#include "Sourcey/Spot/ISynchronizer.h"
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
	_env.streaming().InitializeStreamingSession.cleanup();
	_env.streaming().InitializeStreamingConnection.cleanup();
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
			Anionu::Event event("Motion Detected", env().session().name() + ": Motion detected on channel: " + _channel.name(), Anionu::Event::SpotLocal, Anionu::Event::High);
			env().createEvent(event);
			
			try {	
				// Start recording.
				startRecording();
			} 
			catch (...) {
				// Catch and swallow recording errors. 
				// If the Media Plugin is not installed we will make it here.
				// We do nothing here since the Recording Failed event will be 
				// triggered by the MediaManager.
			}

		break;
	}
}

	
bool SurveillanceMode::startRecording()
{	
	FastMutex::ScopedLock lock(_mutex); 
	_recordingAction.token = "";
	_recordingAction.encoder = NULL;
	_recordingAction.synchronize = _synchronizeVideos;
	Media::RecorderOptions options;
	env().media().initRecorderOptions(_channel, options);
		log() << "Started Recording: " << _recordingAction.token << endl;
	env().media().startRecording(_channel, options, _recordingAction);
		
	return true;
	/*
	//params.duration = time(0) + _segmentDuration;
	RecordingAction* info = env().media().startRecording(_channel, options);
	if (info) {
		info->synchronize = _synchronizeVideos;
		_recordingAction = RecordingAction(*info);
		//_recordingAction.encoder->StateChange += delegate(this, &SurveillanceMode::onEncoderStateChange);
		return true;
	}
	
	return false;
	*/
}


bool SurveillanceMode::stopRecording()
{
	FastMutex::ScopedLock lock(_mutex); 

	if (!_recordingAction.token.empty()) {
		//_recordingAction.encoder->StateChange -= delegate(this, &SurveillanceMode::onEncoderStateChange);
		env().media().stopRecording(_recordingAction.token);
		log() << "Stopped Recording: " << _recordingAction.token << endl;
		_recordingAction.token = "";
		_recordingAction.encoder = NULL;
		return true;
	}
	return false;
}


Token* SurveillanceMode::createStreamingToken(long duration)
{
	Token token(duration);
	FastMutex::ScopedLock lock(_mutex);
	_mediaTokens.push_back(token);
	_mediaTokens.back().start(); // start the token timer
	return &_mediaTokens.back();
}

	
Token* SurveillanceMode::getStreamingToken(const string& token)
{
	FastMutex::ScopedLock lock(_mutex);
	for (TokenList::iterator it = _mediaTokens.begin(); it != _mediaTokens.end(); ++it)
		if (*it == token)
			return &(*it);           
	return NULL;
}


bool SurveillanceMode::removeStreamingToken(const string& token)
{
	FastMutex::ScopedLock lock(_mutex);
	for (TokenList::iterator it = _mediaTokens.begin(); it != _mediaTokens.end(); ++it) {
		if (*it == token) {
			_mediaTokens.erase(it);
			return true;
		}
	}
	return false;
}

	
void SurveillanceMode::onInitializeStreamingSession(void*, IStreamingSession& session, bool& handled)
{
	log() << "Initialize Media Session: " << session.token() << endl;
	
	// Check if the session matches any of our tokens. 
	Token* token = getStreamingToken(session.token());
	if (token) {

		// Check the token is still valid.
		if (!token->expired()) {

			log() << "Creating Configuration Media Session: " << session.token() << endl;

			// Set the input format to GRAY8 for the encoder.
			Media::VideoCapture* video = _channel.videoCapture(true);
			AllocateOpenCVInputFormat(video, session.options().iformat);
			session.options().iformat.video.pixelFmt = "gray";

			// Attach our motion detector to the stream.
			// The motion detector will be the video source.
			session.stream().attach(&_motionDetector, 3, false);			
			session.StateChange += delegate(this, &SurveillanceMode::onStreamingSessionStateChange);

			// Let the session manager know we have completely
			// overridden session initialization.
			handled = true;
	
			// While the isConfiguring flag is set all motion
			// detector states will be ignored. The flag will 
			// be unset when the session is terminated.
			FastMutex::ScopedLock lock(_mutex); 
			_isConfiguring = true;
		}

		// Otherwise the session is no longer valid, so we throw 
		// an exception to terminate the session in error.
		else {
			log() << "Configuration Media Stream Timed Out" << endl;
			removeStreamingToken(session.token());
			throw Exception("Surveillance Mode media preview has timed out.");
		}
	}
}


void SurveillanceMode::onInitializeStreamingConnection(void*, IStreamingSession& session, ConnectionStream& connection, bool& handled)
{	
	log() << "Initialize Media Connection: " << session.token() << endl;		
	
	// Check if the session matches any of our tokens. 
	// If so then we can attach our custom packetizer
	// to this connection.
	if (getStreamingToken(session.token())) {

		log() << "Initializing Media Connection over " << session.options().protocol << endl;		
			
		FastMutex::ScopedLock lock(_mutex); 

		// Override the connection packetizer if we are using HTTP.
		// HTTP is preferable for the configuration session so we
		// can display surveillance metadata.
		// NOTE: We have not completely overridden the connection
		// stream creation, further encoders and packetizers may
		// be added depending on streaming configuration.
		if (session.options().protocol == "HTTP")
			connection.attach(new SurveillanceMultipartPacketizer(_motionDetector), 12, true);
	}
}


void SurveillanceMode::onStreamingSessionStateChange(void* sender, StreamingState& state, const StreamingState&)
{
	IStreamingSession* session = reinterpret_cast<IStreamingSession*>(sender); 
			
	log() << "Configuration Media Session State Changed: " << state << endl;

	// Remove the configuration media session token 
	// reference when the session closes or times out.
	if (state.id() == StreamingState::Terminating) {
		session->StateChange -= delegate(this, &SurveillanceMode::onStreamingSessionStateChange);
				
		removeStreamingToken(session->token());

		// Live configuration is over, life as normal.
		FastMutex::ScopedLock lock(_mutex); 
		_isConfiguring = false;
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


void SurveillanceMode::buildConfigForm(Symple::Form& form, Symple::FormElement& element, bool defaultScope)
{
	log() << "Creating Config Form" << endl;
	//log() << "Creating Config Form: " << defaultScope << endl;
	//log() << "Creating Config Form: " << _config.getScopedKey("MotionThreshold", defaultScope) << endl;
	//log() << "Creating Config Form: " << _config.getDafaultKey("MotionThreshold") << endl;
	 

	Symple::FormField field;

	// When configuring at channel scope we create a media element
	// which will display live changes as the channel is configured.
	// This is achieved by adding a "media" type field with a locally
	// generated media session token as the value. When a media session
	// request is received matching our token we create the video stream.
	if (!form.partial() && !defaultScope) {		
		Token* token = createStreamingToken();

		element.setHint(
			"This form enables you to configure your surveillance and motion detection settings in real-time. "
			"Use the help icons below for more information about each field. "
		);

		field = element.addField("media", _config.getChannelKey("Preview"), "Motion Preview");

		field.setHint(
			"The video above displays live changes as you modify the form below. "
			"Keep an eye on the motion levels and motion detector state; "
			"if motion levels exceed the threshold then the alarm will trigger. "
			"Note that the video may be slightly delayed. "
		);
		field.setValue(token->id());	
	}
	else {	
		element.setHint(
			"This form enables you to configure the default settings for Surveillance Mode. "
			"Any settings configured here may be overridden on a per channel basis (see Channel Configuration)."
		);
	}

	field = element.addField("number", _config.getScopedKey("MotionThreshold", defaultScope), "Motion Threshold");	
	field.setHint(
		"The 'Motion Threshold' determines the sensitivity of the motion detector. "
		"Motion is detected if the 'Motion Level' exceeds the 'Motion Threshold' in any given frame."
	);
	field.setValue(_config.getInt("MotionThreshold", _motionDetector.options().motionThreshold, defaultScope));
	if (!defaultScope)
		field.setLive(true);

	field = element.addField("number", _config.getScopedKey("PreSurveillanceDelay", defaultScope), "Pre Surveillance Mode Delay");	
	field.setHint(
		"This is the delay time (in seconds) before motion detection will actually commence after activating Surveillance Mode. "
		"This should be set to the amount of time you need to vacate the room/premises after mode activation."
	);
	field.setValue(_config.getInt("PreSurveillanceDelay", _motionDetector.options().preSurveillanceDelay, defaultScope));
	//field.setValue(_motionDetector.options().preSurveillanceDelay);
	if (!defaultScope)
		field.setLive(true);

	field = element.addField("number", _config.getScopedKey("MinTriggeredDuration", defaultScope), "Min Triggered Duration");
	field.setHint(
		"This is minimum duration of time that the motion detector can remain in the 'Triggered' state. "
		"Also, each time motion is detected while in the 'Triggered' state the timer will be extended by 'Min Triggered Duration' seconds."
	);
	field.setValue(_config.getInt("MinTriggeredDuration", _motionDetector.options().minTriggeredDuration, defaultScope));
	//field.setValue(_motionDetector.options().minTriggeredDuration);
	if (!defaultScope)
		field.setLive(true);

	field = element.addField("number", _config.getScopedKey("MaxTriggeredDuration", defaultScope), "Max Triggered Duration");	
	field.setHint(
		"This is the maximum amount of time that the motion detector can remain in the 'Triggered' state. "
		"In turn it also controls the maximum duration of any videos recorded in Surveillance Mode. "
		"If the 'Max Triggered Duration' is reached, any videos will stop recording and the system will wait "
		"for the 'Post Triggered Delay' duration of time before motion detection will recommence."
	);
	field.setValue(_config.getInt("MaxTriggeredDuration", _motionDetector.options().maxTriggeredDuration, defaultScope));
	//field.setValue(_motionDetector.options().maxTriggeredDuration);
	if (!defaultScope)
		field.setLive(true);

	field = element.addField("number", _config.getScopedKey("PostTriggeredDelay", defaultScope), "Post Triggered Delay");	
	field.setHint(
		"This is the delay time (in seconds) after the motion detector's 'Triggered' state has ended before motion detection will recommence."
	);
	field.setValue(_config.getInt("PostTriggeredDelay", _motionDetector.options().postMotionEndedDelay, defaultScope));
	//field.setValue(_motionDetector.options().postMotionEndedDelay);
	if (!defaultScope)
		field.setLive(true);

	field = element.addField("number", _config.getScopedKey("StableMotionNumFrames", defaultScope), "Stable Motion Frames");
	field.setHint(
		"In order to avoid false alarms the motion detector must detect a 'Stable Motion Frames' of motion frames before the alarm is triggered. "
		"This is calculated as follows; if motion is detected on a 'Stable Motion Frames' number of frames before a 'Motion Lifetime' duration of time expires then the alarm will trigger."    
	);
	field.setValue(_config.getInt("StableMotionNumFrames", _motionDetector.options().stableMotionNumFrames, defaultScope));
	//field.setValue(_motionDetector.options().stableMotionNumFrames);
	if (!defaultScope)
		field.setLive(true);

	field = element.addField("number", _config.getScopedKey("StableMotionLifetime", defaultScope), "Stable Motion Lifetime");	
	field.setHint(
		"The duration (in seconds) in which motion pixels are remembered by the system. "
		"This setting directly affects how long motion frames remain valid. " 
		"The 'Motion Lifetime' value works as follows; if a 'Stable Motion Frames' "
		"of frames exceeding the 'Motion Threshold' are detected within a 'Motion Lifetime' "
		"duration of time then the alarm will trigger." 
	);
	field.setValue(_config.getInt("StableMotionLifetime", _motionDetector.options().stableMotionLifetime, defaultScope));
	//field.setValue(_motionDetector.options().stableMotionLifetime);
	if (!defaultScope)
		field.setLive(true);

	// Enable Video Synchronization
	field = element.addField("boolean", _config.getScopedKey("SynchronizeVideos", defaultScope), "Enable Video Synchronization");	
	field.setHint(
		"Do you want to upload / synchronize recorded videos with your Anionu account?"
	);
	field.setValue(_config.getBool("SynchronizeVideos", _synchronizeVideos, defaultScope));
	//field.setValue(_synchronizeVideos);	
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


void SurveillanceMode::printInformation(ostream& s) 
{
	s << "<h2>About Surveillance Mode</h2>";
	s << "<p>Surveillance Mode provides Spot with advanced motion detection capabilities and notifications. ";
	
	s << "<h2>Using Surveillance Mode</h2>";
	s << "<p>Once Surveillance Mode is configured and enabled Spot will begin detecting motion on the current channel.</p>";
	s << "<p>When motion is detected two things will happen:</p>";
	s << "<ul><li>A video will be recorded and optionally synchronized with your Anionu account.</li>"; 
	s << "<ul><li>A 'Motion Detected' event will be created which will trigger notifications.</li>";
}


} } // namespace Sourcey::Spot