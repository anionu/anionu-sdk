#ifndef Anionu_Spot_WebRTCPlugin_H
#define Anionu_Spot_WebRTCPlugin_H


#include "peerconnectionclient.h"
#include "anionu/spot/api/iplugin.h"
#include "anionu/spot/api/imode.h"
#include "anionu/spot/api/iplugin.h"
#include "anionu/spot/api/imodule.h"
#include "anionu/spot/api/iformprocessor.h"
#include "anionu/spot/api/imessageprocessor.h"
#include "anionu/spot/api/streamingmanager.h"
#include "anionu/spot/api/mediamanager.h"
#include "scy/media/avpacketencoder.h"
//#include "scy/symple/client.h"


namespace scy {
namespace anio { 
namespace spot {


typedef PointerCollection<std::string, PeerConnectionClient> ClientManager;


class WebRTCPlugin: 
	public api::IPlugin,
	public api::IMessageProcessor,
	public api::IModule
{
public:
	WebRTCPlugin(api::Environment& env);
	virtual ~WebRTCPlugin();

	bool load();
	void unload();
	
	/// IMessageProcessor
	//bool onMessage(const char* message);
	//bool onCommand(const char* command);
	//void onPresence(const char* presence);
	void onEvent(const char* event);
	
	void onLocalOffer(api::StreamingSession& session, const std::string& type, const std::string sdp);
	void onLocalAnswer(api::StreamingSession& session, const std::string& type, const std::string sdp);
	void onLocalCandidate(api::StreamingSession& session, const std::string& mid, int mlineindex, const std::string sdp);
	
	void onSetupStreamingEncoders(void*, api::StreamingSession& session, bool& handled);
	void onStreamingSessionStateChange(void* sender, 
		api::StreamingState& state, const api::StreamingState& oldState);

	const char* errorMessage() const;
	const char* className() const { return "WebRTCPlugin"; }
	
protected:	
	std::string	_error;
	ClientManager _manager;
};


#if 0
inline PacketStreamAdapter* getLastStreamAdapter(PacketStream& stream)
{	
	PacketAdapterReference* res = nullptr;
	auto adapters = stream.base().adapters();
	for (auto& ref : adapters) {
		if (!res || res->order < ref.order)
			res = &ref;
	}
	return res ? res->ptr : nullptr;
}
#endif


} } } // namespace scy::anio::spot


	/*
	//mutable Mutex _mutex;

	*/
	//av::AVPacketEncoder* createEncoder(const av::RecordingOptions& options);	
	//void registerMediaFormats();
#endif // Anionu_Spot_WebRTCPlugin_H


	//void onSetupRecordingEncoders(void*, api::RecorderSession& session, bool& handled);