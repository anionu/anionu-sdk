//
// Anionu SDK
// Copyright (C) 2011, Anionu <http://anionu.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#include "surveillancemode.h"
#include "surveillancemultipartadapter.h"
#include "anionu/spot/api/client.h"
#include "anionu/spot/api/channel.h"
#include "anionu/spot/api/channelmanager.h"
#include "anionu/spot/api/environment.h"
#include "anionu/spot/api/streamingmanager.h"
#include "anionu/spot/api/synchronizer.h"
#include "anionu/spot/api/util.h"
#include "anionu/event.h"
#include "scy/configuration.h"
#include "scy/symple/form.h"


using std::endl;


namespace scy {
namespace anio { 
namespace spot {

	
const int kMotionThreshold = 15000;           // 15000 [50 - 100000]
const int kMinPixelValue = 30;                // 30 [0 - 255]
const int kPreSurveillanceDelay = 0;          // 0 seconds
const int kPostTriggeredDelay = 1;            // 1 seconds
const int kMinTriggeredDuration = 5;          // 5 seconds
const int kMaxTriggeredDuration = 10;         // 60 seconds (1 min)
const int kStableMotionFrames = 3;            // 3 frames
const double kStableMotionLifetime = 1.0;     // 1 second
const int kCaptureFPS = 10;                   // 10 frames per second
const bool kSynchronizeVideos = true;         // synchronize by default


SurveillanceMode::SurveillanceMode(api::Environment& env, const std::string& channel) :
	api::IModule(env), _channel(channel), _isActive(false), _isConfiguring(false), _tempMotionDetector(nullptr)
{
	DebugL << "Creating" << endl;
	loadConfig();	
	env.streaming().SetupStreamingSources += sdelegate(this, &SurveillanceMode::onSetupStreamingSources);
	env.streaming().SetupStreamingConnection += sdelegate(this, &SurveillanceMode::onSetupStreamingConnection);
}


SurveillanceMode::~SurveillanceMode()
{	
	DebugL << "Destroying" << endl;	
	assert(!isActive());
	env().streaming().SetupStreamingSources.detach(this);
	env().streaming().SetupStreamingConnection.detach(this);
	DebugL << "Destroying: OK" << endl;	
}


bool SurveillanceMode::activate() 
{
	DebugL << "Activating" << endl;	
	try {
		startMotionDetector();
		_isActive = true;
	}
	catch (std::exception& exc) {
		ErrorL << "Activation error: " << exc.what() << endl;
		_error.assign(exc.what());
		return false;
	}
	return true;
}


void SurveillanceMode::deactivate() 
{
	DebugL << "Deactivating" << endl;
	try {
		if (isRecording())
			stopRecording();
		stopMotionDetector(false);
		_isActive = false;
	}
	catch (std::exception& exc) {
		ErrorL << "Deactivation error: " << exc.what() << endl;
	}
}


void SurveillanceMode::loadConfig()
{
	Mutex::ScopedLock lock(_mutex); 
	DebugL << "Loading config: " << _channel << endl;	
	ScopedConfiguration config = getModeConfiguration(this);
	MotionDetector::Options& o = _motionDetector.options();
	o.motionThreshold = config.getInt("MotionThreshold", kMotionThreshold);
	o.minPixelValue = config.getInt("MinPixelValue", kMinPixelValue);
	o.preSurveillanceDelay = config.getInt("PreSurveillanceDelay", kPreSurveillanceDelay);
	o.postMotionEndedDelay = config.getInt("PostTriggeredDelay", kPostTriggeredDelay);
	o.minTriggeredDuration = config.getInt("MinTriggeredDuration", kMinTriggeredDuration);
	o.maxTriggeredDuration = config.getInt("MaxTriggeredDuration", kMaxTriggeredDuration);
	o.stableMotionNumFrames = config.getInt("StableMotionFrames", kStableMotionFrames);
	o.stableMotionLifetime = config.getDouble("StableMotionLifetime", kStableMotionLifetime);
	o.captureFPS = config.getInt("CaptureFPS", kCaptureFPS);	
	_synchronizeVideos = config.getBool("SynchronizeVideos", kSynchronizeVideos);

	if (_tempMotionDetector)
		_tempMotionDetector->setOptions(o);
}


void SurveillanceMode::startMotionDetector(bool whiny)
{
	try {
		Mutex::ScopedLock lock(_mutex); 
		if (_motionDetector.isActive())
		//if (_motionStream.active())
			throw std::runtime_error("Cannot start the motion detector because it's already active.");
		
		_motionDetector.StateChange += sdelegate(this, &SurveillanceMode::onMotionStateChange);

		// Get the video capture or throw an exception.
		av::VideoCapture::Ptr video(env().channels().getChannel(_channel)->videoCapture(true));	

		// Setup our packet stream with the video capture
		// feeding into the motion detector.
		_motionStream.attachSource<av::VideoCapture>(video, true);	
		_motionStream.attach(&_motionDetector, 1, false);
		_motionStream.start();
	}
	catch (std::exception& exc) {
		WarnL << "Cannot start the motion detector: " << exc.what() << endl;
		if (whiny) throw exc;
	}
}


void SurveillanceMode::stopMotionDetector(bool whiny)
{
	try {
		DebugL << "Stopping motion detector" << endl;	

		Mutex::ScopedLock lock(_mutex); 
		if (!_motionDetector.isActive())
		//if (!_motionStream.active())
			throw std::runtime_error("Cannot stop the motion detector because it's not active.");
		
		_motionDetector.StateChange -= sdelegate(this, &SurveillanceMode::onMotionStateChange);
		_motionStream.close();
		_motionStream.reset();
	}
	catch (std::exception& exc) {
		WarnL << "Cannot start the motion detector: " << exc.what() << endl;
		if (whiny) throw exc;
	}
}

	
void SurveillanceMode::startRecording(bool whiny)
{	
	DebugL << "Start recording" << endl;
	try {
		if (isRecording())
			throw std::runtime_error("Cannot start recording because recording is already active.");
	
		Mutex::ScopedLock lock(_mutex); 
		api::EncoderOptions options = env().media().getEncoderOptions(_channel);
		options.synchronizeVideo  = _synchronizeVideos;
		env().media().startRecording(options);
		DebugL << "Recording started: " << options.token << endl;
		_recordingToken = options.token;
	}
	catch (std::exception& exc) {
		WarnL << "Cannot start recording: " << exc.what() << endl;
		if (whiny) throw exc;
	}
}


void SurveillanceMode::stopRecording(bool whiny)
{	
	DebugL << "Stop recording" << endl;
	try {
		if (!isRecording())
			throw std::runtime_error("Cannot stop recording because recording is not active.");

		Mutex::ScopedLock lock(_mutex); 
		env().media().stopRecording(_recordingToken, true);
		DebugL << "Recording stopped: " << _recordingToken << endl;
		_recordingToken = "";
	}
	catch (std::exception& exc) {
		WarnL << "Cannot stop recording: " << exc.what() << endl;
		if (whiny) throw exc;
	}
}


TimedToken* SurveillanceMode::createStreamingToken(long duration)
{
	TimedToken token(duration);
	Mutex::ScopedLock lock(_mutex);
	_streamingTokens.push_back(token);
	_streamingTokens.back().start(); // start the token timer
	return &_streamingTokens.back();
}

	
TimedToken* SurveillanceMode::getStreamingToken(const std::string& token)
{
	Mutex::ScopedLock lock(_mutex);
	for (auto it = _streamingTokens.begin(); it != _streamingTokens.end(); ++it) {
		if (*it == token)
			return &*it;  
	}
	return NULL;
}


bool SurveillanceMode::removeStreamingToken(const std::string& token)
{
	Mutex::ScopedLock lock(_mutex);
	for (auto it = _streamingTokens.begin(); it != _streamingTokens.end(); ++it) {
		if (*it == token) {
			_streamingTokens.erase(it);
			return true;
		}
	}
	return false;
}


void SurveillanceMode::onMotionStateChange(void*, anio::MotionDetectorState& state, const anio::MotionDetectorState&)
{
	DebugL << "Motion state changed: " << state.toString() << endl;
	{
		// Discard surveillance events while configuring
		Mutex::ScopedLock lock(_mutex); 
		DebugL << "Skipping motion state: " << _isConfiguring << ": " << _tempMotionDetector << endl;
		if (_isConfiguring)
			return;
	}

	switch (state.id()) {
		case anio::MotionDetectorState::Idle: 
			break;
		case anio::MotionDetectorState::Waiting:

			// Stop recording if active
			stopRecording(false);
		break;
		case anio::MotionDetectorState::Vigilant: 
			break;
		case anio::MotionDetectorState::Triggered:

			// Create a Motion Detected event via the Anionu API to
			// notify account users.
			anio::Event event("Motion Detected", 
				env().client().name() + ": Motion detected on channel " + _channel,
				anio::Event::High, anio::Event::Spot);
			env().client().createEvent(event);
			
			try {	
				// Start recording.
				startRecording(true);
			} 
			catch (std::exception& exc) {
				// Log and swallow recording errors. 
				// Nothing to do here since the "Recording Failed"
				// event will be triggered by the MediaManager.
				WarnL << "Start recording failed: " << exc.what() << endl;
			}
		break;
	}
}

	
void SurveillanceMode::onSetupStreamingSources(void*, api::StreamingSession& session, bool& handled)
{
	DebugL << "Initialize stream session: " << session.token() << endl;

	if (handled) return;
	
	// Check if the session matches any available tokens 
	TimedToken* token = getStreamingToken(session.token());
	if (token) {

		// Ensure token validity
		if (!token->expired()) {
			DebugL << "Creating configuration session: " << session.token() << endl;
			Mutex::ScopedLock lock(_mutex); 

			// Get the video capture or throw an exception.
			av::VideoCapture::Ptr video(env().channels().getChannel(_channel)->videoCapture(true));	
			session.captureStream().attachSource<av::VideoCapture>(video, true);

			// Set the input format to GRAY8 for the encoder
			video->getEncoderFormat(session.options().iformat);
			session.options().iformat.video.pixelFmt = "gray";

			// Attach motion detector to the stream.
			// The motion detector filters video frames before they are
			// sent to the encoder.
			assert(!_tempMotionDetector); // (may not be null since timeout is delayed)
			_tempMotionDetector = new anio::MotionDetector();
			//session.captureStream().attach(&_motionDetector, 3, false);		
			session.captureStream().attach(_tempMotionDetector, 3, true);			
			session.StateChange += sdelegate(this, &SurveillanceMode::onStreamingSessionStateChange);
			
			// Let the session manager know we have overridden 
			// session capture initialization.
			handled = true;
	
			// While the isConfiguring flag is set all motion detector states
			// will be ignored. The flag will be unset when the media session
			// is terminated.
			_isConfiguring = true;
		}

		// Otherwise the session is no longer valid, so we throw an exception 
		// to terminate the session in error.
		else {
			DebugL << "Configuration session timed out" << endl;
			removeStreamingToken(session.token());
			throw std::runtime_error("Surveillance Mode media preview has timed out.");
		}
	}
}


void SurveillanceMode::onSetupStreamingConnection(void*, api::StreamingSession& session, PacketStream& stream, bool& /* handled */)
{	
	DebugL << "Initialize streaming connection: " << session.token() << endl;		
	
	// Check if the session request matches any of our tokens. 
	// If so then we can attach our custom packetizer to this connection.
	if (getStreamingToken(session.token())) {

		// Override the connection packetizer if the peer requests HTTP
		// multipart/mixed-replace. HTTP is the preferable transport for 
		// configuration sessions since we can prepend surveillance metadata
		// using HTTP headers.
		Mutex::ScopedLock lock(_mutex); 
		assert(_tempMotionDetector);
		if (_tempMotionDetector &&
			session.options().protocol == "HTTP" && 
			session.options().framing == "multipart")
			stream.attach(new SurveillanceMultipartAdapter(*_tempMotionDetector), 15, true);
	}
}


void SurveillanceMode::onStreamingSessionStateChange(void* sender, api::StreamingState& state, const api::StreamingState& oldState)
{  
	auto session = reinterpret_cast<api::StreamingSession*>(sender); 			
	DebugL << "Configuration streaming session state changed: " << state << endl;

	// Remove the configuration media session token
	// reference when the session closes or times out.
	if (state.equals(api::StreamingState::Terminated)) {
		session->StateChange -= sdelegate(this, &SurveillanceMode::onStreamingSessionStateChange);
				
		removeStreamingToken(session->token());

		// Live configuration is over, life as normal.
		Mutex::ScopedLock lock(_mutex); 
		_tempMotionDetector = nullptr;
		_isConfiguring = false;
	}
	if (state.equals(api::StreamingState::Ready) && oldState.equals(api::StreamingState::Active)) {
		_isConfiguring = false;
	}
	if (state.equals(api::StreamingState::Active)) {
		_isConfiguring = true;
	}
}


bool SurveillanceMode::isRecording() const
{
	Mutex::ScopedLock lock(_mutex);
	return !_recordingToken.empty();
}


bool SurveillanceMode::isActive() const
{
	Mutex::ScopedLock lock(_mutex); 
	return _isActive;
}


const char* SurveillanceMode::modeName() const
{	
	return "Surveillance Mode";
}


const char* SurveillanceMode::channelName() const
{
	Mutex::ScopedLock lock(_mutex); 
	return _channel.c_str();
}


const char* SurveillanceMode::errorMessage() const 
{ 
	Mutex::ScopedLock lock(_mutex);
	return _error.empty() ? 0 : _error.c_str();
}


const char* SurveillanceMode::docFile() const
{	
	return "surveillancemodeplugin/README.md";
}


// ---------------------------------------------------------------------
//
void SurveillanceMode::buildForm(smpl::Form& form, smpl::FormElement& element)
{		
	smpl::FormField field;
	ScopedConfiguration config = getModeConfiguration(this);
	// Determine weather we are building the form at channel or
	// default scope as it will change the way we display the form.
	//
	//   Channel Scope: channels.[channelName].modes.[modeName]
	//   Defualt Scope: modes.[modeName]
	//
	bool defaultScope = element.id().find("modes.") == 0;
	DebugL << "Building form: " << defaultScope << endl;	 

	// Create a video media element which enables the user to preview motion changes
	// in real time as the channel is configured. This is achieved by adding a "media"
	// type field with is associated with a media session token. When an incoming media
	// session request subscribes to the token we use our motion detector as the streaming
	// source (see onSetupStreamingConnection). Media streaming sessions are only 
	// available when configuring at channel scope ie. defaultScope == false.
	// No surveillance events will be triggered while media sessions are active.
	if (!form.partial() && !defaultScope) {		
		element.setHint(
			"This form lets you to configure settings for Surveillance Mode. "
			"Use the motion video below to visualize motion as Spot sees it, and fine-tune motion detection settings in real-time."
			"Be sure to read the 'Mode Information' page for tips on optimizing settings for the best results."
		);

		TimedToken* token = this->createStreamingToken();
		field = element.addField("media", config.getScopedKey("Preview", defaultScope), "Live Motion Preview");
		field.setHint(
			"Keep an eye on the motion levels and motion state in the top left hand corner of the video. "
			"If motion level exceeds the motion threshold then the motion detector will enter the 'Triggered' state. "			
			"Note that the video may be delayed, depending on your connection speed. "
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
	field.setValue(config.getInt("MotionThreshold", kMotionThreshold, defaultScope));
	if (!defaultScope)
		field.setLive(true);

	field = element.addField("number", config.getScopedKey("PreSurveillanceDelay", defaultScope), "Pre Surveillance Delay");	
	field.setHint(
		"This is the delay time (in seconds) before motion detection will actually commence. "
		"Set this to the amount of time you need to vacate the surveilled area after activating Surveillance Mode."
	);
	field.setValue(config.getInt("PreSurveillanceDelay", kPreSurveillanceDelay, defaultScope));
	if (!defaultScope)
		field.setLive(true);

	field = element.addField("number", config.getScopedKey("MinTriggeredDuration", defaultScope), "Minimum Triggered Duration");
	field.setHint(
		"This is minimum duration of time that the motion detector can remain in the 'Triggered' state. "
		"This is also the minimum duration of any recorded videos."
	);
	field.setValue(config.getInt("MinTriggeredDuration", kMinTriggeredDuration, defaultScope));
	if (!defaultScope)
		field.setLive(true);

	field = element.addField("number", config.getScopedKey("MaxTriggeredDuration", defaultScope), "Maximum Triggered Duration");	
	field.setHint(
		"This is the maximum amount of time that the motion detector can remain in the 'Triggered' state. "
		"This is also the maximum duration of any recorded videos. "
	);
	field.setValue(config.getInt("MaxTriggeredDuration", kMaxTriggeredDuration, defaultScope));
	if (!defaultScope)
		field.setLive(true);

	field = element.addField("number", config.getScopedKey("StableMotionFrames", defaultScope), "Stable Motion Frames");
	field.setHint(
		"In order to avoid false alarms, the motion detector must detect a certain number of motion frames inside a time interval before the alarm will trigger. "
		"This setting determines how many motion frames to detect before motion is considered stable. "
		"Recommended setting: 3 (3 frames)"
	);
	field.setValue(config.getInt("StableMotionFrames", kStableMotionFrames, defaultScope));
	if (!defaultScope)
		field.setLive(true);

	field = element.addField("number", config.getScopedKey("StableMotionLifetime", defaultScope), "Stable Motion Lifetime");	
	field.setHint(
		"This is the duration of time (in seconds) that motion frames remain valid. "
		"See 'Stable Motion Frames' for further explainiation about this setting. "
		"Recommended setting: 1 (1 second)"
	);
	field.setValue(config.getInt("StableMotionLifetime", kStableMotionLifetime, defaultScope));
	if (!defaultScope)
		field.setLive(true);

	// Disabling this setting as it is not completely necessary, and may overcomplicate things.
	//field = element.addField("number", config.getScopedKey("PostTriggeredDelay", defaultScope), "Post Triggered Delay");	
	//field.setHint(
	//	"This is the duration of time (in seconds) to wait before motion detection will recommence after the 'Triggered' state has ended. "
	//	"During this delay, the motion detector will sit in the 'Waiting' state. "
	//	"Once this delay is complete, the motion detector will enter the 'Vigilant' state again, and begin detecting motion."
	//);
	//field.setValue(config.getInt("PostTriggeredDelay", defaultScope));
	//if (!defaultScope)
	//	field.setLive(true);

	// Enable Video Synchronization
	field = element.addField("boolean", config.getScopedKey("SynchronizeVideos", defaultScope), "Synchronize Videos");	
	field.setHint(
		"Do you want to synchronize/upload recorded videos to your online account? "
		"You can access synchronized videos from your dashboard." //are available 
	);
	field.setValue(config.getBool("SynchronizeVideos", kSynchronizeVideos, defaultScope));
}


void SurveillanceMode::parseForm(smpl::Form&, smpl::FormElement& element)
{
	DebugL << "Parsing form" << endl;	 
	smpl::FormField field;
	
	field = element.getField("Surveillance Mode.MotionThreshold", true);
	if (field.valid()) {
		int value = field.intValue();
		if (value < 50 ||
			value > 100000) {
			field.setError("Must be a number between 50 and 100000.");
		}
		else
			env().config().setInt(field.id(), value);
	}
	
	field = element.getField("Surveillance Mode.PreSurveillanceDelay", true);
	if (field.valid())
		env().config().setInt(field.id(), field.intValue());

	//field = element.getField("Surveillance Mode.PostTriggeredDelay", true);
	//if (field.valid())
	//	env().config().setInt(field.id(), field.intValue());

	field = element.getField("Surveillance Mode.MinTriggeredDuration", true);
	if (field.valid())
		env().config().setInt(field.id(), field.intValue());

	field = element.getField("Surveillance Mode.MaxTriggeredDuration", true);
	if (field.valid())
		env().config().setInt(field.id(), field.intValue());

	field = element.getField("Surveillance Mode.StableMotionFrames", true);
	if (field.valid())
		env().config().setInt(field.id(), field.intValue());

	field = element.getField("Surveillance Mode.StableMotionLifetime", true);
	if (field.valid())
		env().config().setInt(field.id(), field.intValue());

	field = element.getField("Surveillance Mode.SynchronizeVideos", true);
	if (field.valid())
		env().config().setBool(field.id(), field.boolValue());

	loadConfig();
}


} } } // namespace scy::anio::spot