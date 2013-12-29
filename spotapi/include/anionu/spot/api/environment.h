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


#ifndef Anionu_Spot_API_Environment_H
#define Anionu_Spot_API_Environment_H


#include "anionu/spot/api/api.h"
#include <string>


namespace scy {
	class Logger;
	class Configuration;
namespace anio {		
namespace spot { 
namespace api { 


// Forward declare base API interfaces.
// Each module resides in it's own header file,
// so interfaces can be included as required.
class IEventManagerBase;
class MediaManagerBase;
class ClientBase;
class SynchronizerBase;
class ISessionBase;


class EnvironmentBase
	/// The base API environment exposes an ABI agnostic subset of the   
	/// core API environment. See Environment below for the core API environment. 
	///
	/// The tradeoffs of using the base API over the core API are features and STL
	/// convenience in favor of binary compatibility. No complex types are used
	/// in the base API, only plain-old-data all the way. 
	/// This means that you should be able to use pretty much any compiler to build
	/// Spot plugins, so long as you ONLY link with the base API environment.
	///
	/// Do not use this class directly, extend from IModuleBase which exposes a
	/// reference to the EnvironmentBase instance for use by plugins.
{
public:
	virtual api::ClientBase& clientBase() = 0;
	virtual api::MediaManagerBase& mediaBase() = 0;
	virtual api::SynchronizerBase& synchronizerBase() = 0;
	
	virtual void log(const char* message, const char* level = "trace", const char* realm = "") const = 0;
		// Log a message to the Spot application logger.
		// See IModule::log() for an STL friendly interface.

	virtual const char* cVersion() const = 0;
		// The currently installed Spot package version.

	virtual const char* cStorageDir() const = 0;
		// The storage path configured for the current user session.
		// Returns the default path if no session is available.
		
protected:
	virtual ~EnvironmentBase() {};
};


#ifdef Anionu_Spot_USING_CORE_API // Core API
//
// Forward declare core API interfaces.
class StreamingManager;
class ChannelManager;
class ModeManager;
class MediaManager;
class Synchronizer;
class Client;


class Environment: public EnvironmentBase
	/// The core API environment instance which exposes Spot's internal modules  
	/// and functionality to plugins. 
	///
	/// The core API shares complex data types such as STL and cv::VideoCapture,
	/// so binary compatibility between your plugins and target Spot client
	/// is essential. To achieve binary compatibility your plugins must be built
	/// with the exactly same compiler, build environment and third party dependencies 
	/// which were used to build the target Spot client.
	/// This is all made easy by including the Anionu SDK package in the build tree. 
	/// Please read the Anionu plugin creation guide for more information.
	/// See EnvironmentBase if you want to sacrifice features and convenience for
	/// binary compatibility.
	///
	/// Use IModule to include the core API environment in your plugins.
{
public:
	virtual scy::Configuration& config() = 0;
		// Configuration for the current user session.

	virtual scy::Configuration& appConfig() = 0;
		// Configuration for the entire application.

	virtual api::Client& client() = 0;	
		// The Anionu client interface implements remote messaging and 
		// surveillance event creation.

	virtual api::MediaManager& media() = 0;
		// The media manager implements recording, and stored media format
		// configuration for the current user session.

	virtual api::ChannelManager& channels() = 0;
		// The channel manager provides access to surveillance channels,
		// and their configured video and audio capture streams. 

	virtual api::ModeManager& modes() = 0;
		// The mode manager provides for registration and activation of
		// channel modes.

	virtual api::StreamingManager& streaming() = 0;
		// The streaming manager implements streaming session creation 
		// and management.

	virtual api::Synchronizer& synchronizer() = 0;
		// The synchronizer uploads files to the current user's 
		// online account.

	virtual Logger& logger() = 0;
		// The Spot application logger.

	virtual std::string version() const = 0;
	virtual std::string storageDir() const = 0;
		
protected:
	virtual ~Environment() {};
};


#endif // Anionu_Spot_USING_CORE_API


} } } } // namespace scy::anio::spot::api


#endif // Anionu_Spot_API_Environment_H




	///
	/// Spot's base API is designed to be ABI agnostic, so it should be
	/// safe to extend from here and still maintain binary compatibility
	/// while using any same platform compiler to build your plugins.
    ///
	/// The tradeoffs are you can only share plain-old-data with the Spot client. 
///which means that no  The advantage here is that since Spot's base API is designed to be ABI agnostic,
	/// 
	/// complex types are used in the base API, 