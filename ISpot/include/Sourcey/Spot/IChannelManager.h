#ifndef ANIONU_SPOT_IChannelManager_H
#define ANIONU_SPOT_IChannelManager_H


#include "Sourcey/EventfulManager.h"
#include "Sourcey/Spot/IChannel.h"
#include "Sourcey/Spot/IModule.h"


namespace Sourcey { 
namespace Spot {


class IChannelManager: public EventfulManager<std::string, IChannel>, public IModule
{
public:
	typedef EventfulManager<std::string, IChannel>	Manager;
	typedef Manager::Map							Map;
	
public:
	IChannelManager(IEnvironment& env);
	virtual ~IChannelManager();

	virtual IChannel* getChannel(const std::string& name);
		// Returns the IChannel instance or throws a
		// NotFoundException exception.
	
	bool addChannel(IChannel* channel);
	bool freeChannel(const std::string& name);	

    virtual IChannel* getDafaultChannel();
		// Returns the first channel in the list.
	
	virtual const char* className() const { return "ChannelManager"; }

protected:		
	mutable Poco::FastMutex _mutex;
};


} } // namespace Sourcey::Spot


#endif // ANIONU_SPOT_IChannelManager_H