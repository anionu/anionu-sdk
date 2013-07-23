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


#ifndef Anionu_Spot_API_IModeManager_H
#define Anionu_Spot_API_IModeManager_H


#include "Anionu/Spot/API/Config.h"
#include "Anionu/Spot/API/IMode.h"

#include "Sourcey/Signal.h"

#include <vector>
#include <string>


namespace scy { 
namespace anio {
namespace spot { 
namespace api { 
	

typedef std::vector<IMode*> IModeList;


class IModeManager
{
public:
	virtual void activate(const std::string& mode, const std::string& channel) = 0;
	virtual void deactivate(const std::string& mode, const std::string& channel) = 0;
	virtual bool isActive(const std::string& mode, const std::string& channel) = 0;

	virtual StringVec types() const = 0;
			
	Signal<const std::string&> ModeRegistered;
	Signal<const std::string&> ModeUnregistered;
			
	Signal<api::IMode&> ModeActivated;
	Signal<api::IMode&> ModeDeactivated;
	Signal<api::IMode&> ModeError;

protected:
	virtual ~IModeManager() = 0 {};
};


} } } } // namespace scy::anio::spot::api


#endif // Anionu_Spot_API_IModeManager_H