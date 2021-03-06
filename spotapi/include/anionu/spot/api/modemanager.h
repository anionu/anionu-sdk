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

#ifndef Anionu_Spot_API_ModeManager_H
#define Anionu_Spot_API_ModeManager_H


#include "anionu/spot/api/api.h"
#include "anionu/spot/api/imode.h"

#include "scy/signal.h"

#include <vector>
#include <string>


namespace scy { 
namespace anio {
namespace spot { 
namespace api { 
	

typedef std::vector<IMode*> IModeList;


class ModeManager
{
public:
	virtual void activate(const std::string& mode, const std::string& channel) = 0;
	virtual void deactivate(const std::string& mode, const std::string& channel) = 0;
	virtual bool isActive(const std::string& mode, const std::string& channel) = 0;

	virtual std::vector<std::string> types() const = 0;
			
	Signal<const std::string&> ModeRegistered;
	Signal<const std::string&> ModeUnregistered;
			
	Signal<api::IMode&> ModeActivated;
	Signal<api::IMode&> ModeDeactivated;
	Signal<api::IMode&> ModeError;

protected:
	virtual ~ModeManager() {};
};


} } } } // namespace scy::anio::spot::api


#endif // Anionu_Spot_API_ModeManager_H