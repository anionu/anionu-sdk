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

#ifndef Anionu_Spot_API_IEventManager_H
#define Anionu_Spot_API_IEventManager_H


#include "anionu/spot/api/api.h"


#ifdef Anionu_Spot_USING_CORE_API
#include "scy/base.h"
#include "scy/signal.h"
#include "anionu/event.h"
#endif


namespace scy {
namespace anio { 
namespace spot { 
namespace api { 
	
	
class IMessageProcessor
	/// The message processor provides an ABI agnostic
	/// interface for processing all incoming, outgoing
	/// and local messages passing through the Spot client.
{
public:		
	virtual bool onMessage(const char* /* message */) { return false; };
		// Handle incoming messages.
		// Return true if the message has been responded to.

	virtual bool onCommand(const char* /* command */) { return false; };
		// Handle incoming commands.
		// Return true if the command has been handled.

	virtual void onPresence(const char* /* presence */) {};
	virtual void onEvent(const char* /* event */) {};
};


// TODO: Base and core imlementations


} } } } // namespace scy::anio::spot::api


#endif // Anionu_Spot_API_IEventManager_H