#include "webrtcplugin.h"
#include "anionu/spot/api/environment.h"
#include "anionu/spot/api/synchronizer.h"
#include "anionu/spot/api/client.h"
#include "scy/media/formatregistry.h"
#include "scy/symple/client.h"


using std::endl;


namespace scy {
namespace anio { 
namespace spot {


SPOT_CORE_PLUGIN(
	WebRTCPlugin, 
	"WebRTC Plugin", 
	"0.1.0"
)


WebRTCPlugin::WebRTCPlugin(api::Environment& env) :
	api::IModule(env)
{
	// Use the Spot application logger
	Logger::setInstance(&env.logger());
}


WebRTCPlugin::~WebRTCPlugin()
{	
	Logger::setInstance(NULL, false);
}


bool WebRTCPlugin::load() 
{
	try {
		DebugL << "Loading" << endl;

		// TODO: Test WebRTC subsytem to check everything is in order.
		
		// WebRTC is the preferred media transport method for browser clients that support it.
		env().streaming().SetupStreamingEncoders += delegate(this, &WebRTCPlugin::onSetupStreamingEncoders);		
	}
	catch (std::exception& exc) {
		_error = std::string(exc.what());
		ErrorL << "Load failed: " << exc.what() << endl;
		return false;
	}
	return true;
}


void WebRTCPlugin::unload() 
{	
	DebugL << "Unloading" << endl;

	env().streaming().SetupStreamingEncoders.detach(this);

	DebugL << "Unloading: Cleanup" << endl;
	_manager.clear();
	DebugL << "Unloading: Cleanup: OK" << endl;
}


void WebRTCPlugin::onEvent(const char* event)
{
	smpl::Event e;
	e.read(event);	
	bool isSDP = e.name() == "Streaming:SDP";
	bool isCandidate = e.name() == "Streaming:Candidate";
	if (!isCandidate && !isSDP) return;

	DebugL << "Recv WebRTC event: " << event << endl;
	
	std::string token(e.data("token").asString());
	PeerConnectionClient* client = _manager.get(token, false);
	if (!client) {
		ErrorL << "Received event for invalid session: " << token << endl;
		return;
	}
	
	if (isSDP) {
		json::Value desc(e.data("description"));
		if (!desc.isObject()) return;
		std::string type(desc.get("type", "").asString());
		std::string sdp(desc.get("sdp", "").asString());
		if (type == "offer") {
			DebugL << "Handle remote offer" << endl;
			if (!client->handleRemoteOffer(type, sdp)) {
				ErrorL << "Failed to  parse remote offer" << endl;
				assert(0 && "cannot parse remote offer");
				return;
			}
		}
		else if (type == "answer") {
			DebugL << "Handle remote answer" << endl;
			if (!client->handleRemoteAnswer(type, sdp)) {
				ErrorL << "Failed to parse remote answer" << endl;
				assert(0 && "cannot parse remote answer");
				return;
			}
		}
	}
	else if (isCandidate) {	
		json::Value cand(e.data("candidate"));
		std::string sdpMId = cand.get("sdpMid", "").asString();
		int sdpMLineLndex = cand.get("sdpMLineIndex", -1).asInt();
		std::string sdpCandidate = cand.get("candidate", "").asString();
		if (sdpMLineLndex == -1 || sdpMId.empty() || sdpCandidate.empty()) {
			ErrorL << "Invalid candidate format" << endl;
			assert(0 && "bad candiate");
			return;
		}
		DebugL << "Handle remote candidate" << endl;
		if (!client->handleRemoteCandidate(sdpMId, sdpMLineLndex, sdpCandidate)) {
			ErrorL << "Failed to apply the received candidate" << endl;
			assert(0 && "cannot parse candiate");
			return;
		}
	}
}
	

void WebRTCPlugin::onLocalOffer(api::StreamingSession& session, const std::string& type, const std::string sdp)
{	
	DebugL << "On local offerr: " << sdp << endl;

	assert(type == "offer");

	smpl::Peer* peer =  env().client().getPeer(session.options().peer);
	if (!peer) {
		WarnL << "The remote peer has gone offline" << endl;
		assert(0 && "peer mismatch");
		return;
	}	

	smpl::Event e;
	e.setName("Streaming:SDP");
	e.setTo(*peer);
	Json::Value desc;
	desc["type"] = type;
	desc["sdp"] = sdp;
	e.setData("token", session.options().token);
	e.setData("channel", session.options().channel);
	e.setData("description", desc);	
	env().client().sendMessage(e);
}


void WebRTCPlugin::onLocalAnswer(api::StreamingSession& session, const std::string& type, const std::string sdp)
{
	DebugL << "On local answer: " << sdp << endl;

	assert(type == "answer");
	assert(0 && "not supported");
}


void WebRTCPlugin::onLocalCandidate(api::StreamingSession& session, const std::string& mid, int mlineindex, const std::string sdp)
{	
	DebugL << "On local candidate: " << sdp << endl;

	session.addCandidate(spot::api::makeWebRtcCandidate(mid, mlineindex, sdp));
}


void WebRTCPlugin::onSetupStreamingEncoders(void*, api::StreamingSession& session, bool& handled)
{
	DebugL << "Initialize streaming session: " << session.token() << endl;
		
	// Override stream processors if the client supports WebRTC.	
	if (session.options().supportWebRTC) {
		if (_manager.exists(session.token())) {
			ErrorL << "Session already exists for token: " << session.token() << endl;
			return;
		}

		auto conn = new PeerConnectionClient(*this, session);
		if (conn->initConnection(session.captureStream().emitter)) {
			conn->createOffer();
			_manager.add(session.token(), conn);
			
			session.StateChange += delegate(this, &WebRTCPlugin::onStreamingSessionStateChange);
	
			DebugL << "Created WebRTC streaming session: " << session.token() << endl;
		}
		else delete conn;
	}
}


void WebRTCPlugin::onStreamingSessionStateChange(void* sender, api::StreamingState& state, const api::StreamingState& oldState)
{
	DebugL << "Session state change: " << oldState << " => " << state << endl;

	auto session = reinterpret_cast<api::StreamingSession*>(sender);

	switch (state.id()) {
	case api::StreamingState::Initializing: break;
	case api::StreamingState::Ready: break;
	case api::StreamingState::Active: break;		
	case api::StreamingState::Error: break;
	case api::StreamingState::Terminated:
		session->StateChange.detach(this);
		bool exists = _manager.free(session->token());
		assert(exists);
		break;
	}
}


const char* WebRTCPlugin::errorMessage() const 
{ 
	return _error.empty() ? 0 : _error.c_str();
}


} } } // namespace scy::anio::spot




//bool WebRTCPlugin::onMessage(const char* /* message */)
//{
//	// Parse the Message and do something with it:
//	// smpl::Message m;
//	// m.read(message);
//	// log("Recv Message: " + m.toStyledString());
//	//
//	// Return true if the message has been responded to, 
//	// false otherwise.
//	return false;
//}
//
//
//bool WebRTCPlugin::onCommand(const char* /* command */)
//{
//	// Parse the Command and do something with it:
//	// smpl::Command c;
//	// c.read(command);
//	// log("Recv Command: " + c.toStyledString());	
//	//
//	// Return true if the message has been responded to, 
//	// false otherwise.
//	return false;
//}
//
//
//void WebRTCPlugin::onEvent(const char* /* event */)
//{
//	// Parse the Event and do something with it:
//	// smpl::Event e;
//	// e.read(event);
//	// log("Recv Event: " + e.toStyledString());
//}
//
//
//void WebRTCPlugin::onPresence(const char* /* presence */) 
//{
//	// Parse the Presence and do something with it:
//	// smpl::Message p;
//	// p.read(presence);	
//	// log("Recv Presence: " + p.toStyledString());
//}


	/*


void WebRTCPlugin::onSetupRecordingEncoders(void*, api::RecorderSession& session, bool& handled)
{
	log("Initialize Recording Encoder");
}


	// Create the encoder unless recording raw video.
	if (options.oformat.id != "rawvideo" && !handled) {
		encoder = createEncoder(options); //reinterpret_cast<api::Recorder*>();
		log("Initializing Recording Encoder: OK");
		
		// Attach the encoder to the packet stream.
		if (encoder)
			session.stream().attach(encoder, 5, true);	
	}
	*/


	/*
	// Create the encoder unless streaming raw video.
	if (session.options().oformat.id != "rawvideo" && !handled) {			
		assert(session.stream().base().getProcessor<av::IPacketEncoder>() == NULL);
	
		av::RecordingOptions options(
			session.options().iformat, 
			session.options().oformat);
		av::AVPacketEncoder* encoder = createEncoder(options);
		
		// Attach the encoder to the packet stream.
		if (encoder)
			session.stream().attach(encoder, 5, true);			
	}
	*/


/*
av::AVPacketEncoder* WebRTCPlugin::createEncoder(const av::RecordingOptions& options)
{
	log("Initializing AV Encoder");	
	
	// Create the encoder and return the instance pointer.	 
	// the returned pointer must extend from IPacketEncoder.
	av::AVPacketEncoder* encoder = nullptr;
	try {
		encoder = new av::AVPacketEncoder(options);
		encoder->initialize();
	}
	catch (std::exception& exc) {
		log("Encoder Error: " + std::string(exc.what()), "error");
		if (encoder)
			delete encoder;
		encoder = nullptr;
	}
	return encoder;
}
*/