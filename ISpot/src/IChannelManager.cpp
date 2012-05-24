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