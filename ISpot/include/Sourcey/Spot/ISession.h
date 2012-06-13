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


#ifndef ANIONU_SPOT_ISession_H
#define ANIONU_SPOT_ISession_H


#include "Sourcey/Base.h"
#include "Sourcey/Stateful.h"
#include "Sourcey/Spot/IModule.h"

#include "Poco/Path.h"

#include <iostream>
#include <string>
#include <assert.h>


namespace Sourcey {
namespace Spot {


struct SessionState: public StateT 
{
	enum Type 
	{
		None = 0,
		Authenticating,
		ActiveOffline,
		ActiveConnecting,
		ActiveOnline,
		Failed,
	};

	std::string str(unsigned int id) const 
	{ 
		switch(id) {
		case None:				return "None";
		case Authenticating:	return "Authenticating";
		case ActiveOffline:		return "Active (Offline)";
		case ActiveConnecting:	return "Active (Connecting)";
		case ActiveOnline:		return "Active (Online)";
		case Failed:			return "Failed";
		default:				assert(false);
		}
		return "undefined"; 
	}
};


class IClient;


class ISession: public StatefulSignal<SessionState>, public IModule
{		
public:
	ISession(IEnvironment& env);
	virtual ~ISession();
	
	virtual std::string name() const = 0;
	virtual std::string username() const = 0;
	//virtual std::string password() const = 0;

	virtual Poco::Path storagePath() const = 0;

	virtual bool isActive() const 
	{ 
		return stateEquals(SessionState::ActiveConnecting)
			|| stateEquals(SessionState::ActiveOffline)
			|| stateEquals(SessionState::ActiveOnline); 
	}
	
	virtual const char* className() const { return "Session"; }
};


} } // namespace Sourcey::Spot


#endif // ANIONU_SPOT_ISession_H
