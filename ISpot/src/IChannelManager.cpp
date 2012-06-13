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


#include "Sourcey/Spot/IChannelManager.h"


using namespace std; 


namespace Sourcey {
namespace Spot {

	
IChannelManager::IChannelManager(IEnvironment& env) :
	IModule(env)
{	
}


IChannelManager::~IChannelManager() 
{
}


bool IChannelManager::addChannel(IChannel* channel) 
{
	assert(channel);
	assert(!channel->name().empty());
	
	log() << "Adding Channel: " << channel->name() << endl;	
	return Manager::add(channel->name(), channel);
}


bool IChannelManager::freeChannel(const string& name) 
{
	assert(!name.empty());

	log() << "Deleting Channel: " << name << endl;		
	return Manager::free(name);
}


IChannel* IChannelManager::getChannel(const string& name) 
{
	return Manager::get(name, true);
}


/*
IChannelManager::Map& IChannelManager::items() 
{ 
	Poco::FastMutex::ScopedLock lock(_mutex); 
	return _items;
}
*/


IChannel* IChannelManager::getDafaultChannel()
{
	if (!_items.empty()) {
		IChannelMap::const_iterator it = _items.begin();
		return it->second;
	}

	return NULL;
}



} } // namespace Sourcey::Spot