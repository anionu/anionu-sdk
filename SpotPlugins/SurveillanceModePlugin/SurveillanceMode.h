#ifndef ANIONU_SPOT_SurveillanceMode_H
#define ANIONU_SPOT_SurveillanceMode_H


#include "Sourcey/Spot/IMode.h"
#include "Sourcey/Spot/IStreamingSession.h"
#include "Sourcey/Spot/IMediaManager.h"
#include "Sourcey/PacketStream.h"
#include "Sourcey/Token.h"
#include "Sourcey/Media/MotionDetector.h"


namespace Sourcey {
namespace Spot {
	

class SurveillanceMode: public IMode
{
public:
	SurveillanceMode(IEnvironment& env, IChannel& channel);
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
	
	bool isConfigurable() const;
	bool hasParsableConfig(Symple::Form& form) const;
	void buildConfigForm(Symple::Form& form, Symple::FormElement& element, bool defaultScope = false);
	void parseConfigForm(Symple::Form& form, Symple::FormElement& element);
	std::string infoFile();

	void onMotionStateChange(void* sender, Media::MotionDetectorState& state, const Media::MotionDetectorState&);
	void onInitializeStreamingSession(void*, IStreamingSession& session, bool& handled);
	void onInitializeStreamingConnection(void*, IStreamingSession& session, ConnectionStream& connection, bool& handled);
	void onStreamingSessionStateChange(void*, StreamingState& state, const StreamingState&);	

	const char* className() const { return "SurveillanceMode"; }
	
protected:	
	mutable Poco::FastMutex _mutex;
	Media::MotionDetector _motionDetector;
	PacketStream	_motionStream;
	TokenList		_mediaTokens;
	RecordingAction	_recordingAction;
	bool			_isConfiguring;
	bool			_synchronizeVideos;
};


} } // namespace Sourcey::Spot


#endif // ANIONU_SPOT_SurveillanceMode_H