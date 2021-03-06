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

#ifndef Anionu_Spot_API_StreamingSession_H
#define Anionu_Spot_API_StreamingSession_H

//#ifndef Anionu_Spot_USING_CORE_API
//#error "StreamingSession requires the Core API"
//#endif


#include "scy/base.h"
#include "scy/stateful.h"
#include "scy/packetstream.h"
#include "scy/media/iencoder.h"
#include "scy/net/address.h"
#include "scy/json/json.h"


#include <sstream>
#include <map>


namespace scy { 
namespace anio {
namespace spot { 
namespace api { 


struct StreamingOptions: public av::EncoderOptions
{	
	std::string peer;		// Peer ID of the initiating party
	std::string channel;	// The channel we are streaming from
	std::string token;		// The session's unique identification token
	std::string transport;	// The transport protocol [UDP, TCP, TLS]
	std::string protocol;	// The packet protocol [Raw, HTTP, RTP/AVP]
	std::string encoding;	// The packet content encoding method [None, Base64, ...]
	std::string framing;	// The packet content framing method [None, multipart, ...]
	int timeout;			// The lifetime after disconnection timeout value
	
	bool disableLAN;		// Disable LAN candidates
	bool disableHost;		// Disable Host candidates
	bool disableRelay;		// Disable Relay candidates
	bool disableAudio;		// Disable the audio stream
	bool enableWebRTC;	    // The client supports WebRTC
	                        // Note that if WebRTC is used then transport, protocol, 
	                        // encoding and framing values will have no effect.	
	bool supressEvents;     // Optionally supress events for this session.
	                        // This is handy if you are creating an separate audio
	                        // stream and want to avoid event spam.	
	bool isLocalPeer;		// True if the peer is on the same LAN

	bool valid() const 
	{
		return !peer.empty()
			&& !channel.empty()
			&& !token.empty()
			&& (transport == "TCP" 
			||  transport == "SSL" 
			||  transport == "UDP") 
			&& (oformat.video.enabled 
			||  oformat.audio.enabled);
	}
	
	StreamingOptions(
		const std::string& peer = "",
		const std::string& channel = "",
		const std::string& transport = "TCP",
		const std::string& protocol = "Raw",
		const std::string& encoding = "None",
		const std::string& framing = "None",
		int timeout = 10000) : 
			peer(peer),
			channel(channel),
			transport(transport),
			protocol(protocol),
			encoding(encoding),
			framing(framing),
			timeout(timeout),
			disableLAN(false),
			disableHost(false),
			disableRelay(false),
			disableAudio(false),
			enableWebRTC(false),
			isLocalPeer(false) {}
};


//
// Candidate
//

	
typedef json::Value Candidate;
typedef std::vector<Candidate> CandidateList;
	/// Candidates are an arbitrary JSON object.
	/// Internally we use the following types: lan, host and relay.
	/// ICE type SDP candidates are also supported via the WebRTC plugin.


inline Candidate makeDefaultCandidate(const std::string& type, 
                                      const net::Address& address, 
							          const std::string& uri = "")
{
	Candidate c;
	c["type"] = type;
	c["address"] = address.toString();
	c["uri"] = uri;
	return c;
}


inline Candidate makeWebRtcCandidate(const std::string& mid, 
									 int mlineindex, 
									 const std::string sdp)
{
	Candidate c;
	c["type"] = "webrtc";
	c["sdpMid"] = mid;
	c["sdpMLineIndex"] = mlineindex;
	c["candidate"] = sdp;
	return c;
}


//
// Streaming State
//


struct StreamingState: public State 
{
	enum Type 
	{
		None = 0,
		Initializing,
		Ready,
		Active,
		Error,
		Terminated
	};

	std::string str(unsigned int id) const 
	{ 
		switch(id) {
		case None:				return "None";
		case Initializing:		return "Initializing";
		case Ready:				return "Ready";
		case Active:			return "Active";
		case Error:				return "Error";
		case Terminated:		return "Terminated";
		default:				assert(false);
		}
		return "undefined"; 
	}
};


//
// Streaming Session
//


class StreamingSession: public Stateful<api::StreamingState>
{
public:
	virtual void addCandidate(const std::string& type, 
                              const net::Address& address, 
							  const std::string& uri = "") = 0;
	virtual void addCandidate(const Candidate& cand) = 0;
		// Adds a streaming candidate.

	virtual bool removeCandidate(const Candidate& cand) = 0;
		// Removes a streaming candidate.
		// The given candidate fields must match the 
		// stored candidate fields exactly.
	
	virtual void addRef() = 0;
		// Increment in internal reference count.
		// When the count becomes positive the state will transition
		// to Active, and the connection timeout will be cancelled.

	virtual void removeRef() = 0;
		// Decrement in internal reference count.
		// When the count becomes zero the state will transition
		// to Ready, and the connection timeout will be started.
		// When the timeout expires the session will be destroyed.

	virtual int refCount() const = 0;
		// Returns the current reference count.

	virtual std::string token() const = 0;
		// Returns the unique streaming session token.	

	virtual StreamingOptions& options() = 0;
		// Returns a reference to the streaming options.
	
	virtual PacketStream& captureStream() = 0;
		// Returns a reference to the source capture packet stream.
		// This stream primarily contains audio and video capture devices.
		// This stream can also contain filters which process and emit the 
		// same packet type as the device captures. This enables us to preform
		// arbritrary processing of captured raw media packets before they are  
		// sent to the encode stream for encoding.
	
	virtual PacketStream& encodeStream() = 0;
		// Returns a reference to the encode packet stream.
		// The encode stream processes data from the capture stream
		// and emits the result to available connections.

	virtual PacketStreamVec connections() const = 0;
		// Returns a list of connection packet streams.

	virtual CandidateList candidates() const = 0;
		// Returns a list of streaming candidates in JSON format.

protected:
	virtual ~StreamingSession() {};
};


} } } } // namespace scy::anio::spot::api


#endif // Anionu_Spot_API_StreamingSession_H