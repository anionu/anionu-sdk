#ifndef RECORDING_MODE_H
#define RECORDING_MODE_H


//#include "Sourcey/Spot/Types.h"
#include "Sourcey/Spot/IMode.h"
#include "Sourcey/Spot/IMediaManager.h"
//#include "Sourcey/Media/CaptureRecorder.h"


namespace Sourcey {
namespace Spot {


class RecordingModeImpl: public IMode
{
public:
	RecordingModeImpl(IEnvironment& env, IChannel& channel);
	~RecordingModeImpl();

	void initialize();
	void uninitialize();

	void enable();
	void disable();
	
	void startRecording();
	void stopRecording();
	
	void onEncoderStateChange(void* sender, Media::EncoderState& state, const Media::EncoderState& oldState);
	
	void loadConfig();

	bool isConfigurable() const;
	bool hasParsableConfig(Symple::Form& form) const;
	void createHelp(std::ostream& s);
	void createConfigForm(Symple::Form& form, Symple::FormElement& element, bool useBase = false);
	void parseConfigForm(Symple::Form& form, Symple::FormElement& element);

	//void createHelp(std::ostream& s);
	//void createConfigForm(Symple::Form& form, Symple::FormElement& element, bool useBase = false);
	//void parseConfigForm(Symple::Form& form, Symple::FormElement& element);

	virtual const char* className() const { return "Recording Mode"; }

private: 
	//Media::CaptureRecorder	_recorder;
	//Media::VideoParams		_videoParams;
	//Media::AudioParams		_audioParams;
	RecordingInfo	_recordingInfo;
	//std::string		_recordingToken;
	int				_segmentDuration;
	bool			_synchronizeVideos;

	mutable Poco::FastMutex _mutex;
};


} } // namespace Sourcey::Spot


#endif // RECORDING_MODE_H