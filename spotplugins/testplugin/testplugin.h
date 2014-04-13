//
// Anionu SDK
// Copyright (C) 2011, Anionu <http://anionu.com>
//
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

#ifndef Anionu_Spot_TestPlugin_H
#define Anionu_Spot_TestPlugin_H


#include "anionu/spot/api/iplugin.h"
#include "anionu/spot/api/imode.h"
#include "anionu/spot/api/iplugin.h"
#include "anionu/spot/api/imodule.h"
#include "anionu/spot/api/iformprocessor.h"
#include "anionu/spot/api/imessageprocessor.h"
#include "anionu/spot/api/streamingmanager.h"
#include "anionu/spot/api/mediamanager.h"
#include "anionu/spot/api/channelmanager.h"
#include "anionu/spot/api/channel.h"
#include "scy/media/avpacketencoder.h"
#include "scy/symple/client.h"


namespace scy {
namespace anio { 
namespace spot {


class TestPlugin: 
	/// Define the TestPlugin class.
	///
	public api::IPlugin,
		/// The IPlugin interface is bare minimum required by Spot
        /// to load plugins and shared libraries.
		///
	public api::IModeFactory, 
		/// This IModeFactory enables is to extend surveillance channels
		/// with operational modes. Modes can be activated and deactivated
		/// via the online dashboard, and generally utilize the channel
		/// media captures. See Surveillance Mode and Recording Mode.
	public api::IMessageProcessorBase,
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
	TestPlugin(api::Environment& env);
	virtual ~TestPlugin();
	
	//
	/// IPlugin
	bool load();
	void unload();

	const char* errorMessage() const;

	//
	/// IMessageProcessorBase
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
	/// Core API media callbacks and methods 
	void onSetupStreamingSources(void*, api::StreamingSession& session, bool& handled);
	void onSetupStreamingEncoders(void*, api::StreamingSession& session, bool& handled);
	void onSetupRecordingSources(void*, api::RecordingSession& session, bool& handled);
	void onSetupRecordingEncoders(void*, api::RecordingSession& session, bool& handled);

	static bool createCaptures(PacketStream& stream, av::EncoderOptions& options, const api::Channel* channel);
	static bool createEncoder(PacketStream& stream, av::EncoderOptions& options);
	void registerMediaFormats();	
	
protected:	
	std::string	_error;
};


} } } // namespace scy::anio::spot


#endif // Anionu_Spot_TestPlugin_H