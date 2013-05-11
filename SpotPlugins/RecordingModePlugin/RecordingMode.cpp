#include "RecordingMode.h"
#include "Anionu/Spot/API/IEnvironment.h"
#include "Anionu/Spot/API/IConfiguration.h"
#include "Anionu/Spot/API/IChannel.h"
#include "Anionu/Spot/API/ISession.h"
#include "Anionu/Spot/API/ISynchronizer.h"
#include "Sourcey/Logger.h"

#include "Poco/File.h"


using namespace std;
using namespace Scy::Media;

using Poco::FastMutex;


namespace Scy {
namespace Anionu { 
namespace Spot {
	using namespace API;


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
	env().media().RecordingStopped += delegate(this, &RecordingMode::onRecordingStopped);
}


void RecordingMode::uninitialize() 
{
	env().media().RecordingStopped.detach(this);
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
		exc.rethrow();
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

	API::RecordingOptions options = env().media().getRecordingOptions(_channel.name());
	options.synchronize = _synchronizeVideos;
	options.duration = _segmentDuration * 1000;

	// Tell the media manager to supress events for recording mode,
	// otherwise we will end up with a rediculous number or events.
	options.supressEvents = true;
	_env.media().startRecording(options);
	log() << "Started Recording: " << options.token << endl;
	_recordingToken = options.token;
	return true;
}


bool RecordingMode::stopRecording()
{
	FastMutex::ScopedLock lock(_mutex); 

	if (!_recordingToken.empty()) {
		_env.media().stopRecording(_recordingToken);
		log() << "Stopped Recording: " << _recordingToken << endl;
		_recordingToken = "";
		return true;
	}
	return false;
}


void RecordingMode::onRecordingStopped(void* sender, API::RecorderStream& stream)
{
	if (!isActive() || 
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


string RecordingMode::recordingToken()
{	
	FastMutex::ScopedLock lock(_mutex); 
	return _recordingToken;
}



// ---------------------------------------------------------------------
//
void RecordingModeFormProcessor::buildForm(Symple::Form&, Symple::FormElement& element) //, bool defaultScope
{
	Symple::FormField field;
	ScopedConfiguration config = mode.config();
	bool defaultScope = element.id().find("channels.") == 0;
	mode.log() << "Building Form: " << element.id() << endl;	

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


void RecordingModeFormProcessor::parseForm(Symple::Form&, Symple::FormElement& element)
{
	mode.log() << "Parsing Config Form" << endl;
	if (!element.hasField(".Recording Mode.", true))
		return;

	IEnvironment& env = mode.env();	
	Symple::FormField field;

	// TODO: Non synchronized videos can have a longer duration.
	// See RecordingMode.md

	field = element.getField("Recording Mode.SegmentDuration", true);
	if (field.valid()) {
		int value = field.intValue();
		if (value < 10 ||
			value > 15 * 60) {
			field.setError("The segment duration must be between 10 seconds and 15 minutes.");
		}
		else
			env.config().setInt(field.id(), value);
	}

	field = element.getField("Recording Mode.SynchronizeVideos", true);
	if (field.valid())
		env.config().setBool(field.id(), field.boolValue());

	mode.loadConfig();
}


string RecordingModeFormProcessor::documentFile() 
{
	return "plugins/RecordingModePlugin/RecordingMode.md";
}


bool RecordingModeFormProcessor::isConfigurable() const
{	
	return true;
}


bool RecordingModeFormProcessor::hasParsableFields(Symple::Form& form) const
{
	return form.hasField(".Recording Mode.", true);
}


} } } // namespace Scy::Anionu::Spot





/*
	
	//Signal2<const API::RecordingOptions&, API::RecordingStream*&> RecordingStarted;
	//Signal2<const API::RecordingOptions&, API::RecordingStream*&> RecordingStopped;
	//env().media().RecordingStarted += delegate(this, &RecordingMode::onRecordingStarted);
	//env().media().RecordingStarted.detach(this);
		//_recordingToken.encoder->StateChange -= delegate(this, &RecordingMode::onEncoderStateChange);
	//_recordingToken.encoder->StateChange += delegate(this, &RecordingMode::onEncoderStateChange);
void RecordingMode::onRecordingStarted(void* sender, API::RecorderStream& stream)
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
	log() << "Recorder State Changed: " << state.toString() << endl;
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
				Spot::Job job; 
				job.type = "Video";
				job.path = options.ofile;
				_env.synchronizer() >> job;
			}
			*/