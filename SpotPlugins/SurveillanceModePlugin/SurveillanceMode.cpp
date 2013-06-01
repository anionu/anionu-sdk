#include "SurveillanceMode.h"
#include "SurveillanceMultipartPacketizer.h"
#include "Anionu/Spot/API/IClient.h"
#include "Anionu/Spot/API/IChannel.h"
#include "Anionu/Spot/API/IChannelManager.h"
#include "Anionu/Spot/API/IEnvironment.h"
#include "Anionu/Spot/API/IStreamingManager.h"
#include "Anionu/Spot/API/ISynchronizer.h"
#include "Anionu/Spot/API/Util.h"
#include "Anionu/Event.h"
#include "Sourcey/IConfiguration.h"
#include "Sourcey/Symple/Form.h"


using namespace std;
using namespace Poco;
using namespace Scy::Anionu::Spot::API;


namespace Scy {
namespace Anionu { 
namespace Spot {
	using namespace API;


SurveillanceMode::SurveillanceMode(IEnvironment& env, const string& channel) :
	IModule(&env), _channel(channel), _isActive(false), _isConfiguring(false)
{
	log("Creating");
	loadConfig();	
	env.streaming().InitStreamingSession += delegate(this, &SurveillanceMode::onInitStreamingSession);
	env.streaming().InitStreamingConnection += delegate(this, &SurveillanceMode::onInitStreamingConnection);
}


SurveillanceMode::~SurveillanceMode()
{	
	log("Destroying");	
	env()->streaming().InitStreamingSession.detach(this);
	env()->streaming().InitStreamingConnection.detach(this);
}


bool SurveillanceMode::activate() 
{
	log("Activating");	
	try {
		startMotionDetector();
	}
	catch (Exception& exc) {
		_error = exc.displayText();
		log("Activation failed: " + _error, "error");
		return false;
	}
	return true;
}


void SurveillanceMode::deactivate() 
{
	log("Deactivating");
	try {
		stopRecording();
		stopMotionDetector();
	}
	catch (Exception& exc) {
		log("Deactivation failed: " + exc.displayText(), "error");
	}
}


void SurveillanceMode::loadConfig()
{
	FastMutex::ScopedLock lock(_mutex); 
	ScopedConfiguration config = getModeConfiguration(this);
	log("Loading Config: " + _channel);	
	MotionDetector::Options& o = _motionDetector.options();
	o.motionThreshold = config.getInt("MotionThreshold", 15000);			// 15000 [50 - 100000]
	o.minPixelValue = config.getInt("MinPixelValue", 30);					// 30 [0 - 255]
	o.preSurveillanceDelay = config.getInt("PreSurveillanceDelay", 0);		// 0 seconds
	o.postMotionEndedDelay = config.getInt("PostTriggeredDelay", 0);		// 0 seconds
	o.minTriggeredDuration = config.getInt("MinTriggeredDuration", 5);		// 5 seconds
	o.maxTriggeredDuration = config.getInt("MaxTriggeredDuration", 10);		// 60 seconds (1 min)
	o.stableMotionNumFrames = config.getInt("StableMotionFrames", 3);		// 3 frames
	o.stableMotionLifetime = config.getDouble("StableMotionLifetime", 1.0);	// 1 second
	o.captureFPS = config.getInt("CaptureFPS", 10);							// 10 frames per second
	_synchronizeVideos = config.getBool("SynchronizeVideos", true);			// synchronize by default
}


void SurveillanceMode::startMotionDetector()
{
	FastMutex::ScopedLock lock(_mutex); 
	if (_motionDetector.isActive())
		throw Exception("Cannot start: Motion detector already active.");

	_motionDetector.StateChange += delegate(this, &SurveillanceMode::onMotionStateChange);

	// Get the video capture or throw an exception.
	Media::VideoCapture* video = env()->channels().getChannel(_channel)->videoCapture(true);	

	// Setup our packet stream with the video capture
	// feeding into the motion detector.
	_motionStream.attach(video, false);	
	_motionStream.attach(&_motionDetector, 1, false);
	_motionStream.start();
}


void SurveillanceMode::stopMotionDetector()
{
	FastMutex::ScopedLock lock(_mutex); 
	if (!_motionDetector.isActive())	
		throw Exception("Cannot stop: Motion detector not active.");

	_motionDetector.StateChange -= delegate(this, &SurveillanceMode::onMotionStateChange);
	_motionStream.close();
}

	
void SurveillanceMode::startRecording()
{	
	log("Start Recording");
	if (isRecording())
		throw Exception("Start recording failed: Recorder already active.");
	
	FastMutex::ScopedLock lock(_mutex); 
	RecordingOptions options = env()->media().getRecordingOptions(_channel);
	options.synchronizeVideo  = _synchronizeVideos;
	env()->media().startRecording(options);
	log("Recording Started: " + options.token);
	_recordingToken = options.token;
}


void SurveillanceMode::stopRecording()
{	
	log("Stop Recording");
	if (!isRecording())		
		throw Exception("Stop recording failed: Recorder not active.");

	FastMutex::ScopedLock lock(_mutex); 
	env()->media().stopRecording(_recordingToken, true);
	log("Recording Stopped: " + _recordingToken);
	_recordingToken = "";
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


void SurveillanceMode::onMotionStateChange(void* sender, Anionu::MotionDetectorState& state, const Anionu::MotionDetectorState&)
{
	log("Motion State Changed: " + state.toString());
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
			try {	
				// Stop recording if recording is active
				if (isRecording())
					stopRecording();
			} catch (...) {}
		break;
		case Anionu::MotionDetectorState::Vigilant: 
			break;
		case Anionu::MotionDetectorState::Triggered:

			// Create a Motion Detected event via the Anionu API to
			// notify account users.
			Anionu::Event event("Motion Detected", 
				env()->client().name() + ": Motion detected on channel: " + _channel,
				Anionu::Event::High, Anionu::Event::SpotLocal);
			env()->client().createEvent(event);
			
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

	
void SurveillanceMode::onInitStreamingSession(void*, IStreamingSession& session, bool& handled)
{
	log("Initialize Media Session: " + session.token());
	
	// Check if the session matches any available tokens 
	Token* token = getStreamingToken(session.token());
	if (token) {

		// Ensure token validity
		if (!token->expired()) {
			log("Creating Configuration Media Session: " + session.token());
			FastMutex::ScopedLock lock(_mutex); 

			// Get the video capture or throw an exception.
			Media::VideoCapture* video = env()->channels().getChannel(_channel)->videoCapture(true);	

			// Set the input format to GRAY8 for the encoder
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
			_isConfiguring = true;
		}

		// Otherwise the session is no longer valid, so we throw an exception 
		// to terminate the session in error.
		else {
			log("Configuration Media Stream Timed Out");
			removeStreamingToken(session.token());
			throw Exception("Surveillance Mode media preview has timed out.");
		}
	}
}


void SurveillanceMode::onInitStreamingConnection(void*, IStreamingSession& session, ConnectionStream& connection, bool& handled)
{	
	log("Initialize Media Connection: " + session.token());		
	
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
	log("Configuration Media Session State Changed: " + state.toString());

	// Remove the configuration media session token
	// reference when the session closes or times out.
	if (state.equals(StreamingState::Terminating)) {
		session->StateChange -= delegate(this, &SurveillanceMode::onStreamingSessionStateChange);
				
		removeStreamingToken(session->token());

		// Live configuration is over, life as normal.
		FastMutex::ScopedLock lock(_mutex); 
		_isConfiguring = false;
	}
}


bool SurveillanceMode::isActive() const
{
	FastMutex::ScopedLock lock(_mutex); 
	return _isActive;
}


bool SurveillanceMode::isRecording() const
{
	FastMutex::ScopedLock lock(_mutex);
	return !_recordingToken.empty();
}


const char* SurveillanceMode::errorMessage() const 
{ 
	FastMutex::ScopedLock lock(_mutex);
	return _error.empty() ? 0 : _error.data();
}


const char* SurveillanceMode::channelName() const
{
	FastMutex::ScopedLock lock(_mutex); 
	return _channel.data();
}


const char* SurveillanceMode::docFile() const
{	
	return "SurveillanceModePlugin/SurveillanceMode.md";
}


// ---------------------------------------------------------------------
//
void SurveillanceMode::buildForm(Symple::Form& form, Symple::FormElement& element) //, bool defaultScope
{		
	log("Building Form");	 
	Symple::FormField field;
	ScopedConfiguration config = getModeConfiguration(this);
	// Determine weather we are building the form at channel or
	// default scope as it will change the way we display the form.
	//
	//   Channel Scope: channels.[channelName].modes.[modeName]
	//   Defualt Scope: modes.[modeName]
	//
	bool defaultScope = element.id().find("channels.") == 0;

	// Create a video media element which enables the user to preview motion changes
	// in real time as the channel is configured. This is achieved by adding a "media"
	// type field with is associated with a media session token. When an incoming media
	// session request subscribes to the token we use our motion detector as the streaming
	// source (see onInitStreamingConnection). Media streaming sessions are only 
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
		field = element.addField("media", config.getScopedKey("Preview", defaultScope), "Live Motion Preview");
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
	log("Parsing Form");	 
	Symple::FormField field;
	
	field = element.getField("Surveillance Mode.MotionThreshold", true);
	if (field.valid()) {
		int value = field.intValue();
		if (value < 50 ||
			value > 100000) {
			field.setError("Must be a number between 50 and 100000.");
		}
		else
			env()->config().setInt(field.id(), value);
	}
	
	field = element.getField("Surveillance Mode.PreSurveillanceDelay", true);
	if (field.valid())
		env()->config().setInt(field.id(), field.intValue());

	/*
	field = element.getField("Surveillance Mode.PostTriggeredDelay", true);
	if (field.valid())
		env()->config().setInt(field.id(), field.intValue());
		*/

	field = element.getField("Surveillance Mode.MinTriggeredDuration", true);
	if (field.valid())
		env()->config().setInt(field.id(), field.intValue());

	field = element.getField("Surveillance Mode.MaxTriggeredDuration", true);
	if (field.valid())
		env()->config().setInt(field.id(), field.intValue());

	field = element.getField("Surveillance Mode.StableMotionFrames", true);
	if (field.valid())
		env()->config().setInt(field.id(), field.intValue());

	field = element.getField("Surveillance Mode.StableMotionLifetime", true);
	if (field.valid())
		env()->config().setInt(field.id(), field.intValue());

	field = element.getField("Surveillance Mode.SynchronizeVideos", true);
	if (field.valid())
		env()->config().setBool(field.id(), field.boolValue());

	loadConfig();
}


} } } // namespace Scy::Anionu::Spot




/*
bool SurveillanceMode::isConfigurable() const
{	
	return true;
}


bool SurveillanceMode::hasParsableFields(Symple::Form& form) const
{
	return form.hasField(".Surveillance Mode.", true);
}


IEnvironment* SurveillanceMode::env() const
{ 
	// Provide synchronization for environment pointer.
	Poco::FastMutex::ScopedLock lock(_mutex);
	assert(_env);
	return _env; 
}
*/