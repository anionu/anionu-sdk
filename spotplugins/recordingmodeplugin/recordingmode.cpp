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

#include "recordingmode.h"
#include "anionu/spot/api/environment.h"
#include "anionu/spot/api/channel.h"
#include "anionu/spot/api/synchronizer.h"
#include "anionu/spot/api/util.h"

#include "scy/symple/client.h"
#include "scy/configuration.h"
#include "scy/logger.h"


using std::endl;


namespace scy {
namespace anio { 
namespace spot {


RecordingMode::RecordingMode(api::Environment& env, const std::string& channel) : 
	api::IModule(env), _channel(channel), _isActive(false)
{
	DebugL << "Creating" << endl;
	loadConfig();
}


RecordingMode::~RecordingMode()
{	
	DebugL << "Destroying" << endl;
}


bool RecordingMode::activate() 
{
	DebugL << "Activating" << endl;
	try {
		env().media().RecordingStopped += sdelegate(this, &RecordingMode::onRecordingStopped);
		startRecording();
		_isActive = true;
	}
	catch (std::exception& exc) {
		_error = std::string(exc.what());
		ErrorL << "Activation error: " << _error << endl;
		return false;
	}
	return true;
}


void RecordingMode::deactivate() 
{
	DebugL << "Deactivating" << endl;
	try {
		_isActive = false;
		env().media().RecordingStopped.detach(this);
		if (isRecording())
			stopRecording();
	}
	catch (std::exception& exc) {
		ErrorL << "Deactivation error: " + std::string(exc.what()) << endl;
	}
}


void RecordingMode::loadConfig()
{
	Mutex::ScopedLock lock(_mutex); 	
	DebugL << "Loading Config: " << _channel << endl;
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
	DebugL << "Start Recording" << endl;
	if (isRecording())
		throw std::runtime_error("Start recording failed: Recorder already active.");
	
	Mutex::ScopedLock lock(_mutex); 

	// Get the recording encoder options for the current session
	// configuration and override some defaults before we start recording.
	api::EncoderOptions options = env().media().getEncoderOptions(_channel);
	options.synchronizeVideo  = _synchronizeVideos;
	options.duration = _segmentDuration * 1000;

	// Tell the media manager to supress events for recording mode,
	// otherwise we will end up with a rediculous number or events.
	options.supressEvents = true;

	// Start recording, or throw an exception.
	env().media().startRecording(options);
	DebugL << "Recording started: " << options.token << endl;
	_recordingToken = options.token;
}


void RecordingMode::stopRecording()
{	
	DebugL << "Stop Recording" << endl;
	if (!isRecording())		
		throw std::runtime_error("Stop recording failed: Recorder not active.");
	
	Mutex::ScopedLock lock(_mutex); 
	env().media().stopRecording(_recordingToken, true);
	DebugL << "Recording Stopped: " << _recordingToken << endl;
	_recordingToken = "";
}


void RecordingMode::onRecordingStopped(void*, api::RecordingSession& recorder)
{
	if (isActive() &&
		recordingToken().empty() || 
		recordingToken() != recorder.options.token) 
		return;
	
	try {	
		// Start a new recording segment if the
		// mode is still active.
		_recordingToken = "";
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


bool RecordingMode::isActive() const
{
	Mutex::ScopedLock lock(_mutex); 
	return _isActive;
}


const char* RecordingMode::modeName() const
{
	return "Recording Mode";
}


const char* RecordingMode::channelName() const
{
	Mutex::ScopedLock lock(_mutex); 
	return _channel.c_str();
}


const char* RecordingMode::errorMessage() const 
{ 
	Mutex::ScopedLock lock(_mutex);
	return _error.empty() ? 0 : _error.c_str();
}


const char* RecordingMode::docFile() const
{
	return "recordingmodeplugin/README.md";
}


// ---------------------------------------------------------------------
//
void RecordingMode::buildForm(smpl::Form&, smpl::FormElement& element)
{
	DebugL << "Building form" << endl;
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
	DebugL << "Parsing form" << endl;
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
	
	//Signal2<const EncoderOptions&, RecordingStream*&> RecordingStarted;
	//Signal2<const EncoderOptions&, RecordingStream*&> RecordingStopped;
	//env().media().RecordingStarted += sdelegate(this, &RecordingMode::onRecordingStarted);
	//env().media().RecordingStarted.detach(this);
		//_recordingToken.encoder->StateChange -= sdelegate(this, &RecordingMode::onEncoderStateChange);
	//_recordingToken.encoder->StateChange += sdelegate(this, &RecordingMode::onEncoderStateChange);
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
			encoder->StateChange -= sdelegate(this, &RecordingMode::onEncoderStateChange);	
			// Synchronize video's if required
			if (_synchronizeVideos) {
				EncoderOptions& options = static_cast<EncoderOptions&>(encoder->options());
				spot::Job job; 
				job.type = "Video";
				job.path = options.ofile;
				_env().synchronizer() >> job;
			}
			*/