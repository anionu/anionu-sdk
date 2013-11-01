#include "RecordingMode.h"
#include "Anionu/Spot/API/Environment.h"
#include "Anionu/Spot/API/Channel.h"
#include "Anionu/Spot/API/Synchronizer.h"
#include "Anionu/Spot/API/Util.h"

#include "Sourcey/Symple/Client.h"
#include "Sourcey/Configuration.h"
#include "Sourcey/Logger.h"


namespace scy {
	using namespace av;
namespace anio { 
namespace spot {


RecordingMode::RecordingMode(api::Environment& env, const std::string& channel) : 
	api::IModule(env), _channel(channel), _isActive(false)
{
	log("Creating");
	loadConfig();
}


RecordingMode::~RecordingMode()
{	
	log("Destroying");
}


bool RecordingMode::activate() 
{
	log("Activating");	
	try {
		env().media().RecordingStopped += delegate(this, &RecordingMode::onRecordingStopped);
		startRecording();
		_isActive = true;
	}
	catch (std::exception& exc) {
		_error = std::string(exc.what());
		log("Activation error: " + _error, "error");
		return false;
	}
	return true;
}


void RecordingMode::deactivate() 
{
	log("Deactivating");	
	try {
		_isActive = false;
		env().media().RecordingStopped.detach(this);
		if (isRecording())
			stopRecording();
	}
	catch (std::exception& exc) {
		log("Deactivation error: " + std::string(exc.what()), "error");
	}
}


void RecordingMode::loadConfig()
{
	Mutex::ScopedLock lock(_mutex); 	
	log("Loading Config: " + _channel);	
	ScopedConfiguration config = getModeConfiguration(this);
	_segmentDuration = config.getInt("SegmentDuration", 5 * 60); // 5 minutes
	_synchronizeVideos = config.getBool("SynchronizeVideos", false); // no sync

	//api::slog(this) << "Loading Config: " << _channel 
	//	<< "\r\tSegment duration: " << _segmentDuration 
	//	<< "\r\tSynchronize videos: " << _synchronizeVideos 
	//	<< endl;
}

	
void RecordingMode::startRecording()
{	
	log("Start Recording");
	if (isRecording())
		throw std::runtime_error("Start recording failed: Recorder already active.");
	
	Mutex::ScopedLock lock(_mutex); 

	// Get the recording encoder options for the current session
	// configuration and override some defaults before we start recording.
	api::RecordingOptions options = env().media().getRecordingOptions(_channel);
	options.synchronizeVideo  = _synchronizeVideos;
	options.duration = _segmentDuration * 1000;

	// Tell the media manager to supress events for recording mode,
	// otherwise we will end up with a rediculous number or events.
	options.supressEvents = true;

	// Start recording, or throw an exception.
	env().media().startRecording(options);
	log("Recording started: " + options.token);
	_recordingToken = options.token;
}


void RecordingMode::stopRecording()
{	
	log("Stop Recording");
	if (!isRecording())		
		throw std::runtime_error("Stop recording failed: Recorder not active.");
	
	Mutex::ScopedLock lock(_mutex); 
	env().media().stopRecording(_recordingToken, true);
	log("Recording Stopped: " + _recordingToken);
	_recordingToken = "";
}


void RecordingMode::onRecordingStopped(void*, api::RecorderStream& stream)
{
	if (isActive() &&
		recordingToken().empty() || 
		recordingToken() != stream.options.token) 
		return;
	
	try {	
		// Start a new recording segment if the
		// mode is still active.
		startRecording();
	} 
	catch (...) {
		// Catch and swallow recording errors. 
		// Nothing to do here since the "Recording Failed"
		// event will be triggered by the MediaManager.
	}
}


bool RecordingMode::isRecording() const
{
	Mutex::ScopedLock lock(_mutex);
	return !_recordingToken.empty();
}


std::string RecordingMode::recordingToken()
{	
	Mutex::ScopedLock lock(_mutex); 
	return _recordingToken;
}


const char* RecordingMode::errorMessage() const 
{ 
	Mutex::ScopedLock lock(_mutex);
	return _error.empty() ? 0 : _error.c_str();
}


const char* RecordingMode::channelName() const
{
	Mutex::ScopedLock lock(_mutex); 
	return _channel.c_str();
}


bool RecordingMode::isActive() const
{
	Mutex::ScopedLock lock(_mutex); 
	return _isActive;
}


const char* RecordingMode::docFile() const
{
	return "RecordingModePlugin/RecordingMode.md";
}


// ---------------------------------------------------------------------
//
void RecordingMode::buildForm(smpl::Form&, smpl::FormElement& element)
{
	log("Building form");	
	smpl::FormField field;
	ScopedConfiguration config = getModeConfiguration(this);

	// Determine weather we are building the form at channel or
	// default scope as it will change the way we display the form.
	//
	//   Channel Scope: channels.[channelName].modes.[modeName]
	//   Defualt Scope: modes.[modeName]
	//
	bool defaultScope = element.type() != "form";
	if (defaultScope) {
		element.setHint(
			"This form enables you to configure the default settings for Recording Mode. "
			"Any settings configured here may be overridden on a per channel basis (see Channel Configuration)."
		);
	}
	
	// Video Segment Duration
	field = element.addField("number", config.getScopedKey("SegmentDuration", defaultScope), "Video Segment Duration");	
	field.setHint(
		"This setting determines the length in seconds of each recorded video. "
	);
	field.setValue(config.getInt("SegmentDuration", _segmentDuration, defaultScope));
	
	// Enable Video Synchronization
	field = element.addField("boolean", config.getScopedKey("SynchronizeVideos", defaultScope), "Enable Video Synchronization");	
	field.setHint(
		"Would you like to synchronize/upload recorded videos to your online storage account? "
		"Synchronizing a constant stream of videos requires a lot of online storage capacity, so appropriate care should be taken when managing this setting."
		"You can check on the status of your remaining bandwidth and storage from the Account page of your dashboard."
	);
	field.setValue(config.getBool("SynchronizeVideos", _synchronizeVideos, defaultScope));	
}


void RecordingMode::parseForm(smpl::Form&, smpl::FormElement& element)
{
	log("Parsing form");
	smpl::FormField field;	

	field = element.getField("Recording Mode.SegmentDuration", true);
	if (field.valid()) {
		int value = field.intValue();
		// TODO: Non synchronized videos can have a longer duration.
		// See RecordingMode.md
		if (value < 10 ||
			value > 15 * 60) {
			field.setError("The segment duration must be between 10 seconds and 15 minutes.");
		}
		else {
			env().config().setInt(field.id(), value);
		}
	}

	field = element.getField("Recording Mode.SynchronizeVideos", true);
	if (field.valid())
		env().config().setBool(field.id(), field.boolValue());

	loadConfig();
}


} } } // namespace scy::anio::spot





/*


bool RecordingMode::isConfigurable() const
{	
	return true;
}


bool RecordingMode::hasParsableFields(smpl::Form& form) const
{
	return form.hasField(".Recording Mode.", true);
}
	
	//Signal2<const RecordingOptions&, RecordingStream*&> RecordingStarted;
	//Signal2<const RecordingOptions&, RecordingStream*&> RecordingStopped;
	//env().media().RecordingStarted += delegate(this, &RecordingMode::onRecordingStarted);
	//env().media().RecordingStarted.detach(this);
		//_recordingToken.encoder->StateChange -= delegate(this, &RecordingMode::onEncoderStateChange);
	//_recordingToken.encoder->StateChange += delegate(this, &RecordingMode::onEncoderStateChange);
void RecordingMode::onRecordingStarted(void* sender, RecorderStream& stream)
{
}
*/
	/*
	s << "<h1>About Recording Mode</h1>"
		 "<p>Recording Mode provides Spot with the ability to record a constant stream of videos from any surveillance channel. "
		 "If you require 24/7 surveillance with video backup then Recording Mode is what you need.</p>";

	s << "<h2>Configuring Recording Mode</h2>"
		"<p>Recorded videos can either stored on your local hard drive, "
		 "or synchronized with your Anionu account where you can access them online via your dashboard.</p>"
		"<p>Bear in mind that synchronizing a constant stream of videos requires a lot of online storage capacity, "
		 "so appropriate care should be taken when considering weather or not to synchronize videos.</p>";
	
	//	"Synchronizing a constant stream of videos requires a lot of online storage capacity, so appropriate care should be taken when managing this setting."
	//	"You can check on the status of your remaining bandwidth and storage from the Account page of your dashboard."
	//s << "<h2>Recording Mode Options</h2>";

	//s << "<h2>What video formats are available?</h2>";
	//s << "<p>Due to the licensing restrictions we can only provide you with a couple of basic video formats by default.</p>";
	//s << "<p>We do however provide a free demonstrational Media Plugin which adds support for other popular proprietary formats like H264 and XviD.</p>";
	*/
			/*
void RecordingMode::onEncoderStateChange(void* sender, EncoderState& state, const EncoderState&)
{
	log(this) << "Recorder State Changed: " << state.toString() << endl;
	IEncoder* encoder = reinterpret_cast<IEncoder*>(sender);

	if (state.equals(EncoderState::Stopped)) {
		// Start a new recording segment if the
		// mode is still active.
		if (isActive() &&
			encoder == recordingOptions().encoder) {			
			startRecording();
		}
	}
}
			*/	

/*
			encoder->StateChange -= delegate(this, &RecordingMode::onEncoderStateChange);	
			// Synchronize video's if required
			if (_synchronizeVideos) {
				RecordingOptions& options = static_cast<RecordingOptions&>(encoder->options());
				spot::Job job; 
				job.type = "Video";
				job.path = options.ofile;
				_env().synchronizer() >> job;
			}
			*/