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


#ifndef ANIONU_SPOT_API_ISession_H
#define ANIONU_SPOT_API_ISession_H


#include "Sourcey/Base.h"
#include "Sourcey/Stateful.h"


#include "Poco/Path.h"

#include <iostream>
#include <string>
#include <assert.h>


namespace Scy { 
namespace Anionu {
namespace Spot { 
namespace API { 


struct SessionState: public State 
{
	enum Type 
	{
		None = 0,
		Authenticating,
		Failed,
		ActiveOffline,
		ActiveConnecting,
		ActiveOnline,
		ActiveFailed,
	};

	std::string str(unsigned int id) const 
	{ 
		switch(id) {
		case None:				return "None";
		case Authenticating:	return "Authenticating";
		case Failed:			return "Failed";
		case ActiveOffline:		return "Active (Offline)";
		case ActiveConnecting:	return "Active (Connecting)";
		case ActiveOnline:		return "Active (Online)";
		case ActiveFailed:		return "Active (Failed)";
		default:				assert(false);
		}
		return "undefined"; 
	}
};


class ISession: public StatefulSignal<SessionState>
{		
public:	
	virtual std::string name() const = 0;
	virtual std::string username() const = 0;

	virtual Poco::Path storagePath() = 0;

	virtual bool isActive() const = 0;
		
	NullSignal SessionStart;
	NullSignal SessionEnd;
	
protected:
	virtual ~ISession() = 0 {};
};


} } } } // namespace Scy::Anionu::Spot::API


#endif // ANIONU_SPOT_API_ISession_H
