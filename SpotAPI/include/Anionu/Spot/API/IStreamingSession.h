//
// LibSourcey
// Copyright (C) 2005, Sourcey <http://sourcey.com>
//
// LibSourcey is is distributed under a dual license that allows free, 
// open source use and closed source use under a standard commercial
// license.
//
// Non-Commercial Use:
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
// Commercial Use:
// Please contact mail@sourcey.com
//


#ifndef ANIONU_SPOT_API_IStreamingSession_H
#define ANIONU_SPOT_API_IStreamingSession_H


#include "Sourcey/Base.h"
#include "Sourcey/Stateful.h"
#include "Sourcey/PacketStream.h"
#include "Sourcey/Media/IEncoder.h"
#include "Sourcey/Net/Address.h"


#include <sstream>
#include <map>


namespace Scy { 
namespace Anionu {
namespace Spot { 
namespace API { 


struct StreamingOptions: public Media::EncoderOptions
{	
	std::string peer;		// Peer ID of the initiating party
	std::string channel;	// The channel we are streaming from
	std::string token;		// The session's unique identification token
	std::string transport;	// The transport protocol [UDP, TCP, TLS]
	std::string protocol;	// The packet protocol [Raw, HTTP, RTP/AVP]
	std::string encoding;	// The packet encoding method [None, Base64, ...]
	std::string mime;		// The HTTP packet content type [None, image/jpeg, multipart/x-mixed-replace, ...]
	int timeout;			// The lifetime after disconnection timeout value

	bool disableLAN;		// Disable LAN candidates
	bool disableHost;		// Disable Host candidates
	bool disableRelay;		// Disable Relay candidates
	
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
		const std::string& mime = "None",
		int timeout = 20000) : 
			peer(peer),
			channel(channel),
			transport(transport),
			protocol(protocol),
			encoding(encoding),
			mime(mime),
			timeout(timeout),
			disableLAN(false),
			disableHost(false),
			disableRelay(false) {}
};


// ---------------------------------------------------------------------
//
struct Candidate
{
	std::string type; 
		/// Candidate type is implementation defined.
		/// Currently used are lan, host and relay.

	Net::Address address;
	std::string uri; 

	Candidate(const std::string& type = "", 
			  const Net::Address& address = Net::Address(), 
			  const std::string& uri = "") :
		type(type), address(address), uri(uri) {};	
	
	bool operator == (const Candidate& r) const {
		return type == r.type 
			&& address == r.address 
			&& uri == r.uri;
	}
};

	
typedef PacketStream ConnectionStream;
typedef std::vector<ConnectionStream*> ConnectionStreamList;
typedef std::vector<Candidate> CandidateList;


// ---------------------------------------------------------------------
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
		Terminating
	};

	std::string str(unsigned int id) const 
	{ 
		switch(id) {
		case None:				return "None";
		case Initializing:		return "Initializing";
		case Ready:				return "Ready";
		case Active:			return "Active";
		case Error:				return "Error";
		case Terminating:		return "Terminating";
		default:				assert(false);
		}
		return "undefined"; 
	}
};


class IStreamingSession: public StatefulSignal<API::StreamingState>
{
public:
	virtual void addCandidate(const std::string& type, 
							  const Net::Address& address, 
							  const std::string& uri = "") = 0;
		/// Adds a streaming candidate.

	virtual bool removeCandidate(const std::string& type, 
							     const Net::Address& address) = 0;
		/// Removes a streaming candidate.

	virtual std::string token() const = 0;
	virtual StreamingOptions& options() = 0;
	virtual PacketStream& stream() = 0;
	virtual ConnectionStreamList connections() const = 0;
	virtual CandidateList candidates() const = 0;

protected:
	virtual ~IStreamingSession() = 0 {};
		/// The session is terminated(), never deleted.
};


} } } } // namespace Scy::Anionu::Spot::API


#endif // ANIONU_SPOT_API_IStreamingSession_H