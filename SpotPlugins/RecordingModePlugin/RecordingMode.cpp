#include "RecordingMode.h"
#include "Sourcey/Spot/IEnvironment.h"
#include "Sourcey/Spot/IConfiguration.h"
#include "Sourcey/Spot/IChannel.h"
#include "Sourcey/Spot/ISession.h"
#include "Sourcey/Spot/ISynchronizer.h"
//#include "Sourcey/Spot/Util.h"
//#include "Sourcey/Media/CaptureFactory.h"
#include "Sourcey/Logger.h"

#include "Poco/File.h"


using namespace std;
using namespace Poco;
using namespace Sourcey::Media;


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

	log() << "Loading Config: " << _channel.name() << endl;

	_segmentDuration = _config.getInt("SegmentDuration", 3 * 60);
	_synchronizeVideos = _config.getBool("SynchronizeVideos", false);
}


void RecordingMode::enable() 
{
	log() << "Starting" << endl;
	
	try
	{
		loadConfig();
		startRecording();
		setState(this, ModeState::Enabled);
	}
	catch (Exception& exc)
	{
		cerr << "Error:" << exc.displayText() << endl;
		setState(this, ModeState::Failed);
		throw exc;
	}
}


void RecordingMode::disable() 
{
	log() << "Stopping" << endl;
	
	setState(this, ModeState::Disabled);
	stopRecording();
}

	
void RecordingMode::startRecording()
{	
	FastMutex::ScopedLock lock(_mutex); 

	RecorderParams params;
	env().media().initRecorderParams(_channel, params);
	params.stopAt = ::time(0) + _segmentDuration;
	_recordingInfo = env().media().startRecording(_channel, params);
	_recordingInfo.encoder->StateChange += delegate(this, &RecordingMode::onEncoderStateChange);

	log() << "Started Recording: " << _recordingInfo.token << endl;
}


void RecordingMode::stopRecording()
{
	FastMutex::ScopedLock lock(_mutex); 

	if (!_recordingInfo.token.empty()) {
		_recordingInfo.encoder->StateChange -= delegate(this, &RecordingMode::onEncoderStateChange);
		env().media().stopRecording(_recordingInfo.token);
		log() << "Stopped Recording: " << _recordingInfo.token << endl;
		_recordingInfo.token = "";
		_recordingInfo.encoder = NULL;
	}
}


void RecordingMode::onEncoderStateChange(void* sender, EncoderState& state, const EncoderState& oldState)
{
	log() << "Recorder State Changed: " << state.toString() << endl;
	IEncoder* encoder = reinterpret_cast<IEncoder*>(sender);

	switch (state.id()) {

		case EncoderState::None:	
		break;

		case EncoderState::Ready:
		break;

		case EncoderState::Encoding:
		break;

		case EncoderState::Failed:
		break;

		case EncoderState::Stopped:

			// Start a new recording segment if the mode is 
			// still enabled.
			if (!isDisabled() &&
				encoder == _recordingInfo.encoder) {
				encoder->StateChange -= delegate(this, &RecordingMode::onEncoderStateChange);	
				{
					FastMutex::ScopedLock lock(_mutex); 

					// Synchronize video's if required
					if (_synchronizeVideos) {

						RecorderParams& params = static_cast<RecorderParams&>(encoder->params());

						Spot::Job job; 
						job.type = "Video";
						job.path = params.ofile;

						_env.synchronizer() >> job;
					}
				}
				
				startRecording();
			}
		break;
	}
}


bool RecordingMode::isConfigurable() const
{	
	return true;
}


bool RecordingMode::hasParsableConfig(Symple::Form& form) const
{
	return form.hasField(".Recording Mode.", true);
}


void RecordingMode::createConfigForm(Symple::Form& form, Symple::FormElement& element, bool useBase)
{
	log() << "Creating Config Form" << endl;

	/*
	FastMutex::ScopedLock lock(_mutex); 

	Symple::FormField field;	
	
	// Video Segment Duration
	field = element.addField("text", _config.getScoped("SegmentDuration", useBase), "Video Segment Duration");	
	field.setHint(
		"This setting determines the length in seconds of each recorded video. "
	);
	field.setValue(_segmentDuration);
	//layout.addFieldRef(field);	

	// Enable Video Synchronization
	field = element.addField("boolean", _config.getScoped("SynchronizeVideos", useBase), "Enable Video Synchronization?");	
	field.setHint(
		"Would you like to upload / synchronize recorded videos with your Anionu account? "
		"This is not recommended for low-bandwidth accounts because you will use up your bandwidth in no time."
	);
	field.setValue(_synchronizeVideos);	
	//layout.addFieldRef(field);
	*/
}


void RecordingMode::parseConfigForm(Symple::Form& form, Symple::FormElement& element)
{
	log() << "Parsing Config Form" << endl;

	FastMutex::ScopedLock lock(_mutex); 
	
	Symple::FormField field;

	field = element.getField("Recording Mode.SegmentDuration", true);
	if (field.valid()) {
		int value = field.intValue();
		if (value < 10 ||
			value > 60 * 60) {
			field.setError("The segment duration must be between 10 seconds and one hour.");
		}
		else
			_env.config().setInt(field.id(), value);
	}

	field = element.getField("Recording Mode.SynchronizeVideos", true);
	if (field.valid())
		_env.config().setBool(field.id(), field.boolValue());

	loadConfig();
}


void RecordingMode::createHelp(std::ostream& s) 
{
	s << "<h2>What is Recording Mode?</h2>";
	s << "<p>Recording Mode enables you to record a constant stream of videos from a surveillance channel. ";
	s << "Recorded videos can be stored either on your local hard drive, ";
	s << "or synchronized with your Anionu account where they are watchable from your online dashboard.</p>"; 
	s << "<p>We do recommend keeping video synchronization disabled however, because uploading a continuous "; 
	s << "stream of videos to Anionu servers will use up your bandwidth quickly.</p>";

	s << "<h2>What video formats are available?</h2>";
	s << "<p>Due to the licensing restrictions we can only provide you with a couple of basic video formats by default.</p>";
	s << "<p>We do however provide a free demonstrational Media Plugin which adds support for other popular proprietary formats like H264 and XviD.</p>";
}


} } // namespace Sourcey::Spot