//
// LibSourcey
// Copyright (C) 2005, Sourcey <http://sourcey.com>
//
// LibSourcey is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// LibSourcey is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.
//


#ifndef Anionu_Spot_API_IEnvironment_H
#define Anionu_Spot_API_IEnvironment_H


#include "Anionu/Spot/API/Config.h"
#include <string>


namespace scy {
	class Logger;
	class IConfiguration;
namespace anio {		
namespace spot { 
namespace api { 


// Forward declare base API interfaces 
// to be included as required.
class IEventManagerBase;
class IMediaManagerBase;
class IClientBase;
class ISynchronizerBase;
class ISessionBase;


class IEnvironmentBase
	/// The base API environment exposes an ABI agnostic subset of the actual   
	/// core API envionment. See IEnvironment below for the core API envionment. 
	///
	/// The tradeoffs of using the base API over the core API are features and STL
	/// convenience in favour of binary compatability. No complex types are used
	/// in the base API, only plain-old-data all the way. 
	/// This means that you should be able to use pretty much any compiler to build
	/// Spot plugins, so long as you ONLY link with the base API environment.
	///
	/// Use IModuleBase to include the base API in your plugins.
{
public:
	virtual api::IClientBase& clientBase() = 0;
	virtual api::IMediaManagerBase& mediaBase() = 0;
	virtual api::ISynchronizerBase& synchronizerBase() = 0;
	
	virtual void log(const char* message, const char* level = "trace", const char* realm = "") const = 0;
		/// Log a message to the Spot application logger.
		/// See IModule::log() for an STL friendly interface.

	virtual const char* cVersion() const = 0;
		/// The currently installed Spot package version.

	virtual const char* cStoragePath() const = 0;
		/// The storage path configured for the current user session.
		/// Returns the default path if no session is available.
		
protected:
	virtual ~IEnvironmentBase() = 0 {};
};


// ---------------------------------------------------------------------
//
#ifdef Anionu_Spot_USING_CORE_API
//
// Forward declare core API interfaces.
class IStreamingManager;
class IChannelManager;
class IModeManager;
class IMediaManager;
class ISynchronizer;
class IClient;


class IEnvironment: public IEnvironmentBase
	/// The core API environment exposes Spot's internal modules and functionality
	/// to Spot plugins. 
	///
	/// The core API shares complex data types such as STL and cv::VideoCapture,
	/// so binary compatability between your plugins and target Spot client
	/// is essential. To acheive binary compatability your plugins must be built
	/// with exactly the same compiler, build environment and dependency library
	/// versions that were used to build the target Spot client.
	/// This is all made easy by including the Anionu dpendencies package in your 
	/// build tree. Please read the Anionu plugin tutorial for more information.
	/// See IEnvironmentBase if you want to sacrifice features and convenience for
	/// binary compatability.
	///
	/// Use IModule to include the core API environment in your plugins.
{
public:
	virtual IConfiguration& config() = 0;
		/// Configuration for the current user session.

	virtual IConfiguration& appConfig() = 0;
		/// Configuration for the entire application.

	virtual api::IClient& client() = 0;	
		/// The Anionu client interface implements remote messaging and 
		/// surveillance event creation.

	virtual api::IMediaManager& media() = 0;
		/// The media manager implements recording, and stored media format
		/// configuration for the current user session.

	virtual api::IChannelManager& channels() = 0;
		/// The channel manager provides access to surveillance channels,
		/// and their configured video and audio capture streams. 

	virtual api::IModeManager& modes() = 0;
		/// The mode manager provides for registration and activation of
		/// channel modes.

	virtual api::IStreamingManager& streaming() = 0;
		/// The streaming manager implements streaming session creation 
		/// and management.

	virtual api::ISynchronizer& synchronizer() = 0;
		/// The synchronizer uploads files to the current user's 
		/// online account.

	virtual Logger& logger() = 0;
		/// The Spot application logger.

	virtual std::string version() const = 0;
	virtual std::string storagePath() const = 0;
		
protected:
	virtual ~IEnvironment() = 0 {};
};

#endif // Anionu_Spot_USING_CORE_API


} } } } // namespace anio::spot::api


#endif // Anionu_Spot_API_IEnvironment_H




	///
	/// Spot's base API is designed to be ABI agnostic, so it should be
	/// safe to extend from here and still maintain binary compatability
	/// while using any same platform compiler to build your plugins.
    ///
	/// The tradeoffs are you can only share plain-old-data with the Spot client. 
///which means that no  The advantage here is that since Spot's base API is designed to be ABI agnostic,
	/// 
	/// complex types are used in the base API, 