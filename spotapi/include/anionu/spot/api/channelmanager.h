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


#ifndef Anionu_Spot_API_ChannelManager_H
#define Anionu_Spot_API_ChannelManager_H


#include "anionu/spot/api/channel.h"


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
		// index position, or nullptr.

	virtual ChannelVec list() const = 0;
		// Returns the list of channels.
		
	Signal<api::Channel&> ChannelAdded;
	Signal<api::Channel&> ChannelRemoved;
		
protected:
	virtual ~ChannelManager() {};
};


} } } } // namespace scy::anio::spot::api


#endif // Anionu_Spot_API_ChannelManager_H