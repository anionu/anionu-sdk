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
	env().streaming().InitializeStreamingSession += delegate(this, &SurveillanceMode::onInitializeStreamingSession);
	env().streaming().InitializeStreamingConnection += delegate(this, &SurveillanceMode::onInitializeStreamingConnection);
}


void SurveillanceMode::uninitialize() 
{
	log() << "UnInitializing" << endl;

	// XXX: Delegates added across the process boundaries
	// can caused the SignalBase to crash when cleanup() 
	// is called from the other side, so we call cleanup() here.
	env().streaming().InitializeStreamingSession.detachAll(this);
	env().streaming().InitializeStreamingSession.cleanup();
	env().streaming().InitializeStreamingConnection.detachAll(this);
	env().streaming().InitializeStreamingConnection.cleanup();
}


void SurveillanceMode::loadConfig()
{
	log() << "Loading Config: " << _channel.name() << endl;	
	FastMutex::ScopedLock lock(_mutex); 
	MotionDetector::Options& o = _motionDetector.options();
	o.motionThreshold = _config.getInt("MotionThreshold", 15000);			// 15000 [50 - 100000]
	o.minPixelValue = _config.getInt("MinPixelValue", 30);					// 30 [0 - 255]
	o.preSurveillanceDelay = _config.getInt("PreSurveillanceDelay", 0);		// 0 seconds
	o.postMotionEndedDelay = _config.getInt("PostTriggeredDelay", 0);		// 0 seconds
	o.minTriggeredDuration = _config.getInt("MinTriggeredDuration", 5);		// 5 seconds
	o.maxTriggeredDuration = _config.getInt("MaxTriggeredDuration", 10);	// 60 seconds (1 min)
	o.stableMotionNumFrames = _config.getInt("StableMotionFrames", 3);		// 3 frames
	o.stableMotionLifetime = _config.getDouble("StableMotionLifetime", 1.0);// 1 second
	o.captureFPS = _config.getInt("CaptureFPS", 10);						// 10 frames per second
	_synchronizeVideos = _config.getBool("SynchronizeVideos", true);		// synchronize by default
}


void SurveillanceMode::activate() 
{
	log() << "Activating" << endl;	
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
	log() << "Activating: OK" << endl;
}


void SurveillanceMode::deactivate() 
{
	log() << "Deactivating" << endl;
	stopRecording();
	stopMotionDetector();
	IMode::deactivate();
	log() << "Deactivating: OK" << endl;
}


bool SurveillanceMode::startMotionDetector()
{
	FastMutex::ScopedLock lock(_mutex); 
	if (_motionDetector.isRunning()) {
		log("warn") << "Cannot start, the motion detector is already running" << endl;
		return false;
	}

	_motionDetector.StateChange += delegate(this, &SurveillanceMode::onMotionStateChange);

	// Setup our packet stream with a video capture
	// feeding into the motion detector.
	Media::VideoCapture* video = _channel.videoCapture(true);		
	_motionStream.attach(video, false);	
	_motionStream.attach(&_motionDetector, 1, false);
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
		
	log("warn") << "Cannot stop, the motion detector is not running" << endl;
	//_motionStream.close();
	return false;
}


void SurveillanceMode::onMotionStateChange(void* sender, Media::MotionDetectorState& state, const Media::MotionDetectorState&)
{
	log() << "Motion State Changed: " << state.toString() << endl;

	{
		// Discard surveillance events while configuring
		FastMutex::ScopedLock lock(_mutex); 
		if (_isConfiguring)
			return;
	}

	switch (state.id()) {
		case Media::MotionDetectorState::Idle: 
			break;
		case Media::MotionDetectorState::Waiting:

			// Stop recording
			// Always make this call just to be sure
			stopRecording();
		break;
		case Media::MotionDetectorState::Vigilant: 
			break;
		case Media::MotionDetectorState::Triggered:

			// Create a Motion Detected event via the Anionu API to
			// notify account users.
			Anionu::Event event("Motion Detected", 
				env().session().name() + ": Motion detected on channel: " + _channel.name(),
				Anionu::Event::SpotLocal, Anionu::Event::High);
			env().createEvent(event);
			
			try {	
				// Start recording.
				startRecording();
			} 
			catch (...) {
				// Catch and swallow recording errors. 
				// If the Media Plugin is not installed we will make it
				// here. We do nothing here since the "Recording Failed"
				// event will be triggered by the MediaManager.
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
		
	//_recordingAction.encoder->StateChange += delegate(this, &SurveillanceMode::onEncoderStateChange);
	return true;
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
	
	// Check if the session matches any available tokens 
	Token* token = getStreamingToken(session.token());
	if (token) {

		// Ensure token validity
		if (!token->expired()) {
			log() << "Creating Configuration Media Session: " << session.token() << endl;

			// Set the input format to GRAY8 for the encoder
			Media::VideoCapture* video = _channel.videoCapture(true);
			AllocateOpenCVInputFormat(video, session.options().iformat);
			session.options().iformat.video.pixelFmt = "gray";

			// Attach motion detector to the stream.
			// The motion detector acts as the video source.
			session.stream().attach(&_motionDetector, 3, false);			
			session.StateChange += delegate(this, &SurveillanceMode::onStreamingSessionStateChange);

			// Let the session manager know we have completely overridden 
			// session initialization.
			handled = true;
	
			// While the isConfiguring flag is set all motion detector states
			// will be ignored. The flag will be unset when the media session
			// is terminated.
			FastMutex::ScopedLock lock(_mutex); 
			_isConfiguring = true;
		}

		// Otherwise the session is no longer valid, so we throw an exception 
		// to terminate the session in error.
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
	
	// Check if the session request matches any of our tokens. 
	// If so then we can attach our custom packetizer to this connection.
	if (getStreamingToken(session.token())) {

		// Override the connection packetizer if we are using HTTP.
		// HTTP is the preferable transport for configuration sessions because
		// we can prepend surveillance metadata using HTTP headers.
		// NOTE: We have not completely overridden the connection stream
		// creation, further encoders and packetizers may be added depending
		// on streaming configuration.
		FastMutex::ScopedLock lock(_mutex); 
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
		
	ModeConfiguration config = this->config();
	Symple::FormField field;

	// Create a video media element which enables the user to preview motion changes
	// in real time as the channel is configured. This is achieved by adding a "media"
	// type field with is associated with a media session token. When an incoming media
	// session request subscribes to the token we use our motion detector as the streaming
	// source (see onInitializeStreamingConnection). Media streaming sessions are only 
	// available when configuring at channel scope ie. defaultScope == false.
	// No surveillance events will be triggered while media sessions are active.
	if (!form.partial() && !defaultScope) {		
		element.setHint(
			"This form enables you to configure the settings for Surveillance Mode. "
			"The video below enables you to visualize motion in real-time as Spot sees it. "
			"You can use the video as a guide while you fine tune the motion detector settings below. "
			"Be sure to read the 'Mode Information' page for tips on optimizing your settings for the best results."
		);

		Token* token = createStreamingToken();
		field = element.addField("media", config.getChannelKey("Preview"), "Live Motion Preview");
		field.setHint(
			"Keep an eye on the motion levels and motion state in the top left hand corner of the video. "
			"If motion level exceeds the motion threshold then the motion detector will enter the 'Triggered' state. "			
			"Note that the video may be slightly delayed, depending on your connection speed. "
		);
		field.setValue(token->id());	
	}
	else {	
		element.setHint(
			"This form enables you to configure the default settings for Surveillance Mode. "
			"Any settings configured here may be overridden on a per channel basis (see Channel Configuration). "
			"Be sure to read the 'Mode Information' page for tips on optimizing your settings for the best results. "
		);
	}

	field = element.addField("number", config.getScopedKey("MotionThreshold", defaultScope), "Motion Threshold");	
	field.setHint(
		"This setting directly affects the sensitivity of the motion detector. "
		"A lower value means greater sensitivity; If the 'Motion Level' exceeds the 'Motion Threshold' then motion is detected."
	);
	field.setValue(config.getInt("MotionThreshold", defaultScope));
	if (!defaultScope)
		field.setLive(true);

	field = element.addField("number", config.getScopedKey("PreSurveillanceDelay", defaultScope), "Pre Surveillance Delay");	
	field.setHint(
		"This is the delay time (in seconds) before motion detection will actually commence. "
		"Set this to the amount of time you need to vacate the surveilled area after activating Surveillance Mode."
	);
	field.setValue(config.getInt("PreSurveillanceDelay", defaultScope));
	if (!defaultScope)
		field.setLive(true);

	field = element.addField("number", config.getScopedKey("MinTriggeredDuration", defaultScope), "Minimum Triggered Duration");
	field.setHint(
		"This is minimum duration of time that the motion detector can remain in the 'Triggered' state. "
		"This is also the minimum duration of any recorded videos."
	);
	field.setValue(config.getInt("MinTriggeredDuration", defaultScope));
	if (!defaultScope)
		field.setLive(true);

	field = element.addField("number", config.getScopedKey("MaxTriggeredDuration", defaultScope), "Maximum Triggered Duration");	
	field.setHint(
		"This is the maximum amount of time that the motion detector can remain in the 'Triggered' state. "
		"This is also the maximum duration of any recorded videos. "
	);
	field.setValue(config.getInt("MaxTriggeredDuration", defaultScope));
	if (!defaultScope)
		field.setLive(true);

	field = element.addField("number", config.getScopedKey("StableMotionFrames", defaultScope), "Stable Motion Frames");
	field.setHint(
		"In order to avoid false alarms, the motion detector must detect a certain number of motion frames inside a time interval before the alarm will trigger. "
		"This setting determines how many motion frames to detect before motion is considered stable. "
		"Recommended setting: 3 (3 frames)"
	);
	field.setValue(config.getInt("StableMotionFrames", defaultScope));
	if (!defaultScope)
		field.setLive(true);

	field = element.addField("number", config.getScopedKey("StableMotionLifetime", defaultScope), "Stable Motion Lifetime");	
	field.setHint(
		"This is the duration of time (in seconds) that motion frames remain valid. "
		"See 'Stable Motion Frames' for further explainiation about this setting. "
		"Recommended setting: 1 (1 second)"
	);
	field.setValue(config.getInt("StableMotionLifetime", defaultScope));
	if (!defaultScope)
		field.setLive(true);

	// Disabling this setting as it is not completely necessary, and may overcomplicate things.
	/*
	field = element.addField("number", config.getScopedKey("PostTriggeredDelay", defaultScope), "Post Triggered Delay");	
	field.setHint(
		"This is the duration of time (in seconds) to wait before motion detection will recommence after the 'Triggered' state has ended. "
		"During this delay, the motion detector will sit in the 'Waiting' state. "
		"Once this delay is complete, the motion detector will enter the 'Vigilant' state again, and begin detecting motion."
	);
	field.setValue(config.getInt("PostTriggeredDelay", defaultScope));
	if (!defaultScope)
		field.setLive(true);
		*/

	// Enable Video Synchronization
	field = element.addField("boolean", config.getScopedKey("SynchronizeVideos", defaultScope), "Synchronize Videos");	
	field.setHint(
		"Do you want to synchronize/upload recorded videos to your online account? "
		"You can access synchronized videos from your dashboard." //are available 
	);
	field.setValue(config.getBool("SynchronizeVideos", defaultScope));
}


void SurveillanceMode::parseConfigForm(Symple::Form& form, Symple::FormElement& element)
{
	log() << "Parsing Config Form" << endl;
	
	Symple::FormField field;
	ModeConfiguration config = this->config();
	IEnvironment& env = this->env();
	
	field = element.getField("Surveillance Mode.MotionThreshold", true);
	if (field.valid()) {
		int value = field.intValue();
		if (value < 50 ||
			value > 100000) {
			field.setError("Must be a number between 50 and 100000.");
		}
		else
			env.config().setInt(field.id(), value);
	}
	
	field = element.getField("Surveillance Mode.PreSurveillanceDelay", true);
	if (field.valid())
		env.config().setInt(field.id(), field.intValue());

	/*
	field = element.getField("Surveillance Mode.PostTriggeredDelay", true);
	if (field.valid())
		env.config().setInt(field.id(), field.intValue());
		*/

	field = element.getField("Surveillance Mode.MinTriggeredDuration", true);
	if (field.valid())
		env.config().setInt(field.id(), field.intValue());

	field = element.getField("Surveillance Mode.MaxTriggeredDuration", true);
	if (field.valid())
		env.config().setInt(field.id(), field.intValue());

	field = element.getField("Surveillance Mode.StableMotionFrames", true);
	if (field.valid())
		env.config().setInt(field.id(), field.intValue());

	field = element.getField("Surveillance Mode.StableMotionLifetime", true);
	if (field.valid())
		env.config().setInt(field.id(), field.intValue());

	field = element.getField("Surveillance Mode.SynchronizeVideos", true);
	if (field.valid())
		env.config().setBool(field.id(), field.boolValue());

	loadConfig();
}


string SurveillanceMode::infoFile() 
{	
	return "plugins/SurveillanceModePlugin/SurveillanceMode.md";
	/*
	std::ifstream infile;
	//this->path()
	infile.open(path().data(), std::ifstream::in);
	if (!infile.is_open())		
		throw Poco::OpenFileException("Cannot open input file: " + path);

	JSON::Reader reader;
	bool res = reader.parse(infile, root);
	infile.close();
	s << 

		"<h1>About Surveillance Mode</h1>"
		"<p>Surveillance Mode provides Spot with motion detection capabilities. "
		"Surveillance mode is ideal for when you are away from the surveilled premises, and want to protect yourself against unwanted intruders.</p>"

		"<p class=\"note\">The text below includes references to Surveillance Mode configuration options. "
		"There references are capitalized and enclosed in inverted commas. "
		"See the Mode Configuration page for a detailed description of each option.</p>"
	
		"<h2>How It Works</h2>"
		"<p>When Surveillance Mode is activated for a channel, it will begin detecting motion after the 'Pre Surveillance Delay' has elapsed. "
		"The 'Pre Surveillance Delay' option gives you time to leave the premesis after activating Surveilance Mode.</p>"
		"<p>When motion is detected the process is as follows:</p>"
		"<ul>"
		"<li>A 'Motion Detected' event will be created which will trigger notifications to you or a designated third party. "
		"See the 'Event Notifiers' page of your dashboard to configure who receives notifications.</li>"
		"<li>A video will be recorded during the interval of motion, and optionally synchronized with your Anionu account. "
		"If you want videos to be synchronized be sure to to enable the 'SynchronizeVideos' option on the 'Mode Configuration' page."
		"</li>"
		"</ul>"

	
	s << "<h2>Best Configuration Tips</h2>";
	
	configured and Spot will begin detecting motion on the current 
	s << "<p>Once Surveillance Mode is configured and enabled Spot will begin detecting motion on the current channel.</p>";
	s << "<p>When motion is detected two things will happen:</p>";
	s << "<ul><li>A video will be recorded and optionally synchronized with your Anionu account.</li>"; 
	s << "<ul><li>A 'Motion Detected' event will be created which will trigger notifications.</li>";

	s << "Surveillance mode is ideal for when you are away from your surveilled premises, and want to be notified about any trespasser. ";
	he Surveillance plugin provides Spot with motion detection capabilities. Surveillance mode is perfect for when you are away from your surveilled premises. The plugin enables you to configure motion detection sensitivities, be notified by SMS when motion is detected, and record videos during intervals of motion.
	
		//"This setting directly affects how long motion frames remain valid. " 
		//"The 'Motion Lifetime' value works as follows; if a 'Stable Motion Frames' "
		//"of frames exceeding the 'Motion Threshold' are detected within a 'Motion Lifetime' "
		//"duration of time then the alarm will trigger." 
		*/
}


} } // namespace Sourcey::Spot