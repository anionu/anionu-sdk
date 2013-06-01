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


#ifndef Anionu_Spot_API_IClient_H
#define Anionu_Spot_API_IClient_H


#include "Anionu/Spot/API/Config.h"
#include <string>

#ifdef Anionu_Spot_ENABLE_ABI_COMPATABILITY
#include "Sourcey/PacketSignal.h"
#include "Sourcey/Stateful.h"
#include "Anionu/Event.h"
#endif


namespace Scy {
namespace Symple {
	class Message;
	struct MessageDelegate;}
namespace Anionu {
namespace Spot { 
namespace API { 


class IClientBase
{
public:		
	virtual bool sendMessage(const char* message, bool respond = false) = 0;
		/// Send a Symple::Message to a peer, or over the network.
		/// If the "respond" flag is true, the 'to' and 'from' fields
		/// will be swapped, so the message is returned to sender.
		/// An exception will be thrown if the Symple client is offline,
		/// or the session is invalid.

	virtual bool createEvent(const char* name, const char* message, int severity = 0) = 0;

	virtual bool broadcastEvent(const char* name, const char* message, int severity = 0) = 0;
		
	virtual bool isActive() const = 0;
		/// True when the current user session is authenticated.

	virtual bool isOnline() const = 0;
		/// True when the current user session is authenticated,
		/// and the Symple client is online.
	
	virtual const char* cName() const = 0;
		/// Spot client name on the network.

	virtual const char* cUsername() const = 0;
		/// Currently authenticated username.
};


// ---------------------------------------------------------------------
//
#ifdef Anionu_Spot_ENABLE_ABI_COMPATABILITY


struct ClientState: public State 
{
	enum Type 
	{
		None = 0,
		Authenticating,
		ActiveOffline,
		ActiveConnecting,
		ActiveOnline,
		ActiveFailed,
		Failed,
	};

	std::string str(unsigned int id) const 
	{ 
		switch(id) {
			case ClientState::None:				return "None";
			case ClientState::Authenticating:	return "Authenticating";
			case ClientState::ActiveOffline:	return "Active (Offline)";
			case ClientState::ActiveConnecting:	return "Active (Connecting)";
			case ClientState::ActiveOnline:		return "Active (Online)";
			case ClientState::ActiveFailed:		return "Active (Failed)";
			case ClientState::Failed:			return "Failed";
			default: assert(false);
		}
		return "undefined"; 
	}
};


class IClient: public IClientBase, public StatefulSignal<ClientState>
{
public:
	virtual void sendMessage(Symple::Message& message, bool respond = false) = 0;
		/// Send a Symple::Message to a peer, or over the network.
		/// An exception will be thrown on error, or if the Symple
		/// client is offline.
	
	virtual void createEvent(const Anionu::Event& event) = 0;
	virtual void createEvent(
			const std::string& name,
			const std::string& message,
			Anionu::Event::Severity severity) = 0;
		/// Creates a surveillance event via the Anionu REST API.
		/// Once created the event will be broadcasted over the network 
		/// to online peers.
		/// This method should be used for important events which need
		/// to be stored in the database.
		/// An exception will be thrown on error, or if the REST API is
		/// unavailable.
	
	virtual void broadcastEvent(const Anionu::Event& event) = 0;
	virtual void broadcastEvent(
		    const std::string& name, 
		    const std::string& message,
			Anionu::Event::Severity severity) = 0;
		/// Broadcasts a surveillance event over the network to online peers.
		/// This method should be used for notifications or trivial events
		/// which don't need to be stored in the database.
		/// An exception will be thrown on error, or if the Symple
		/// client is offline.

	virtual std::string name() const = 0;
	virtual std::string username() const = 0;
	
	PacketSignal MessageReceived;
		/// Signal for incoming Symple messages, including presence and
		/// commands from remote peers.
		/// 
		/// If the message is responded to by the callback handler, a
		/// StopPropagation exception should be thrown to break out of
		/// the callback scope.
		///
		/// See plugin implementations for usage of the packetDelegate type
		/// for polymorphic message class filtering.

	Signal<const Anionu::Event&> EventTriggered;
		/// Signals when a local surveillance event is triggered.
		/// Called internally by create/broadcastEvent() functions
		/// to dispatch the event to the internal modules.
		
	NullSignal SessionStart;
		/// Signals when the new session is created.

	NullSignal SessionEnd;
		/// Signals when the session ends.
};

#endif /// Anionu_Spot_ENABLE_ABI_COMPATABILITY


} } } } // namespace Scy::Anionu::Spot::API


#endif /// Anionu_Spot_API_IClient_H





	
/*

	virtual void attachListener(const Symple::MessageDelegate& delegate) = 0;
		/// Attach a delegate to listen for Symple presence, messages,
		/// and/or commands from remote peers.
		/// If the message is responded to inside this callback,
		/// a StopPropagation exception should be thrown to break
		/// out of the callback scope.

	virtual void detachListener(const Symple::MessageDelegate& delegate) = 0;
		/// Detach any previously attached message delegates.
enum ClientState 
{
	None = 0,
	Authenticating,
	ActiveOffline,
	ActiveConnecting,
	ActiveOnline,
	ActiveFailed,
	Failed
};
inline std::string ClientStateToString(ClientState state) 
{
	switch(state) {
		case ClientState::None:				return "None";
		case ClientState::Authenticating:	return "Authenticating";
		case ClientState::Failed:			return "Failed";
		case ClientState::ActiveOffline:	return "Active (Offline)";
		case ClientState::ActiveConnecting:	return "Active (Connecting)";
		case ClientState::ActiveOnline:		return "Active (Online)";
		case ClientState::ActiveFailed:		return "Active (Failed)";
		default: assert(false);
		return "Unknown"; 
	}
}
*/