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


#ifndef ANIONU_SPOT_API_IEnvironment_H
#define ANIONU_SPOT_API_IEnvironment_H


#include "Sourcey/Base.h"
#include "Sourcey/Signal.h"


namespace Scy {
namespace Media {
	class FormatRegistry;}
namespace Symple {
	class Message;
	struct MessageDelegate;}
namespace Anionu {		
	struct Event;
namespace Spot { 
namespace API { 

	// Forward declare API interfaces
	class ISession;
	class IConfiguration;
	class ISynchronizer;
	class IChannelManager;
	class ModeRegistry;
	class IMediaManager;
	class IStreamingManager;
	class IStreamingSession;


static const char* SDKVersion = "0.5.1";
	/// Current Spot SDK version number.
	/// This version is be bumped whenever the API or
	/// dependencies change, breaking binary compatability.
	/// Spot plugins must be built with the same compiler,
	/// dependencies, and SDK version as the target Spot client.
	/// See Compatability.txt for more information.


static const char* DateFormat = "%Y-%m-%d %H:%M:%S %Z";


class IEnvironment
{
public:
	virtual API::IConfiguration& appConfig() = 0;
	virtual API::IConfiguration& config() = 0;
	virtual API::ISession& session() = 0;
	virtual API::IMediaManager& media() = 0;
	virtual API::IChannelManager& channels() = 0;
	virtual API::IStreamingManager& streaming() = 0;
	virtual API::ISynchronizer& synchronizer() = 0;
	virtual API::ModeRegistry& modes() = 0;
	virtual Logger& logger() = 0;

	virtual std::string version() const = 0;
		/// Returns the installed Spot package version.
	
	virtual void createEvent(const Anionu::Event& event) = 0;
		/// Creates a surveillance event via the Anionu REST API.
		/// Once created the event will be broadcasted over the 
		/// network to online peers.
		/// This method should be used for important events which
		/// need to be stored in the database.

	virtual bool broadcastEvent(const Anionu::Event& event) = 0;
		/// Broadcasts a surveillance event over the network to 
		/// online peers.
		/// This method should be used for messages or trivial
		/// events which don't need to be stored in the database.
		/// Returns false if offline, true otherwise.
	
	virtual void sendMessage(const Symple::Message& message, bool respond = false) = 0;
		/// Sends a Symple message to a remote peer.
		/// If the "respond" flag is true, the 'to' and 'from' fields
		/// will be swapped, so the message is returned to sender.
		/// An exception will be thrown if the ISession is invalid,
		/// or the Symple client is offline.
	
	virtual void attachMessageListener(const Symple::MessageDelegate& delegate) = 0;
		/// Attach a delegate to listen for Symple presence, messages,
		/// and/or commands from remote peers.
		/// If the message is responded to inside this callback,
		/// a StopPropagation exception should be thrown to break
		/// out of the callback scope.

	virtual void detachMessageListener(const Symple::MessageDelegate& delegate) = 0;
		/// Detach any previously attached message delegates.
		
protected:
	virtual ~IEnvironment() = 0 {};
};


} } } } // namespace Anionu::Spot::API


#endif // ANIONU_SPOT_API_IEnvironment_H