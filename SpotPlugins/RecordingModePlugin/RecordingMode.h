#ifndef ANIONU_SPOT_RecordingMode_H
#define ANIONU_SPOT_RecordingMode_H


#include "Sourcey/Spot/IMode.h"
#include "Sourcey/Spot/IMediaManager.h"


namespace Sourcey {
namespace Spot {


class RecordingMode: public IMode
{
public:
	RecordingMode(IEnvironment& env, IChannel& channel);
	~RecordingMode();

	void initialize();
	void uninitialize();

	void enable();
	void disable();
	
	bool startRecording();
	bool stopRecording();
	
	void onEncoderStateChange(void* sender, Media::EncoderState& state, const Media::EncoderState& oldState);
	
	void loadConfig();

	bool isConfigurable() const;
	bool hasParsableConfig(Symple::Form& form) const;
	void printInformation(std::ostream& s);
	void buildConfigForm(Symple::Form& form, Symple::FormElement& element, bool baseScope = false);
	void parseConfigForm(Symple::Form& form, Symple::FormElement& element);

	//void printInformation(std::ostream& s);
	//void buildConfigForm(Symple::Form& form, Symple::FormElement& element, bool baseScope = false);
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


#endif // ANIONU_SPOT_RecordingMode_H