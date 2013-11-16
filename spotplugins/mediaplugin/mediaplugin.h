#ifndef Anionu_Spot_MediaPlugin_H
#define Anionu_Spot_MediaPlugin_H


#include "anionu/spot/api/iplugin.h"
#include "anionu/spot/api/imode.h"
#include "anionu/spot/api/iplugin.h"
#include "anionu/spot/api/imodule.h"
#include "anionu/spot/api/iformprocessor.h"
#include "anionu/spot/api/imessageprocessor.h"
#include "anionu/spot/api/streamingmanager.h"
#include "anionu/spot/api/mediamanager.h"
#include "scy/media/avpacketencoder.h"
#include "scy/symple/client.h"


namespace scy {
namespace anio { 
namespace spot {


class MediaPlugin: 
	/// Define the MediaPlugin class.
	///
	/// Note that the plugin class name must match the "id" of
	/// the plugin specified in the accompanying plugin.json.
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
		/*
	public api::IModule
        /// IModule exposes the full core API environment.
        /// The core API shares complex data types such as STL and
        /// cv::VideoCapture with the application, so strict binary compatibility
        /// between your plugin and the target Spot client is required.
        /// This is the recommended API to use, especially for beginners.
        ///
		*/
#ifdef Anionu_Spot_USING_CORE_API		
		///
        /// The IModule interface exposes Spot's internal class structure
        /// to the plugin. There are two API levels which may be linked
        /// with:
	public api::IModule
        /// IModule exposes the full core API environment.
        /// The core API shares complex data types such as STL and
        /// cv::VideoCapture with the application, so strict binary compatibility
        /// between your plugin and the target Spot client is required.
        /// This is the recommended API to use, especially for beginners.
        ///
#else
	public api::IModuleBase
		/// IModuleBase exposes the ABI agnostic base environment.
		/// The base contains only a subset of the core API features, but
		/// since no complex types are used, only plain-old-data, you have
		/// greater freedom to compile your plugin with any compiler and
        /// maintain compatibility between SDK versions.
#endif
{
public:
	MediaPlugin(api::Environment& env);
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
	/// Synchronizer
	void synchronizeTestVideo();	
	
	//
	/// Core API media methods and callbacks
#ifdef Anionu_Spot_USING_CORE_API
	av::AVPacketEncoder* createEncoder(const av::RecordingOptions& options);	
	void registerMediaFormats();
	
	void onInitStreamingSession(void*, api::StreamingSession& session, bool& handled);
	void onInitRecordingEncoder(void*, const api::RecordingOptions& options, api::Recorder*& encoder);
#endif

	const char* errorMessage() const;
	const char* className() const { return "MediaPlugin"; }
	
protected:	
	mutable Mutex _mutex;
	std::string	_error;
};


} } } // namespace scy::anio::spot


#endif // Anionu_Spot_MediaPlugin_H