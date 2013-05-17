#include "SurveillanceMode.h"
#include "SurveillanceMultipartPacketizer.h"
#include "Anionu/Spot/API/IChannel.h"
#include "Anionu/Spot/API/IEnvironment.h"
#include "Anionu/Spot/API/ISession.h"
#include "Anionu/Spot/API/IStreamingManager.h"
#include "Anionu/Spot/API/IEventManager.h"
#include "Anionu/Spot/API/ISynchronizer.h"
#include "Anionu/Event.h"
#include "Sourcey/IConfiguration.h"


using namespace std;
using namespace Poco;
using namespace Scy::Anionu::Spot::API;


namespace Scy {
namespace Anionu { 
namespace Spot {
	using namespace API;


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
	env().streaming().SetupStreamingSession += delegate(this, &SurveillanceMode::onSetupStreamingSession);
	env().streaming().SetupStreamingConnection += delegate(this, &SurveillanceMode::onSetupStreamingConnection);
}


void SurveillanceMode::uninitialize() 
{
	log() << "UnInitializing" << endl;
	env().streaming().SetupStreamingSession.detach(this);
	env().streaming().SetupStreamingConnection.detach(this);
}


void SurveillanceMode::loadConfig()
{
	FastMutex::ScopedLock lock(_mutex); 
	log() << "Loading Config: " << _channel.name() << endl;	
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
		_motionStream.close();
		return true;
	}
		
	log("warn") << "Cannot stop, the motion detector is not running" << endl;
	return false;
}


void SurveillanceMode::onMotionStateChange(void* sender, Anionu::MotionDetectorState& state, const Anionu::MotionDetectorState&)
{
	log() << "Motion State Changed: " << state.toString() << endl;
	{
		// Discard surveillance events while configuring
		FastMutex::ScopedLock lock(_mutex); 
		if (_isConfiguring)
			return;
	}

	switch (state.id()) {
		case Anionu::MotionDetectorState::Idle: 
			break;
		case Anionu::MotionDetectorState::Waiting:

			// Stop recording
			// Always make this call just to be sure
			stopRecording();
		break;
		case Anionu::MotionDetectorState::Vigilant: 
			break;
		case Anionu::MotionDetectorState::Triggered:

			// Create a Motion Detected event via the Anionu API to
			// notify account users.
			Anionu::Event event("Motion Detected", 
				env().session().name() + ": Motion detected on channel: " + _channel.name(),
				Anionu::Event::High, Anionu::Event::SpotLocal);
			env().events().createEvent(event);
			
			try {	
				// Start recording.
				startRecording();
			} 
			catch (...) {
				// Catch and swallow recording errors. 
				// Nothing to do here since the "Recording Failed"
				// event will be triggered by the MediaManager.
			}
		break;
	}
}

	
bool SurveillanceMode::startRecording()
{	
	FastMutex::ScopedLock lock(_mutex); 
	log() << "Start Recording" << endl;
	API::RecordingOptions options = env().media().getRecordingOptions(_channel.name());
	options.synchronize = _synchronizeVideos;
	env().media().startRecording(options);
	log() << "Started Recording: " << options.token << endl;
	_recordingToken = options.token;
	return true;
}


bool SurveillanceMode::stopRecording()
{
	FastMutex::ScopedLock lock(_mutex); 

	if (!_recordingToken.empty()) {
		env().media().stopRecording(_recordingToken);
		log() << "Stopped Recording: " << _recordingToken << endl;
		_recordingToken = "";
		return true;
	}
	return false;
}


Token* SurveillanceMode::createStreamingToken(long duration)
{
	Token token(duration);
	FastMutex::ScopedLock lock(_mutex);
	_streamingTokens.push_back(token);
	_streamingTokens.back().start(); // start the token timer
	return &_streamingTokens.back();
}

	
Token* SurveillanceMode::getStreamingToken(const string& token)
{
	FastMutex::ScopedLock lock(_mutex);
	for (TokenList::iterator it = _streamingTokens.begin(); it != _streamingTokens.end(); ++it)
		if (*it == token)
			return &(*it);           
	return NULL;
}


bool SurveillanceMode::removeStreamingToken(const string& token)
{
	FastMutex::ScopedLock lock(_mutex);
	for (TokenList::iterator it = _streamingTokens.begin(); it != _streamingTokens.end(); ++it) {
		if (*it == token) {
			_streamingTokens.erase(it);
			return true;
		}
	}
	return false;
}

	
void SurveillanceMode::onSetupStreamingSession(void*, API::IStreamingSession& session, bool& handled)
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


void SurveillanceMode::onSetupStreamingConnection(void*, API::IStreamingSession& session, API::ConnectionStream& connection, bool& handled)
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


void SurveillanceMode::onStreamingSessionStateChange(void* sender, API::StreamingState& state, const API::StreamingState&)
{  
	API::IStreamingSession* session = reinterpret_cast<API::IStreamingSession*>(sender); 
			
	log() << "Configuration Media Session State Changed: " << state << endl;

	// Remove the configuration media session token
	// reference when the session closes or times out.
	if (state.equals(API::StreamingState::Terminating)) {
		session->StateChange -= delegate(this, &SurveillanceMode::onStreamingSessionStateChange);
				
		removeStreamingToken(session->token());

		// Live configuration is over, life as normal.
		FastMutex::ScopedLock lock(_mutex); 
		_isConfiguring = false;
	}
}


// ---------------------------------------------------------------------
//
bool SurveillanceMode::isConfigurable() const
{	
	return true;
}


bool SurveillanceMode::hasParsableFields(Symple::Form& form) const
{
	return form.hasField(".Surveillance Mode.", true);
}


void SurveillanceMode::buildForm(Symple::Form& form, Symple::FormElement& element) //, bool defaultScope
{		
	Symple::FormField field;
	ScopedConfiguration config = this->config();
	bool defaultScope = element.id().find("channels.") == 0;
	this->log() << "Building Form: " << element.id() << endl;	 

	// Create a video media element which enables the user to preview motion changes
	// in real time as the channel is configured. This is achieved by adding a "media"
	// type field with is associated with a media session token. When an incoming media
	// session request subscribes to the token we use our motion detector as the streaming
	// source (see onSetupStreamingConnection). Media streaming sessions are only 
	// available when configuring at channel scope ie. defaultScope == false.
	// No surveillance events will be triggered while media sessions are active.
	if (!form.partial() && !defaultScope) {		
		element.setHint(
			"This form enables you to configure the settings for Surveillance Mode. "
			"The video below enables you to visualize motion in real-time as Spot sees it. "
			"You can use the video as a guide while you fine tune the motion detector settings below. "
			"Be sure to read the 'Mode Information' page for tips on optimizing your settings for the best results."
		);

		Token* token = this->createStreamingToken();
		field = element.addField("media", config.getModuleScope("Preview"), "Live Motion Preview");
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


void SurveillanceMode::parseForm(Symple::Form& form, Symple::FormElement& element)
{
	this->log() << "Parsing Config Form" << endl;	
	Symple::FormField field;
	ScopedConfiguration config = this->config();
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

	this->loadConfig();
}


string SurveillanceMode::helpFile() 
{	
	return "plugins/SurveillanceModePlugin/SurveillanceMode.md";
}


} } } // namespace Scy::Anionu::Spot