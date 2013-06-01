#ifndef ANIONU_SPOT_SurveillanceMode_H
#define ANIONU_SPOT_SurveillanceMode_H


#include "Anionu/Spot/API/IMode.h"
#include "Anionu/Spot/API/IModule.h"
#include "Anionu/Spot/API/IModule.h"
#include "Anionu/Spot/API/IMediaManager.h"
#include "Anionu/Spot/API/IStreamingSession.h"
#include "Anionu/Spot/API/IFormProcessor.h"
#include "Anionu/MotionDetector.h"
#include "Sourcey/PacketStream.h"
#include "Sourcey/Token.h"


namespace Scy {
namespace Anionu { 
namespace Spot {
	

class SurveillanceMode: 
	public API::IMode, 
	public API::IModule, 
	public API::IFormProcessor
{
public:
	SurveillanceMode(API::IEnvironment& env, const std::string& channel);
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
	
	Token* createStreamingToken(long duration = 20000);
	Token* getStreamingToken(const std::string& token);
	bool removeStreamingToken(const std::string& token);

	//
	/// IFormProcessor methods
	void buildForm(Symple::Form& form, Symple::FormElement& element);
	void parseForm(Symple::Form& form, Symple::FormElement& element);

	//
	/// Callbacks
	void onMotionStateChange(void* sender, Anionu::MotionDetectorState& state, const Anionu::MotionDetectorState&);
	void onInitStreamingSession(void*, API::IStreamingSession& session, bool& handled);
	void onInitStreamingConnection(void*, API::IStreamingSession& session, API::ConnectionStream& connection, bool& handled);
	void onStreamingSessionStateChange(void*, API::StreamingState& state, const API::StreamingState&);	
	
protected:	
	mutable Poco::FastMutex _mutex;
	Anionu::MotionDetector _motionDetector;
	PacketStream _motionStream;
	std::string	_error;
	std::string _channel;
	std::string	_recordingToken;
	TokenList _streamingTokens;
	bool _isActive;
	bool _isConfiguring;
	bool _synchronizeVideos;
};


} } } // namespace Scy::Anionu::Spot


#endif // ANIONU_SPOT_SurveillanceMode_H