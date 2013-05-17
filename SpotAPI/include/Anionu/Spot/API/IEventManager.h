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


#ifdef ENFORCE_STRICT_ABI_COMPATABILITY
#include "Sourcey/Base.h"
#include "Sourcey/Signal.h"
#endif


namespace Scy {
namespace Anionu { 
	struct Event;
namespace Spot { 
namespace API { 
	
	

// ---------------------------------------------------------------------
//
class IEventManagerBase
	/// ABI agnostic API
{
public:
	virtual bool createEvent(const char* name, const char* message, int severity = 0) = 0;
		/// Create a surveillance event using the Anionu REST API.
		/// The event will be propagated across the Anionu system.
		
protected:
	virtual ~IEventManagerBase() = 0 {};
};


// ---------------------------------------------------------------------
//
#ifdef ENFORCE_STRICT_ABI_COMPATABILITY

class IEventManager: public IEventManagerBase
{
public:	
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
			
	Signal<const Anionu::Event&> Event;
		/// Signals when a local surveillance event is triggered.
		/// Called internally by createEvent() to dispatch
		/// the event to the internal application.
		
protected:
	virtual ~IEventManager() = 0 {};
};


#endif // ENFORCE_STRICT_ABI_COMPATABILITY


} } } } // namespace Scy::Anionu::Spot::API


#endif // Anionu_Spot_API_IEventManager_H