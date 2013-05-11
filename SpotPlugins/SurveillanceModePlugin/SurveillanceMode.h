#ifndef ANIONU_SPOT_SurveillanceMode_H
#define ANIONU_SPOT_SurveillanceMode_H


#include "Anionu/Spot/API/IMode.h"
#include "Anionu/Spot/API/IStreamingSession.h"
#include "Anionu/Spot/API/IMediaManager.h"
#include "Sourcey/PacketStream.h"
#include "Sourcey/Token.h"
#include "Sourcey/Media/MotionDetector.h"


namespace Scy {
namespace Anionu { 
namespace Spot {

	
using namespace API;
	/// Specify the API namespace.
	/// Use this value to switch SpotAPI versions.
	

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

	void onMotionStateChange(void* sender, Media::MotionDetectorState& state, const Media::MotionDetectorState&);
	void onSetupStreamingSession(void*, IStreamingSession& session, bool& handled);
	void onSetupStreamingConnection(void*, IStreamingSession& session, ConnectionStream& connection, bool& handled);
	void onStreamingSessionStateChange(void*, API::StreamingState& state, const API::StreamingState&);	

	const char* className() const { return "SurveillanceMode"; }
	
protected:	
	mutable Poco::FastMutex _mutex;
	Media::MotionDetector _motionDetector;
	PacketStream	_motionStream;
	std::string		_recordingToken;
	TokenList		_streamingTokens;
	bool			_isConfiguring;
	bool			_synchronizeVideos;
};


// ---------------------------------------------------------------------
//
class SurveillanceModeFormProcessor: public Symple::IFormProcessor
{	
	SurveillanceModeFormProcessor(SurveillanceMode& mode) : mode(mode) {};

	bool isConfigurable() const;
	bool hasParsableFields(Symple::Form& form) const;
	void buildForm(Symple::Form& form, Symple::FormElement& element);
	void parseForm(Symple::Form& form, Symple::FormElement& element);
	std::string documentFile();

	SurveillanceMode& mode;
};


} } } // namespace Scy::Anionu::Spot


#endif // ANIONU_SPOT_SurveillanceMode_H