#ifndef SCY_PeerConnectionClient_H
#define SCY_PeerConnectionClient_H


#include "anionu/spot/api/environment.h"
#include "anionu/spot/api/streamingsession.h"
//#include "scy/packetstream.h"

#include "talk/app/webrtc/peerconnectioninterface.h"
#include "talk/app/webrtc/jsep.h"
#include "talk/app/webrtc/datachannelinterface.h"
#include "talk/media/devices/devicemanager.h"
#include "talk/app/webrtc/videosourceinterface.h"
#include "talk/app/webrtc/test/fakeconstraints.h"
#include "talk/base/logging.h"
#include "assert.h"


namespace scy { 
namespace anio { 
namespace spot {
	

class WebRTCPlugin;


class PeerConnectionClient : 
	public webrtc::PeerConnectionObserver,
	public webrtc::CreateSessionDescriptionObserver {
public:
	PeerConnectionClient(WebRTCPlugin& observer, api::StreamingSession& session); //
	virtual ~PeerConnectionClient();
	
	bool initConnection(PacketSignal& source);
	bool closeConnection();

	void createOffer();

	bool handleRemoteOffer(const std::string& type, const std::string sdp);
	bool handleRemoteAnswer(const std::string& type, const std::string sdp);
	bool handleRemoteCandidate(const std::string& mid, int mlineindex, const std::string sdp);
	
	void onVideoCaptureStart();
	void onVideoCaptureStop();
	
	api::StreamingSession& session() { return session_; };

protected:

	// inherited from PeerConnectionObserver
	virtual void OnError();
	virtual void OnSignalingChange(webrtc::PeerConnectionInterface::SignalingState new_state);
	virtual void OnStateChange(webrtc::PeerConnectionObserver::StateType state_changed);
	virtual void OnAddStream(webrtc::MediaStreamInterface* stream);
	virtual void OnRemoveStream(webrtc::MediaStreamInterface* stream);
	virtual void OnIceCandidate(const webrtc::IceCandidateInterface* candidate);

	// inherited from CreateSessionDescriptionObserver
	virtual void OnSuccess(webrtc::SessionDescriptionInterface* desc);
	virtual void OnFailure(const std::string &error);
	virtual int AddRef() { return 1; }
	virtual int Release() { return 0; }

private:
	WebRTCPlugin& observer_;
	api::StreamingSession& session_;
	webrtc::PeerConnectionInterface::IceServers servers_;
	webrtc::PeerConnectionInterface::IceServer server_;
	webrtc::FakeConstraints constraints_;
	talk_base::scoped_refptr<webrtc::PeerConnectionFactoryInterface> peer_connection_factory_;
	talk_base::scoped_refptr<webrtc::PeerConnectionInterface> peer_connection_;
	talk_base::scoped_refptr<webrtc::MediaStreamInterface> stream_;
};


class DummySetSessionDescriptionObserver : 
	public webrtc::SetSessionDescriptionObserver {
public:
	static DummySetSessionDescriptionObserver* Create() {
		return new talk_base::RefCountedObject<DummySetSessionDescriptionObserver>();
	}
	virtual void OnSuccess() {}
	virtual void OnFailure(const std::string& error) { assert(0); }

protected:
	DummySetSessionDescriptionObserver() {}
	~DummySetSessionDescriptionObserver() {}
};


} } } // namespace scy::anio::spot


#endif



	
	//cricket::VideoCapturer* OpenVideoCaptureDevice();
	//const webrtc::SessionDescriptionInterface* local_description() const {
	//		return peer_connection_->local_description();
	//}