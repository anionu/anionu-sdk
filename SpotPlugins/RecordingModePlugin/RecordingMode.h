#ifndef Anionu_Spot_RecordingMode_H
#define Anionu_Spot_RecordingMode_H


#include "Anionu/Spot/API/IMode.h"
#include "Anionu/Spot/API/IModule.h"
#include "Anionu/Spot/API/IChannel.h"
#include "Anionu/Spot/API/IMediaManager.h"
#include "Anionu/Spot/API/IFormProcessor.h"


namespace scy {
namespace anionu { 
namespace spot {


class RecordingMode: 
	public api::IMode, 
	public api::IModule, 
	public api::IFormProcessor
{
public:
	RecordingMode(api::IEnvironment& env, const std::string& channel);
	~RecordingMode();
	
	bool activate();
	void deactivate();
		
	void startRecording();
	void stopRecording();
	
	void loadConfig();
	bool isActive() const;
	bool isRecording() const;
	std::string recordingToken();
		
	void buildForm(symple::Form& form, symple::FormElement& element);
	void parseForm(symple::Form& form, symple::FormElement& element);
		
	void onRecordingStopped(void* sender, api::RecorderStream& stream);
	
	const char* docFile() const;	
	const char* errorMessage() const;
	const char* channelName() const;
	const char* modeName() const { return "Recording Mode"; }
	const char* className() const { return "RecordingMode"; }

private: 
	int	_segmentDuration;
	bool _synchronizeVideos;
	bool _isActive;
	std::string _error;
	std::string _channel;
	std::string	_recordingToken;
	mutable Poco::FastMutex _mutex;
};


} } } // namespace scy::anionu::Spot


#endif // Anionu_Spot_RecordingMode_H



	/*
	//
	/// symple::IFormProcessor methods
	//bool isConfigurable() const;
	//bool hasParsableFields(symple::Form& form) const;
	
	//void initialize();
	//void uninitialize();
	bool isConfigurable() const;
	bool hasParsableFields(symple::Form& form) const;	
	std::string docFile();

	void buildForm(symple::Form& form, symple::FormElement& element, bool defaultScope = false);
	void parseForm(symple::Form& form, symple::FormElement& element);
	*/
	
	//void onRecordingStarted(void* sender, api::RecorderStream& stream);
	//void onEncoderStateChange(void* sender, av::EncoderState& state, const av::EncoderState& oldState);
	//Signal2<const api::RecordingOptions&, api::RecordingStream*&> RecordingStarted;
	//Signal2<const api::RecordingOptions&, api::RecordingStream*&> RecordingStopped;
	//env().media().RecordingStarted += delegate(this, &RecordingMode::onRecordingStarted);
	//env().media().RecordingStopped += delegate(this, &RecordingMode::onRecordingStopped);