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


#ifndef ANIONU_SPOT_IModule_H
#define ANIONU_SPOT_IModule_H


#include "Sourcey/Logger.h"


namespace Sourcey { 
namespace Spot {


class IEnvironment;


class IModule: public ILoggable
	/// This class is the unified interface from which all
	/// Spot classes inherit from. It ensures all classes
	/// have an IEnvironment reference and Logger access.
{
public:
	IModule(IEnvironment& env);
	virtual ~IModule();

	//virtual void initialize() = 0;
	//virtual void uninitialize() = 0;

	virtual void onSessionStart() {};
	virtual void onSessionEnd() {};

	IEnvironment& env();

	LogStream log(const char* level = "debug") const;
	
protected:
	mutable Poco::FastMutex _mutex;
	IEnvironment& _env;
};


} } // namespace Sourcey::Spot


#endif // ANIONU_SPOT_IModule_H