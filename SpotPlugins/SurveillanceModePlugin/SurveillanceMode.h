#ifndef Anionu_Spot_SurveillanceMode_H
#define Anionu_Spot_SurveillanceMode_H


#include "Anionu/Spot/API/IMode.h"
#include "Anionu/Spot/API/IModule.h"
#include "Anionu/Spot/API/IModule.h"
#include "Anionu/Spot/API/MediaManager.h"
#include "Anionu/Spot/API/StreamingSession.h"
#include "Anionu/Spot/API/IFormProcessor.h"
#include "Anionu/MotionDetector.h"
#include "Sourcey/PacketStream.h"
#include "Sourcey/Timer.h"


namespace scy {
namespace anio { 
namespace spot {
	

class SurveillanceMode: 
	public api::IMode, 
	public api::IModule, 
	public api::IFormProcessor
{
public:
	SurveillanceMode(api::Environment& env, const std::string& channel);
	virtual ~SurveillanceMode();
	
	//
	/// IMode interface
	virtual bool activate();
	virtual void deactivate();
	
	bool isActive() const;
	bool isRecording() const;
	virtual const char* docFile() const;	
	virtual const char* errorMessage() const;
	virtual const char* channelName() const;
	virtual const char* modeName() const { return "Surveillance Mode"; }
	
	//
	/// IModule methods	
	virtual const char* className() const { return "SurveillanceMode"; }
	
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
	virtual void buildForm(smpl::Form& form, smpl::FormElement& element);
	virtual void parseForm(smpl::Form& form, smpl::FormElement& element);

	//
	/// Callbacks
	void onMotionStateChange(void* sender, anio::MotionDetectorState& state, const anio::MotionDetectorState&);
	void onInitStreamingSession(void*, api::StreamingSession& session, bool& handled);
	void onInitStreamingConnection(void*, api::StreamingSession& session, PacketStream& stream, bool& handled);
	void onStreamingSessionStateChange(void*, api::StreamingState& state, const api::StreamingState&);
	
protected:	
	mutable Mutex _mutex;
	bool _isActive;
	bool _isConfiguring;
	bool _synchronizeVideos;
	std::string	_error;
	std::string _channel;
	std::string	_recordingToken;
	std::vector<TimedToken> _streamingTokens;
	anio::MotionDetector _motionDetector;
	PacketStream _motionStream;
};


} } } // namespace scy::anio::spot


#endif // Anionu_Spot_SurveillanceMode_H