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


#ifndef Anionu_Spot_API_IEventManager_H
#define Anionu_Spot_API_IEventManager_H


#include "Anionu/Spot/API/Config.h"


#ifdef Anionu_Spot_ENABLE_ABI_COMPATABILITY
#include "Sourcey/Base.h"
#include "Sourcey/Signal.h"
#include "Anionu/Event.h"
#endif


namespace Scy {
namespace Anionu { 
namespace Spot { 
namespace API { 
	
	
class IMessageProcessor
	/// The message processor provides an ABI agnostic
	/// interface for procession all incoming, outgoing
	/// and local messages passing through the Spot client.
{
public:		
	virtual bool onMessage(const char* message) { return false; };
		/// Handle incoming messages.
		/// Return true if the message has been responded to.

	virtual bool onCommand(const char* command) { return false; };
		/// Handle incoming commands.
		/// Return true if the command has been handled.

	virtual void onPresence(const char* presence) {};
	virtual void onEvent(const char* event) {};
};


} } } } // namespace Scy::Anionu::Spot::API


#endif // Anionu_Spot_API_IEventManager_H