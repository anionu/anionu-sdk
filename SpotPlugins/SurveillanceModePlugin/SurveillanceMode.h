#ifndef Anionu_Spot_SurveillanceMode_H
#define Anionu_Spot_SurveillanceMode_H


#include "Anionu/Spot/API/IMode.h"
#include "Anionu/Spot/API/IModule.h"
#include "Anionu/Spot/API/IModule.h"
#include "Anionu/Spot/API/IMediaManager.h"
#include "Anionu/Spot/API/IStreamingSession.h"
#include "Anionu/Spot/API/IFormProcessor.h"
#include "Anionu/MotionDetector.h"
#include "Sourcey/PacketStream.h"
#include "Sourcey/Timer.h"


namespace scy {
namespace anionu { 
namespace spot {
	

class SurveillanceMode: 
	public api::IMode, 
	public api::IModule, 
	public api::IFormProcessor
{
public:
	SurveillanceMode(api::IEnvironment& env, const std::string& channel);
	~SurveillanceMode();
	
	//
	/// IMode interface
	bool activate();
	void deactivate();
	
	bool isActive() const;
	bool isRecording() const;
	const char* docFile() const;	
	const char* errorMessage() const;
	const char* channelName() const;
	const char* modeName() const { return "Surveillance Mode"; }
	
	//
	/// IModule methods	
	const char* className() const { return "SurveillanceMode"; }
	
	//
	/// Local methods
	void loadConfig();

	void startMotionDetector();
	void stopMotionDetector();
	
	void startRecording();
	void stopRecording();
	
	TimedToken* createStreamingToken(long duration = 20000);
	TimedToken* getStreamingToken(const std::string& token);
	bool removeStreamingToken(const std::string& token);

	//
	/// IFormProcessor methods
	void buildForm(symple::Form& form, symple::FormElement& element);
	void parseForm(symple::Form& form, symple::FormElement& element);

	//
	/// Callbacks
	void onMotionStateChange(void* sender, anionu::MotionDetectorState& state, const anionu::MotionDetectorState&);
	void onInitStreamingSession(void*, api::IStreamingSession& session, bool& handled);
	void onInitStreamingConnection(void*, api::IStreamingSession& session, PacketStream& stream, bool& handled);
	void onStreamingSessionStateChange(void*, api::StreamingState& state, const api::StreamingState&);	
	
protected:	
	mutable Poco::FastMutex _mutex;
	bool _isActive;
	bool _isConfiguring;
	bool _synchronizeVideos;
	std::string	_error;
	std::string _channel;
	std::string	_recordingToken;
	PacketStream _motionStream;
	anionu::MotionDetector _motionDetector;
	std::vector<TimedToken> _streamingTokens;
};


} } } // namespace scy::anionu::Spot


#endif // Anionu_Spot_SurveillanceMode_H