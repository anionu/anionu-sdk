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


#ifndef Anionu_Spot_API_IFormProcessor_H
#define Anionu_Spot_API_IFormProcessor_H


#include "Anionu/Spot/API/Config.h"


namespace Scy {
namespace Symple {
	class Message;
	struct MessageDelegate;}
namespace Anionu {
namespace Spot { 
namespace API { 


class ISympleClientBase
{
public:		
	virtual bool sendMessage(const char* message, bool respond = false) = 0;
		/// Sends a Symple message to a remote peer.
		/// If the "respond" flag is true, the 'to' and 'from' fields
		/// will be swapped, so the message is returned to sender.
		/// An exception will be thrown if the Symple client is offline,
		/// or the session is invalid.

	virtual bool isOnline() const = 0;
		/// True when the Symple::Client is fully online.
};


// ---------------------------------------------------------------------
//
#ifdef ENFORCE_STRICT_ABI_COMPATABILITY

class ISympleClient: public ISympleClientBase
{
public:
	virtual void sendMessage(const Symple::Message& message, bool respond = false) = 0;

	virtual void attachListener(const Symple::MessageDelegate& delegate) = 0;
		/// Attach a delegate to listen for Symple presence, messages,
		/// and/or commands from remote peers.
		/// If the message is responded to inside this callback,
		/// a StopPropagation exception should be thrown to break
		/// out of the callback scope.

	virtual void detachListener(const Symple::MessageDelegate& delegate) = 0;
		/// Detach any previously attached message delegates.
};

#endif /// ENFORCE_STRICT_ABI_COMPATABILITY


} } } } // namespace Scy::Anionu::Spot::API


#endif /// Anionu_Spot_API_IFormProcessor_H