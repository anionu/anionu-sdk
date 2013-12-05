#include "peerconnectionclient.h"
#include "videostreamcapturer.h"
#include "webrtcplugin.h"
#include "anionu/spot/api/client.h"
#include "scy/application.h"
#include "scy/symple/client.h"
#include "scy/net/sslmanager.h"
#include "scy/util.h"

#include <iostream>
#include <string>


using std::endl;


namespace scy {
namespace anio { 
namespace spot {
	

const char kStunServerUri[] = "stun:stun.l.google.com:19302"; // TODO: TURN server
const char kAudioLabel[] = "audio_label";
const char kVideoLabel[] = "video_label";
const char kStreamLabel[] = "stream_label";


PeerConnectionClient::PeerConnectionClient(WebRTCPlugin& observer, api::StreamingSession& session) : 
	observer_(observer), session_(session)
{
	peer_connection_factory_ = webrtc::CreatePeerConnectionFactory();
	server_.uri = kStunServerUri;
	servers_.push_back(server_);
	constraints_.SetMandatoryReceiveAudio(false);
	constraints_.SetMandatoryReceiveVideo(false);
	//constraints_.SetAllowRtpDataChannels();
}


PeerConnectionClient::~PeerConnectionClient() 
{
	closeConnection();
}


bool PeerConnectionClient::initConnection(PacketSignal& source) 
{
	peer_connection_ = peer_connection_factory_->CreatePeerConnection(servers_, &constraints_, NULL, this);
	stream_ = peer_connection_factory_->CreateLocalMediaStream(kStreamLabel);
	
	// Add our custom video stream track
	talk_base::scoped_refptr<webrtc::VideoTrackInterface> video_track(
		peer_connection_factory_->CreateVideoTrack(kVideoLabel, 
			peer_connection_factory_->CreateVideoSource(new VideoStreamCapturer(source), NULL)));
	stream_->AddTrack(video_track);
	
	// TODO: Select device ID based on channel defaults
	if (!session_.options().disableAudio) {
		talk_base::scoped_refptr<webrtc::AudioTrackInterface> audio_track(
			peer_connection_factory_->CreateAudioTrack(kAudioLabel, 
				peer_connection_factory_->CreateAudioSource(NULL)));
		stream_->AddTrack(audio_track);
	}

	if (!peer_connection_->AddStream(stream_, &constraints_)) {
		ErrorL << "#### Adding stream to PeerConnection failed" << endl;
		assert(0);
		return false;
	}

	return true;
}


bool PeerConnectionClient::closeConnection()
{
	if (peer_connection_) {
		peer_connection_->Close();
		return true;
	}
	return false;
}


void PeerConnectionClient::createOffer() 
{
	peer_connection_->CreateOffer(this, &constraints_);
}


bool PeerConnectionClient::handleRemoteOffer(const std::string& type, const std::string sdp) 
{
	webrtc::SessionDescriptionInterface* desc(webrtc::CreateSessionDescription(type, sdp));
	if (!desc) return false;	
	peer_connection_->SetRemoteDescription(DummySetSessionDescriptionObserver::Create(), desc);
	peer_connection_->CreateAnswer(this, &constraints_);
	return true;
}


bool PeerConnectionClient::handleRemoteAnswer(const std::string& type, const std::string sdp) 
{	
	webrtc::SessionDescriptionInterface* desc(webrtc::CreateSessionDescription(type, sdp));
	if (!desc) return false;		
	peer_connection_->SetRemoteDescription(DummySetSessionDescriptionObserver::Create(), desc);
	return true;
}


bool PeerConnectionClient::handleRemoteCandidate(const std::string& mid, int mlineindex, const std::string sdp)
{			
	talk_base::scoped_ptr<webrtc::IceCandidateInterface> candidate(webrtc::CreateIceCandidate(mid, mlineindex, sdp));
	return candidate.get() && peer_connection_->AddIceCandidate(candidate.get());
}


void PeerConnectionClient::OnError() 
{
	ErrorL << "### PeerConnection error" << endl;
	assert(0);
}


void PeerConnectionClient::OnSignalingChange(webrtc::PeerConnectionInterface::SignalingState new_state)
{	
	DebugL << "### PeerConnection signaling state change: " << new_state << endl;

	switch(new_state) {
	case webrtc::PeerConnectionInterface::kStable:	
		session_.addRef();
		break;
		
	case webrtc::PeerConnectionInterface::kClosed:		
		session_.removeRef();
		break;
	}
}


void PeerConnectionClient::OnStateChange(webrtc::PeerConnectionObserver::StateType state_changed) 
{	
	DebugL << "### PeerConnection state change: " << state_changed << endl;
}


void PeerConnectionClient::OnAddStream(webrtc::MediaStreamInterface* stream) 
{
	DebugL << "### PeerConnection error" << endl;
}


void PeerConnectionClient::OnRemoveStream(webrtc::MediaStreamInterface* stream)
{
}


void PeerConnectionClient::OnIceCandidate(const webrtc::IceCandidateInterface* candidate) 
{	
	std::string sdp;
	if (!candidate->ToString(&sdp)) {
		ErrorL << "#### Failed to serialize candidate" << endl;
		assert(0);
		return;
	}	
	
	observer_.onLocalCandidate(session_, candidate->sdp_mid(), candidate->sdp_mline_index(), sdp);
	
	//session_.addCandidate(
	//	spot::api::makeWebRtcCandidate(
	//		candidate->sdp_mid(), candidate->sdp_mline_index(), sdp));
	//session_.broadcastCandidate(candidate->sdp_mid(), candidate->sdp_mline_index(), sdp);
}


void PeerConnectionClient::OnSuccess(webrtc::SessionDescriptionInterface* desc) 
{
	DebugL << "### Set local description" << endl;
	peer_connection_->SetLocalDescription(
		DummySetSessionDescriptionObserver::Create(), desc);
		
	// Send an SDP offer to the peer
	std::string type(desc->type());
	std::string sdp;
	desc->ToString(&sdp);
	
	observer_.onLocalOffer(session_, type, sdp);
}


void PeerConnectionClient::OnFailure(const std::string &error) 
{
	ErrorL << "### " << error << endl;
	assert(0);
}


} } } // namespace scy::anio::spot