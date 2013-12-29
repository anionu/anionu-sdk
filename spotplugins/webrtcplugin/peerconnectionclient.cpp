//
// Anionu SDK
// Copyright (C) 2011, Anionu <http://anionu.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

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
	

const char kAudioLabel[] = "default_audio";
const char kVideoLabel[] = "default_video";
const char kStreamLabel[] = "default_stream";


PeerConnectionClient::PeerConnectionClient(WebRTCPlugin& observer, api::StreamingSession& session) : 
	observer_(observer), session_(session)
{
	peer_connection_factory_ = webrtc::CreatePeerConnectionFactory();
		
	// Setup ICE serves from the streaming API
	std::vector<api::IceServer> servers(observer.env().streaming().getIceServers());
	for (auto& item : servers) {
		TraceL << "Adding ICE server: " << item.url << endl;
		webrtc::PeerConnectionInterface::IceServer srv;
		srv.uri = item.url;
		srv.username = item.username;
		srv.password = item.credential;
		servers_.push_back(srv);
	}
	
	// Setup SDP constraints
	sdpConstraints_.SetAllowDtlsSctpDataChannels();

	// Setup media constraints
	mediaConstraints_.SetMandatoryReceiveAudio(false);
	mediaConstraints_.SetMandatoryReceiveVideo(false);
}


PeerConnectionClient::~PeerConnectionClient() 
{
	closeConnection();
}


bool PeerConnectionClient::initConnection(PacketSignal& source) 
{
	peer_connection_ = peer_connection_factory_->CreatePeerConnection(servers_, &sdpConstraints_, NULL, this);
	stream_ = peer_connection_factory_->CreateLocalMediaStream(kStreamLabel);
	
	// Add our custom video stream track
	talk_base::scoped_refptr<webrtc::VideoTrackInterface> video_track(
		peer_connection_factory_->CreateVideoTrack(kVideoLabel, 
			peer_connection_factory_->CreateVideoSource(new VideoStreamCapturer(source), NULL)));
	stream_->AddTrack(video_track);
	
	// TODO: Select device ID based on channel settings
	if (!session_.options().disableAudio) {
		DebugL << "Adding audio track" << endl;
		talk_base::scoped_refptr<webrtc::AudioTrackInterface> audio_track(
			peer_connection_factory_->CreateAudioTrack(kAudioLabel, 
				peer_connection_factory_->CreateAudioSource(NULL)));
		stream_->AddTrack(audio_track);
	}

	if (!peer_connection_->AddStream(stream_, &sdpConstraints_)) {
		ErrorL << "Adding stream to PeerConnection failed" << endl;
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
	peer_connection_->CreateOffer(this, &mediaConstraints_);
}


bool PeerConnectionClient::handleRemoteOffer(const std::string& type, const std::string sdp) 
{
	webrtc::SessionDescriptionInterface* desc(webrtc::CreateSessionDescription(type, sdp));
	if (!desc) return false;	
	peer_connection_->SetRemoteDescription(DummySetSessionDescriptionObserver::Create(), desc);
	peer_connection_->CreateAnswer(this, &mediaConstraints_);
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
	return true;
}


void PeerConnectionClient::OnError() 
{
	ErrorL << "PeerConnection error" << endl;
	assert(0);
}


void PeerConnectionClient::OnSignalingChange(webrtc::PeerConnectionInterface::SignalingState new_state)
{	
	DebugL << "PeerConnection signaling state change: " << new_state << endl;
	
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
	DebugL << "State change: " << state_changed << endl;
}


void PeerConnectionClient::OnAddStream(webrtc::MediaStreamInterface* stream) 
{
	DebugL << "Add stream" << endl;
}


void PeerConnectionClient::OnRemoveStream(webrtc::MediaStreamInterface* stream)
{
	DebugL << "Remove stream" << endl;
}


void PeerConnectionClient::OnIceCandidate(const webrtc::IceCandidateInterface* candidate) 
{	
	std::string sdp;
	if (!candidate->ToString(&sdp)) {
		ErrorL << "Failed to serialize candidate" << endl;
		assert(0);
		return;
	}	
	
	observer_.onLocalCandidate(session_, candidate->sdp_mid(), candidate->sdp_mline_index(), sdp);
}


void PeerConnectionClient::OnSuccess(webrtc::SessionDescriptionInterface* desc) 
{
	DebugL << "Set local description" << endl;
	peer_connection_->SetLocalDescription(
		DummySetSessionDescriptionObserver::Create(), desc);
		
	// Send an SDP offer to the peer
	std::string type(desc->type());
	std::string sdp;
	desc->ToString(&sdp);
	
	observer_.onLocalOffer(session_, type, sdp);
}


void PeerConnectionClient::OnFailure(const std::string& error) 
{
	ErrorL << "Session error: " << error << endl;
	// TODO: Need to removeRef?
	assert(0);
}


} } } // namespace scy::anio::spot