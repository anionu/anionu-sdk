#include "RecordingMode.h"
#include "Sourcey/Spot/IEnvironment.h"
#include "Sourcey/Spot/IConfiguration.h"
#include "Sourcey/Spot/IChannel.h"
#include "Sourcey/Spot/ISession.h"
#include "Sourcey/Spot/ISynchronizer.h"
#include "Sourcey/Logger.h"

#include "Poco/File.h"


using namespace std;
using namespace Sourcey::Media;

using Poco::FastMutex;


namespace Sourcey {
namespace Spot {


RecordingMode::RecordingMode(IEnvironment& env, IChannel& channel) :
	IMode(env, channel, "Recording Mode")
{
	log() << "Creating" << endl;
}


RecordingMode::~RecordingMode()
{	
	log() << "Destroying" << endl;
}


void RecordingMode::initialize() 
{
	loadConfig();
}


void RecordingMode::uninitialize() 
{
}


void RecordingMode::loadConfig()
{
	FastMutex::ScopedLock lock(_mutex); 

	_segmentDuration = _config.getInt("SegmentDuration", 5 * 60);
	_synchronizeVideos = _config.getBool("SynchronizeVideos", false);

	log() << "Loading Config: " << _channel.name() 
		<< "\r\tSegmentDuration: " << _segmentDuration 
		<< "\r\tSynchronizeVideos: " << _synchronizeVideos 
		<< endl;
}


void RecordingMode::activate() 
{
	log() << "Starting" << endl;
	
	try
	{
		loadConfig();
		IMode::activate();
		startRecording();
	}
	catch (Exception& exc)
	{
		log("error")  << "Error:" << exc.displayText() << endl;
		setState(this, ModeState::Error);
		throw exc;
	}
}


void RecordingMode::deactivate() 
{
	log() << "Stopping" << endl;
	
	stopRecording();
	IMode::deactivate();
}

	
bool RecordingMode::startRecording()
{	
	log() << "Start Recording" << endl;
	FastMutex::ScopedLock lock(_mutex); 

	RecorderOptions options;
	_env.media().initRecorderOptions(_channel, options);
	options.duration = _segmentDuration * 1000;
		
	_recordingAction.token = "";
	_recordingAction.encoder = NULL;
	_recordingAction.synchronize = _synchronizeVideos;

	// Tell the media manager to supress recording events for recording mode,
	// otherwise we will end up with a huge number or recording events.
	_recordingAction.supressEvents = true;
	_env.media().startRecording(_channel, options, _recordingAction);

	_recordingAction.encoder->StateChange += delegate(this, &RecordingMode::onEncoderStateChange);

	log() << "Started Recording: " << _recordingAction.token << endl;
	return true;
}


bool RecordingMode::stopRecording()
{
	FastMutex::ScopedLock lock(_mutex); 

	if (!_recordingAction.token.empty()) {
		_recordingAction.encoder->StateChange -= delegate(this, &RecordingMode::onEncoderStateChange);
		_env.media().stopRecording(_recordingAction.token);
		log() << "Stopped Recording: " << _recordingAction.token << endl;
		_recordingAction.token = "";
		_recordingAction.encoder = NULL;
		return true;
	}
	return false;
}


void RecordingMode::onEncoderStateChange(void* sender, EncoderState& state, const EncoderState&)
{
	log() << "Recorder State Changed: " << state.toString() << endl;
	IEncoder* encoder = reinterpret_cast<IEncoder*>(sender);

	if (state.equals(EncoderState::Stopped)) {
		// Start a new recording segment if the mode is 
		// still active.
		if (isActive() &&
			encoder == recordingAction().encoder) {
			encoder->StateChange -= delegate(this, &RecordingMode::onEncoderStateChange);	
			/*
			// Synchronize video's if required
			if (_synchronizeVideos) {
				RecorderOptions& options = static_cast<RecorderOptions&>(encoder->options());
				Spot::Job job; 
				job.type = "Video";
				job.path = options.ofile;
				_env.synchronizer() >> job;
			}
			*/				
			startRecording();
		}
	}
}


void RecordingMode::buildConfigForm(Symple::Form&, Symple::FormElement& element, bool defaultScope)
{
	log() << "Creating Config Form" << endl;
	ModeConfiguration config = this->config();
	Symple::FormField field;	

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
	field.setValue(config.getInt("SegmentDuration", defaultScope));
	
	// Enable Video Synchronization
	field = element.addField("boolean", config.getScopedKey("SynchronizeVideos", defaultScope), "Enable Video Synchronization");	
	field.setHint(
		"Would you like to synchronize/upload recorded videos to your online storage account? "
		"Synchronizing a constant stream of videos requires a lot of online storage capacity, so appropriate care should be taken when managing this setting."
		"You can check on the status of your remaining bandwidth and storage from the Account page of your dashboard."
	);
	field.setValue(config.getBool("SynchronizeVideos", defaultScope));	
}


void RecordingMode::parseConfigForm(Symple::Form&, Symple::FormElement& element)
{
	log() << "Parsing Config Form" << endl;

	IEnvironment& env = this->env();	
	Symple::FormField field;

	field = element.getField("Recording Mode.SegmentDuration", true);
	log() << "Parsing Config Form: SynchronizeVideos: " << field.valid() << endl;
	if (field.valid()) {
		int value = field.intValue();
		if (value < 10 ||
			value > 60 * 60) {
			field.setError("The segment duration must be between 10 seconds and one hour.");
		}
		else
			env.config().setInt(field.id(), value);
	}

	field = element.getField("Recording Mode.SynchronizeVideos", true);
	log() << "Parsing Config Form: SynchronizeVideos: " << field.valid() << ": " << field.boolValue() << endl;
	if (field.valid())
		env.config().setBool(field.id(), field.boolValue());

	loadConfig();
}


void RecordingMode::printInformation(std::ostream& s) 
{
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
}


bool RecordingMode::isConfigurable() const
{	
	return true;
}


RecordingAction& RecordingMode::recordingAction()
{	
	FastMutex::ScopedLock lock(_mutex); 
	return _recordingAction;
}


bool RecordingMode::hasParsableConfig(Symple::Form& form) const
{
	return form.hasField(".Recording Mode.", true);
}


} } // namespace Sourcey::Spot