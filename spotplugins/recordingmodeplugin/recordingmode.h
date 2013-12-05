#ifndef Anionu_Spot_RecordingMode_H
#define Anionu_Spot_RecordingMode_H


#include "anionu/spot/api/imode.h"
#include "anionu/spot/api/imodule.h"
#include "anionu/spot/api/channel.h"
#include "anionu/spot/api/mediamanager.h"
#include "anionu/spot/api/iformprocessor.h"


namespace scy {
namespace anio { 
namespace spot {


class RecordingMode: 
	public api::IMode, 
	public api::IModule, 
	public api::IFormProcessor
{
public:
	RecordingMode(api::Environment& env, const std::string& channel);
	virtual ~RecordingMode();
	
	virtual bool activate();
	virtual void deactivate();
		
	void startRecording();
	void stopRecording();
	
	void loadConfig();
	bool isActive() const;
	bool isRecording() const;
	std::string recordingToken();
		
	virtual void buildForm(smpl::Form& form, smpl::FormElement& element);
	virtual void parseForm(smpl::Form& form, smpl::FormElement& element);
		
	void onRecordingStopped(void* sender, api::RecorderSession& recorder);
	
	virtual const char* docFile() const;	
	virtual const char* errorMessage() const;
	virtual const char* channelName() const;
	virtual const char* modeName() const { return "Recording Mode"; }
	virtual const char* className() const { return "RecordingMode"; }

private: 
	int	_segmentDuration;
	bool _synchronizeVideos;
	bool _isActive;
	std::string _error;
	std::string _channel;
	std::string	_recordingToken;
	mutable Mutex _mutex;
};


} } } // namespace scy::anio::spot


#endif // Anionu_Spot_RecordingMode_H



	/*
	//
	/// smpl::IFormProcessor methods
	//bool isConfigurable() const;
	//bool hasParsableFields(smpl::Form& form) const;
	
	//void initialize();
	//void uninitialize();
	bool isConfigurable() const;
	bool hasParsableFields(smpl::Form& form) const;	
	std::string docFile();

	void buildForm(smpl::Form& form, smpl::FormElement& element, bool defaultScope = false);
	void parseForm(smpl::Form& form, smpl::FormElement& element);
	*/
	
	//void onRecordingStarted(void* sender, api::RecorderStream& stream);
	//void onEncoderStateChange(void* sender, av::EncoderState& state, const av::EncoderState& oldState);
	//Signal2<const api::RecordingOptions&, api::RecordingStream*&> RecordingStarted;
	//Signal2<const api::RecordingOptions&, api::RecordingStream*&> RecordingStopped;
	//env().media().RecordingStarted += delegate(this, &RecordingMode::onRecordingStarted);
	//env().media().RecordingStopped += delegate(this, &RecordingMode::onRecordingStopped);