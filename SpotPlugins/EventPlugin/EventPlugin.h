#ifndef ANIONU_SPOT_EventPlugin_H
#define ANIONU_SPOT_EventPlugin_H


#include "Anionu/Spot/API/IPlugin.h"
	/// Include the IPlugin.h header.
	/// This is the only header that is required for 
	/// creating plugins.

#include "Anionu/Spot/API/IEnvironment.h"
#include "Anionu/Spot/API/IModule.h"

#include "Anionu/Spot/API/ISympleProcessors.h"
	/// The message processor provides an ABI agnostic
	/// interface for procession all incoming, outgoing
	/// and local messages passing through the Spot client.

#include "Sourcey/Symple/Client.h"
	/// Include the Symple client headers, as we will
	/// be listening for incoming presence messages.
	

namespace Scy {
namespace Anionu {
namespace Spot {


class EventPlugin: 
	public API::IPlugin, 
	public API::IModuleBase, 
	public API::IMessageProcessor
	/// Define our EventPlugin class, which extends the IPlugin interface.
	///
	/// The plugin class name must match the plugin "id"
	/// specified in the manifest.json file.
{
public:
	EventPlugin();
	~EventPlugin();

	bool load();
	void unload();
	
	//void onRecvPresence(void*, Symple::Presence& p);
	virtual void onPresence(const char* presence);

	const char* className() const { return "EventPlugin"; }
};


} } } // namespace Scy::Anionu::Spot


#endif // ANIONU_SPOT_EventPlugin_H