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


#ifndef ANIONU_SPOT_IConfiguration_H
#define ANIONU_SPOT_IConfiguration_H


#include "Sourcey/Signal.h"
#include "Sourcey/JSON/Configuration.h"
#include "Sourcey/Spot/IModule.h"


namespace Sourcey { 
namespace Spot {


class IConfiguration: public JSON::Configuration, public IModule
{
public:
	IConfiguration(IEnvironment& env);
	virtual ~IConfiguration();

	Signal2<const std::string&, const std::string&> ConfigurationChanged;
		/// The Key and Value of the changed configuration property.

	virtual const char* className() const { return "Configuration"; }

protected:
	virtual void setRaw(const std::string& key, const std::string& value);
		/// The base method gets called after the config value is
		/// updated to trigger the ConfigurationChanged callback.
};


} } // namespace Sourcey::Spot


#endif