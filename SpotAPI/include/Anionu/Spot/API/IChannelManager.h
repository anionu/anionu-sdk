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


#ifndef Anionu_Spot_API_IChannelManager_H
#define Anionu_Spot_API_IChannelManager_H


#include "Anionu/Spot/API/IChannel.h"


namespace Scy {
namespace Anionu { 
namespace Spot { 
namespace API { 
	class IModeManager;
	

class IChannelManager
{
public:
	virtual API::IChannel* getChannel(const std::string& name) const = 0;
		// Returns the API::IChannel instance or
		// throws a NotFoundException exception.

    virtual API::IChannel* getDefaultChannel() const = 0;
		// Returns the first channel in the list.

	virtual IChannelList list() const = 0;
	virtual IModeManager& modes() = 0;
		
	Signal<API::IChannel&> ChannelAdded;
	Signal<API::IChannel&> ChannelRemoved;
		
protected:
	virtual ~IChannelManager() = 0 {};
};


} } } } // namespace Scy::Anionu::Spot::API


#endif // Anionu_Spot_API_IChannelManager_H