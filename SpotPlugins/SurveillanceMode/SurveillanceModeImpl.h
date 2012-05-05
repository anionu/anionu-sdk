#ifndef SURVEILLANCE_MODE_H
#define SURVEILLANCE_MODE_H


#include "Sourcey/Spot/IMode.h"
#include "Sourcey/Spot/IStreamingSession.h"
#include "Sourcey/Spot/IMediaManager.h"
#include "Sourcey/PacketStream.h"
#include "Sourcey/Timeout.h"
#include "Sourcey/Media/MotionDetector.h"


namespace Sourcey {
namespace Spot {


class SurveillanceModeImpl: public IMode
{
public:
	SurveillanceModeImpl(IEnvironment& env, IChannel& channel);
	~SurveillanceModeImpl();
	
	void initialize();
	void uninitialize();
	
	void enable();
	void disable();
	
	void loadConfig();

	bool startMotionDetector();
	bool stopMotionDetector();
	
	void startRecording();
	void stopRecording();

	void onMotionStateChange(void* sender, Media::MotionDetectorState& state, const Media::MotionDetectorState&);
	void onInitializeStreamingSession(void*, IStreamingSession& session, bool& handled);
	void onInitializeStreamingConnection(void*, IStreamingSession& session, ConnectionStream& connection, bool& handled);
	void onSessionStreamStateChange(void*, PacketStreamState& state, const PacketStreamState&);	
	void onEncoderStateChange(void* sender, Media::EncoderState& state, const Media::EncoderState&);

	bool isConfigurable() const;
	bool hasParsableConfig(Symple::Form& form) const;
	void createHelp(std::ostream& s);
	void createConfigForm(Symple::Form& form, Symple::FormElement& element, bool useBase = false);
	void parseConfigForm(Symple::Form& form, Symple::FormElement& element);

	const char* className() const { return "Surveillance Mode"; }

	/*
	PacketDispatcher _fuck;
	MutexStateful<PacketStreamState> _fucka;
	Poco::Event _ready;

	mutable Poco::Mutex	_mutex1;
	*/
	
protected:

	Media::MotionDetector	_motionDetector;
	PacketStream	_motionStream;
	std::string		_mediaToken;
	Timeout			_mediaTokenTimeout;
	bool			_isConfiguring;
	RecordingInfo	_recordingInfo;
	mutable Poco::FastMutex _mutex;
};


} } // namespace Sourcey::Spot


#endif // SURVEILLANCE_MODE_H