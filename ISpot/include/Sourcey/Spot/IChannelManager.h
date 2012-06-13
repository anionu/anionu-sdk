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


#ifndef ANIONU_SPOT_IChannelManager_H
#define ANIONU_SPOT_IChannelManager_H


#include "Sourcey/EventfulManager.h"
#include "Sourcey/Spot/IChannel.h"
#include "Sourcey/Spot/IModule.h"


namespace Sourcey { 
namespace Spot {


class IChannelManager: public EventfulManager<std::string, IChannel>, public IModule
{
public:
	typedef EventfulManager<std::string, IChannel>	Manager;
	typedef Manager::Map							Map;
	
public:
	IChannelManager(IEnvironment& env);
	virtual ~IChannelManager();

	virtual IChannel* getChannel(const std::string& name);
		// Returns the IChannel instance or throws a
		// NotFoundException exception.
	
	bool addChannel(IChannel* channel);
	bool freeChannel(const std::string& name);	

    virtual IChannel* getDafaultChannel();
		// Returns the first channel in the list.
	
	virtual const char* className() const { return "ChannelManager"; }

protected:		
	mutable Poco::FastMutex _mutex;
};


} } // namespace Sourcey::Spot


#endif // ANIONU_SPOT_IChannelManager_H