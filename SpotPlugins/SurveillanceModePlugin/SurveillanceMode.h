#ifndef ANIONU_SPOT_SurveillanceMode_H
#define ANIONU_SPOT_SurveillanceMode_H


#include "Anionu/Spot/API/IMode.h"
#include "Anionu/Spot/API/IMediaManager.h"
#include "Anionu/Spot/API/IStreamingSession.h"
#include "Anionu/Spot/API/ISympleProcessor.h"
#include "Anionu/MotionDetector.h"
#include "Sourcey/PacketStream.h"
#include "Sourcey/Token.h"


namespace Scy {
namespace Anionu { 
namespace Spot {
	

class SurveillanceMode: public API::IMode
{
public:
	SurveillanceMode(API::IEnvironment& env, API::IChannel& channel);
	~SurveillanceMode();
	
	void initialize();
	void uninitialize();
	
	void activate();
	void deactivate();
	
	void loadConfig();

	bool startMotionDetector();
	bool stopMotionDetector();
	
	bool startRecording();
	bool stopRecording();
	
	Token* createStreamingToken(long duration = 20000);
	Token* getStreamingToken(const std::string& token);
	bool removeStreamingToken(const std::string& token);

	//
	/// Symple::ISympleFormProcessor methods
	bool isConfigurable() const;
	bool hasParsableFields(Symple::Form& form) const;
	void buildForm(Symple::Form& form, Symple::FormElement& element);
	void parseForm(Symple::Form& form, Symple::FormElement& element);
	std::string helpFile();

	void onMotionStateChange(void* sender, Anionu::MotionDetectorState& state, const Anionu::MotionDetectorState&);
	void onSetupStreamingSession(void*, API::IStreamingSession& session, bool& handled);
	void onSetupStreamingConnection(void*, API::IStreamingSession& session, API::ConnectionStream& connection, bool& handled);
	void onStreamingSessionStateChange(void*, API::StreamingState& state, const API::StreamingState&);	

	const char* className() const { return "SurveillanceMode"; }
	
protected:	
	mutable Poco::FastMutex _mutex;
	Anionu::MotionDetector _motionDetector;
	PacketStream	_motionStream;
	std::string		_recordingToken;
	TokenList		_streamingTokens;
	bool			_isConfiguring;
	bool			_synchronizeVideos;
};


} } } // namespace Scy::Anionu::Spot


#endif // ANIONU_SPOT_SurveillanceMode_H