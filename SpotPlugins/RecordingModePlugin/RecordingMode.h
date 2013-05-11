#ifndef ANIONU_SPOT_RecordingMode_H
#define ANIONU_SPOT_RecordingMode_H


#include "Anionu/Spot/API/IMode.h"
#include "Anionu/Spot/API/IMediaManager.h"


namespace Scy {
namespace Anionu { 
namespace Spot {


class RecordingMode: public API::IMode
{
public:
	RecordingMode(API::IEnvironment& env,  API::IChannel& channel);
	~RecordingMode();

	void initialize();
	void uninitialize();

	void activate();
	void deactivate();
	
	bool startRecording();
	bool stopRecording();
	
	void loadConfig();
	std::string recordingToken();
	
	void onRecordingStopped(void* sender, API::RecorderStream& stream);

	virtual const char* className() const { return "RecordingMode"; }

private: 
	int	_segmentDuration;
	bool _synchronizeVideos;
	std::string	_recordingToken;
	mutable Poco::FastMutex _mutex;
};


// ---------------------------------------------------------------------
//
class RecordingModeFormProcessor: public Symple::IFormProcessor
{	
	RecordingModeFormProcessor(RecordingMode& mode) : mode(mode) {};

	bool isConfigurable() const;
	bool hasParsableFields(Symple::Form& form) const;
	void buildForm(Symple::Form& form, Symple::FormElement& element);
	void parseForm(Symple::Form& form, Symple::FormElement& element);
	std::string documentFile();

	RecordingMode& mode;
};


} } } // namespace Scy::Anionu::Spot


#endif // ANIONU_SPOT_RecordingMode_H



	/*
	bool isConfigurable() const;
	bool hasParsableFields(Symple::Form& form) const;	
	std::string documentFile();

	void buildForm(Symple::Form& form, Symple::FormElement& element, bool defaultScope = false);
	void parseForm(Symple::Form& form, Symple::FormElement& element);
	*/
	
	//void onRecordingStarted(void* sender, API::RecorderStream& stream);
	//void onEncoderStateChange(void* sender, Media::EncoderState& state, const Media::EncoderState& oldState);
	//Signal2<const API::RecordingOptions&, API::RecordingStream*&> RecordingStarted;
	//Signal2<const API::RecordingOptions&, API::RecordingStream*&> RecordingStopped;
	//env().media().RecordingStarted += delegate(this, &RecordingMode::onRecordingStarted);
	//env().media().RecordingStopped += delegate(this, &RecordingMode::onRecordingStopped);