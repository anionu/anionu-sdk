//
// LibSourcey
// Copyright (C) 2005, Sourcey <http://sourcey.com>
//
// LibSourcey is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// LibSourcey is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.
//


#ifndef Anionu_Spot_API_ChannelManager_H
#define Anionu_Spot_API_ChannelManager_H


#include "Anionu/Spot/API/Channel.h"


namespace scy {
namespace anio { 
namespace spot { 
namespace api {
	

class ChannelManager
{
public:
	virtual api::Channel* getChannel(const std::string& name) const = 0;
		// Returns the api::Channel instance or
		// throws a NotFoundException exception.

    virtual api::Channel* getDefaultChannel() const = 0;
		// Returns the first channel in the list.

	virtual Channel* at(int index) const = 0;
		// Returns the channel at the given 0 based 
		// index position, or NULL.

	virtual ChannelList list() const = 0;
		// Returns the list of channels.
		
	Signal<api::Channel&> ChannelAdded;
	Signal<api::Channel&> ChannelRemoved;
		
protected:
	virtual ~ChannelManager() = 0;
};


} } } } // namespace scy::anio::spot::api


#endif // Anionu_Spot_API_ChannelManager_H