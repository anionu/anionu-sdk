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


#ifndef Anionu_Spot_API_IModeManager_H
#define Anionu_Spot_API_IModeManager_H


#include "Anionu/Spot/API/Config.h"
#include "Anionu/Spot/API/IMode.h"

#include "Sourcey/Signal.h"

#include <vector>
#include <string>


namespace Scy { 
namespace Anionu {
namespace Spot { 
namespace API { 
	

typedef std::vector<IMode*> IModeList;


class IModeManager
{
public:
	virtual void activate(const std::string& name, const std::string& channel) = 0;
	virtual void deactivate(const std::string& name, const std::string& channel) = 0;

	virtual IModeList list() const = 0;
			
	Signal<const std::string&> ModeRegistered;
	Signal<const std::string&> ModeUnregistered;

	Signal<API::IMode&> ModeActivated;
	Signal<API::IMode&> ModeDeactivated;
	Signal<API::IMode&> ModeError;

protected:
	virtual ~IModeManager() = 0 {};
};


} } } } // namespace Scy::Anionu::Spot::API


#endif // Anionu_Spot_API_IModeManager_H