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


#ifndef ANIONU_SPOT_IStreamingSession_H
#define ANIONU_SPOT_IStreamingSession_H


#include "Sourcey/Base.h"
#include "Sourcey/Stateful.h"
//#include "Sourcey/Util/Timer.h"
#include "Sourcey/PacketStream.h"
#include "Sourcey/Spot/IModule.h"
#include "Sourcey/Net/TCPPacketStreamConnection.h"
#include "Sourcey/Net/Address.h"
#include "Sourcey/JSON/ISerializable.h"
#include "Sourcey/Symple/Peer.h"
#include "Sourcey/Symple/Command.h"
#include "Sourcey/Media/IEncoder.h"

#include <sstream>
#include <map>


namespace Sourcey {
namespace Spot {


class IStreamingManager;
class IChannel;


struct StreamingOptions: public Media::EncoderOptions, public JSON::ISerializable
{	
	std::string peer;		// Peer ID of the initiating party
	std::string channel;	// The canllel we are streaming on
	std::string token;		// The session's unique identification token
	std::string transport;	// The transport protocol  [UDP, TCP, TLS]
	std::string protocol;	// The packet protocol [Raw, HTTP, RTP/AVP]
	std::string encoding;	// The packet encoding method [None, Base64]
	int timeout;			// The lifetime after disconnection timeout value
	StreamingOptions(
		const std::string& peer = "",
		const std::string& channel = "",
		const std::string& transport = "TCP",
		const std::string& protocol = "Raw",
		const std::string& encoding = "None",
		const Media::Format& iformat = Media::Format(),
		const Media::Format& oformat = Media::Format(),
		int timeout = 20000);

	bool valid();

	void serialize(JSON::Value& root);
	void deserialize(JSON::Value& root);
};


typedef PacketStream ConnectionStream;
typedef Net::TCPPacketStreamConnection TCPStreamingConnection;
typedef std::vector<ConnectionStream*> ConnectionStreamList;


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
	
	bool operator ==(const Candidate& r) const {
		return type == r.type 
			&& address == r.address 
			&& uri == r.uri;
	}
};


typedef std::vector<Candidate> CandidateList;


class IStreamingSession: public StatefulSignal<StreamingState>, public IModule
{
public:
	IStreamingSession(IEnvironment& env, 
					  IStreamingManager& service, 
					  const StreamingOptions& options);

	virtual ~IStreamingSession();
		/// The media session is terminated(), not deleted.

	virtual void initialize();
		/// Session initialization logic should be implemented here.
		/// Derived methods should always call the base method first.
		/// An exception is thrown in case of failure.

	virtual void terminate();
		/// Session termination logic should be implemented here.
		/// Derived methods should always call the base method.
		/// NOTE: Sessions are never deleted, only terminated.

	virtual void addCandidate(const std::string& type, 
							  const Net::Address& address, 
							  const std::string& uri = "");
		/// Adds a streaming candidate.

	virtual bool removeCandidate(const std::string& type, 
							     const Net::Address& address);
		/// Removes a streaming candidate.
		
	virtual ConnectionStream* createConnection() = 0;
		/// Creates a connection stream for this media session.

	virtual TCPStreamingConnection* createTCPConnection(const Poco::Net::StreamSocket& socket,
														const std::string& request) = 0;
		/// Creates a TCP socket connection for this media session.

	virtual std::string token() const;
	virtual IStreamingManager& service();
	virtual PacketStream& stream();
	virtual StreamingOptions& options();
	virtual CandidateList candidates() const;
	virtual ConnectionStreamList connections() const;

	virtual void setError(const std::string& reason);
		/// Sets the session as erroneous resulting in termination.
	
	virtual bool isActive() const;
	virtual bool isError() const;
	virtual bool isTerminating() const;

	Signal<const CandidateList&> CandidatesCollected;

	virtual const char* className() const { return "StreamingSession"; }

protected:
	virtual void notifyCandidates();
		/// Notifies the outside application that candidates have
		/// been collected and should be sent to the peer.

	virtual bool setState(unsigned int id, const std::string& message = "");
		/// Sub-classes must use this method for setting state
		/// because the sender instance must match the base.

	mutable Poco::FastMutex _mutex;
	
protected:
	IStreamingManager&		_service;
	StreamingOptions			_options;
	PacketStream			_stream;
	CandidateList			_candidates;
	ConnectionStreamList	_connections;

	friend class IStreamingManager;
};


} } // namespace Sourcey::Spot


#endif // ANIONU_SPOT_IStreamingSession_H

/*, 
					  IChannel& channel, 
					  Symple::Peer& peer = Symple::Peer(),
					  const Symple::Command& command = Symple::Command()*/

	//virtual IChannel& channel();
	//virtual Symple::Peer& peer();
	//virtual Symple::Command& command();
	//IChannel&				_channel; 
	//Symple::Peer&			_peer;
	//Symple::Command		_command;


	//std::string id;
	//std::string channel;
		//const std::string& id = "",				/// The session ID
		//const std::string& channel = "",
/*
*/
	//virtual JSON::Value& candidates();
	//virtual void startTimer(int timeout = 0);
	//virtual void stopTimer();
	//virtual void onTimeout(TimerCallback<IStreamingSession>& timer);

	//CandidateList			_candidates;
		/// Returns true if the stream is ready, or false if the, int timeout = 20000
		/// stream is still in the negotiating phase in which case
		/// the stream will manage it's own .
	//StreamingOptions	_options;