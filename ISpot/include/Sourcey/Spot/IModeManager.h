#ifndef ANIONU_SPOT_IModeManager_H
#define ANIONU_SPOT_IModeManager_H


#include "Sourcey/EventfulManager.h"
#include "Sourcey/Spot/IMode.h"
#include "Sourcey/Spot/IModule.h"

#include "Poco/Format.h"

#include <map>
#include <assert.h>


namespace Sourcey {
namespace Spot {


class IChannel;


typedef EventfulManager<std::string, IMode>	ModeBase;
typedef ModeBase::Map					ModeMap;


class IModeManager: public ModeBase, public IModule
{
public:
	IModeManager(IChannel& channel);
	virtual ~IModeManager();
	
	virtual const char* className() const { return "ModeManager"; }

protected:
	IChannel& _channel;
};


} } // namespace Sourcey::Spot


#endif // ANIONU_SPOT_IModeManager_H