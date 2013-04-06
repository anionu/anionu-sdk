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

	void activate();
	void deactivate();
	
	bool startRecording();
	bool stopRecording();
	
	void loadConfig();
	bool isConfigurable() const;
	bool hasParsableConfig(Symple::Form& form) const;
	void printInformation(std::ostream& s);
	RecordingAction& recordingAction();

	void buildConfigForm(Symple::Form& form, Symple::FormElement& element, bool defaultScope = false);
	void parseConfigForm(Symple::Form& form, Symple::FormElement& element);
	
	void onEncoderStateChange(void* sender, Media::EncoderState& state, const Media::EncoderState& oldState);

	virtual const char* className() const { return "Recording Mode"; }

private: 
	int		_segmentDuration;
	bool	_synchronizeVideos;
	RecordingAction	_recordingAction;

	mutable Poco::FastMutex _mutex;
};


} } // namespace Sourcey::Spot


#endif // ANIONU_SPOT_RecordingMode_H