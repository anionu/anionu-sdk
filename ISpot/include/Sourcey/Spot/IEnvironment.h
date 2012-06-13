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


#ifndef ANIONU_SPOT_IEnvironment_H
#define ANIONU_SPOT_IEnvironment_H


#include "Sourcey/Signal.h"
#include "Sourcey/Spot/IModule.h"


namespace Sourcey {
	

namespace Media {
class FormatRegistry;
}

namespace Anionu {
struct Event;
}

namespace Spot {


class ISession;
class IConfiguration;
class ISynchronizer;
class IChannelManager;
class IModeRegistry;
class IStreamingManager;
class IStreamingSession;
class IMediaManager;
class IDiagnosticManager;


class IEnvironment 
{
public:
	IEnvironment() {};	
	virtual ~IEnvironment() {};
	
	virtual IConfiguration& appConfig() /*const*/ = 0;
	virtual IConfiguration& config() /*const*/ = 0;
	virtual ISession& session() /*const*/ = 0;
	virtual ISynchronizer& synchronizer() /*const*/ = 0;
	virtual IChannelManager& channels() /*const*/ = 0;
	virtual IModeRegistry& modes() /*const*/ = 0;
	virtual IStreamingManager& streaming() /*const*/ = 0;
	virtual IMediaManager& media() /*const*/ = 0;
	virtual IDiagnosticManager& diagnostics() /*const*/ = 0;
	virtual Logger& logger() /*const*/ = 0;

	virtual std::string version() const = 0;
		/// Returns the version number string of the library.
	
	virtual void notifyEvent(const Anionu::Event& event) = 0;
		/// Notifies the system about an event.
		/// If the event severity is higher than the minimum 
		/// severity defined by the user then the event will
		/// be broadcast over the remote network and saved on
		/// the user account. If the event is not broadcast
		/// then event notifications will not be triggered.
	
	Signal<const Anionu::Event&> OnEvent;
		/// Signals an event. Called internally by notifyEvent()
};


} } // namespace Sourcey::Spot


#endif // ANIONU_SPOT_IEnvironment_H