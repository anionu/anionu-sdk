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


#ifndef ANIONU_SPOT_IModeRegistry_H
#define ANIONU_SPOT_IModeRegistry_H


#include "Sourcey/Base.h"
//#include "Sourcey/IRegistry.h"

#include <map>


namespace Sourcey {
namespace Spot {


class IMode;
class IChannel;
class IEnvironment;


// ---------------------------------------------------------------------
//
// IMode Factory
//
// ---------------------------------------------------------------------
template<typename T> 
IMode* createT(IEnvironment& env, IChannel& channel) { return new T(env, channel); }


class IModeRegistry//: public IRegistry<IMode>
{
public:
	typedef std::map<std::string, IMode*(*)(IEnvironment&, IChannel&)> ModeTypeMap;

	IModeRegistry() {};
	~IModeRegistry() {};

    IMode* createInstance(const std::string& name, IEnvironment& env, IChannel& channel) 
	{
        ModeTypeMap::iterator it = _types.find(name);
        if (it == _types.end())
            return NULL;
        return it->second(env, channel);
    }
	
	template<typename T>
    void registerMode(const std::string& name)	
	{ 
        //_types.insert(std::make_pair(name, &createT<T>)); // vc2010 bug
        _types[name] = &createT<T>;
		ModeRegistered.dispatch(this, name);
    }
	
    void unregisterMode(const std::string& name)	
	{ 
        ModeTypeMap::iterator it = _types.find(name);
        if (it == _types.end())
            return;
		_types.erase(it);
		ModeUnregistered.dispatch(this, name);
    }

    ModeTypeMap types() const { return _types; }

	Signal<const std::string&> ModeRegistered;
	Signal<const std::string&> ModeUnregistered;

private:
    ModeTypeMap _types;
};


} } // namespace Sourcey::Spot


#endif // ANIONU_SPOT_IModeRegistry_H