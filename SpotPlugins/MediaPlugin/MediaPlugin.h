#ifndef Anionu_Spot_MediaPlugin_H
#define Anionu_Spot_MediaPlugin_H


#include "Anionu/Spot/API/IPlugin.h"
#include "Anionu/Spot/API/IMode.h"
#include "Anionu/Spot/API/IPlugin.h"
#include "Anionu/Spot/API/IModule.h"
#include "Anionu/Spot/API/IFormProcessor.h"
#include "Anionu/Spot/API/IMessageProcessor.h"
#include "Anionu/Spot/API/IStreamingManager.h"
#include "Anionu/Spot/API/IMediaManager.h"
#include "Sourcey/Media/AVEncoder.h"
#include "Sourcey/Symple/Client.h"


namespace scy {
namespace anionu { 
namespace spot {


class MediaPlugin: 
	/// Define the MediaPlugin class.
	///
	/// Note that the plugin class name must match the "id" of
	/// the plugin specified in the accompanying manifest.json.
	///
	public api::IPlugin,
		/// The IPlugin interface is bare minimum required by Spot
        /// to load plugins and shared libraries.
		///
	public api::IMessageProcessor,
		/// The IMessageProcessor captures incoming and outgoing
		/// messages between Spot and the Anionu server.
		///
		/// This plugin specifically utilizes the onPresence method.
		/// Note that JSON message data is passed in as const char*
		/// for ABI flexibility, so it must be parsed locally.
		///
		/// The message processor provides an ABI agnostic
		/// interface for procession all incoming, outgoing
		/// and local messages passing through the Spot client.
		///
#ifdef Anionu_Spot_USING_CORE_API		
		///
        /// The IModule interface exposes Spot's internal class structure
        /// to the plugin. There are two API levels which may be linked
        /// with:
	public api::IModule
        /// IModule exposes the full core API environment.
        /// The core API shares complex data types such as STL and
        /// cv::VideoCapture with the application, so strict binary compatability
        /// between your plugin and the target Spot client is required.
        /// This is the recommended API to use, especially for beginners.
        ///
#else
	public api::IModuleBase
		/// IModuleBase exposes the ABI agnostic base environment.
		/// The base contains only a subset of the core API features, but
		/// since no complex types are used, only plain-old-data, you have
		/// greater freedom to compile your plugin with any compiler and
        /// maintain compatability between SDK versions.
#endif
{
public:
	MediaPlugin();
	virtual ~MediaPlugin();

	bool load();
	void unload();

	//
	/// IMessageProcessor
	bool onMessage(const char* message);
	bool onCommand(const char* command);
	void onPresence(const char* presence);
	void onEvent(const char* event);

	//
	/// IModeFactory
	api::IMode* createModeInstance(const char* modeName, const char* channelName);
	const char** modeNames() const;
	
	//
	/// ISynchronizer
	void synchronizeTestVideo();	
	
	//
	/// Core API media methods and callbacks
#ifdef Anionu_Spot_USING_CORE_API
	av::IPacketEncoder* createEncoder(const av::RecordingOptions& options);	
	void registerMediaFormats();
	
	void onInitStreamingSession(void*, api::IStreamingSession& session, bool& handled);
	void onInitRecordingEncoder(void*, const api::RecordingOptions& options, av::IPacketEncoder*& encoder);
#endif

	const char* errorMessage() const;
	const char* className() const { return "MediaPlugin"; }
	
protected:	
	mutable Poco::FastMutex _mutex;
	std::string	_error;
};


} } } // namespace scy::anionu::Spot


#endif // Anionu_Spot_MediaPlugin_H