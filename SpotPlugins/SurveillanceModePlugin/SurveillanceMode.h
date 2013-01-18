#ifndef ANIONU_SPOT_SurveillanceMode_H
#define ANIONU_SPOT_SurveillanceMode_H


#include "Sourcey/Spot/IMode.h"
#include "Sourcey/Spot/IStreamingSession.h"
#include "Sourcey/Spot/IMediaManager.h"
#include "Sourcey/PacketStream.h"
#include "Sourcey/Timeout.h"
#include "Sourcey/CryptoProvider.h"
#include "Sourcey/Media/MotionDetector.h"


namespace Sourcey {
namespace Spot {
	

class Token: public Timeout
{
public:
	Token(long duration) : 
		Timeout(duration), _id(CryptoProvider::generateRandomKey(32)) {}

	//Token(const std::string& id, long duration = 10000) : 
	//	Timeout(duration), _id(id) {}

	Token(const std::string& id = CryptoProvider::generateRandomKey(32), long duration = 10000) : 
		Timeout(duration), _id(id) {}
	
	std::string id() const { return _id; }
	
	bool operator == (const Token& r) const {
		return id()  == r.id();
	}
	
	bool operator == (const std::string& r) const {
		return id() == r;
	}

protected:
	std::string _id;
};


typedef std::vector<Token> TokenList;


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
		/// Removes a streaming token reference.

	void onMotionStateChange(void* sender, Media::MotionDetectorState& state, const Media::MotionDetectorState&);
	void onInitializeStreamingSession(void*, IStreamingSession& session, bool& handled);
	void onInitializeStreamingConnection(void*, IStreamingSession& session, ConnectionStream& connection, bool& handled);
	void onStreamingSessionStateChange(void*, StreamingState& state, const StreamingState&);	

	bool isConfigurable() const;
	bool hasParsableConfig(Symple::Form& form) const;
	void printInformation(std::ostream& s);
	void buildConfigForm(Symple::Form& form, Symple::FormElement& element, bool defaultScope = false);
	void parseConfigForm(Symple::Form& form, Symple::FormElement& element);

	const char* className() const { return "Surveillance Mode"; }
	
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