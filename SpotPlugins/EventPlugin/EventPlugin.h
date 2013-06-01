#ifndef ANIONU_SPOT_EventPlugin_H
#define ANIONU_SPOT_EventPlugin_H


#include "Anionu/Spot/API/IPlugin.h"
	/// Include the IPlugin.h header.
	/// This is the only header that is required for 
	/// creating plugins.

#include "Anionu/Spot/API/IModule.h"

#include "Anionu/Spot/API/IMessageProcessor.h"
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
	/// Define the EventPlugin class.
	///
	/// Note that the plugin class name must match the "id" of
	/// the plugin specified in the accompanying manifest.json.
	///
	public API::IPlugin, 
		/// The IPlugin interface is the bare minimum required by
		/// Spot in order to load a plugin/shared library.
		///
	public API::IModuleBase, 
		/// The IModuleBase interface exposes Spot's base environment.
		/// It contains only a subset of the full API, but since it is
		/// ABI agnostic, it enables you the freedom to compile your
		/// plugin on any compiler.
		///
		/// If you intend to use the full API, you would inherit from 
		/// API::IModule instead.
		///
	public API::IMessageProcessor
		/// The IMessageProcessor captures incoming and outgoing
		/// messages between Spot and the Anionu server.
		///
		/// This plugin specifically utilizes the onPresence method.
		/// Note that JSON message data is passed in as const char*
		/// for ABI flexibility, so it must be parsed locally.
		///
{
public:
	EventPlugin();
	~EventPlugin();

	bool load();
	void unload();
	
	void onPresence(const char* presence);

	const char* className() const { return "EventPlugin"; }
};


} } } // namespace Scy::Anionu::Spot


#endif // ANIONU_SPOT_EventPlugin_H