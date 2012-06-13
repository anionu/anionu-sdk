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


#ifndef ANIONU_SPOT_IModeManager_H
#define ANIONU_SPOT_IModeManager_H


#include "Sourcey/EventfulManager.h"
#include "Sourcey/Spot/IMode.h"
#include "Sourcey/Spot/IModule.h"

#include "Poco/Format.h"

#include <map>
#include <assert.h>


namespace Sourcey {
namespace Spot {


class IChannel;

class IModeManager: public EventfulManager<std::string, IMode>, public IModule
{
public:
	typedef EventfulManager<std::string, IMode>	Manager;
	typedef Manager::Map						Map;

public:
	IModeManager(IChannel& channel);
	virtual ~IModeManager();
	
	virtual const char* className() const { return "ModeManager"; }

protected:		
	mutable Poco::FastMutex _mutex;
	IChannel& _channel;
};


} } // namespace Sourcey::Spot


#endif // ANIONU_SPOT_IModeManager_H