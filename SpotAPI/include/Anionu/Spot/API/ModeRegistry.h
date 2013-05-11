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


#ifndef ANIONU_SPOT_API_ModeRegistry_H
#define ANIONU_SPOT_API_ModeRegistry_H


#include "Sourcey/Base.h"
#include "Sourcey/Signal.h"

#include <map>


namespace Scy {
namespace Anionu {
namespace Spot { 
namespace API { 
	class IMode;
	class IChannel;
	class IEnvironment;


template<typename T> 
IMode* createT(API::IEnvironment& env, API::IChannel& channel) { return new T(env, channel); }

typedef std::map<std::string, IMode*(*)(API::IEnvironment&, API::IChannel&)> ModeRegistryMap;


class ModeRegistry
{
public:
	ModeRegistry() {};
	~ModeRegistry() {};
	
	template<typename T>
    void registerMode(const std::string& name)	
	{ 
		{
			Poco::FastMutex::ScopedLock lock(_mutex);	
			//_types.insert(std::make_pair(name, &createT<T>)); // vc2010 bug
			_types[name] = &createT<T>;
		}
		ModeRegistered.emit(this, name);
    }
	
    void unregisterMode(const std::string& name)	
	{ 
		{
			Poco::FastMutex::ScopedLock lock(_mutex);	
			ModeRegistryMap::iterator it = _types.find(name);
			if (it == _types.end())
				return;
			_types.erase(it);
		}
		ModeUnregistered.emit(this, name);
    }

    IMode* createInstance(const std::string& name, API::IEnvironment& env, API::IChannel& channel) 
	{
		Poco::FastMutex::ScopedLock lock(_mutex);	
        ModeRegistryMap::iterator it = _types.find(name);
        if (it == _types.end())
            return NULL;
        return it->second(env, channel);
    }

    ModeRegistryMap types() const 
	{ 
		Poco::FastMutex::ScopedLock lock(_mutex);	
		return _types; 
	}

	Signal<const std::string&> ModeRegistered;
	Signal<const std::string&> ModeUnregistered;

private:
	mutable Poco::FastMutex	_mutex;	
    ModeRegistryMap _types;
};


} } } } // namespace Scy::Anionu::Spot::API


#endif // ANIONU_SPOT_API_ModeRegistry_H