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


#ifndef Anionu_Spot_API_IPlugin_H
#define Anionu_Spot_API_IPlugin_H


#include "Anionu/Spot/API/Config.h"
#include "Anionu/Spot/API/IModule.h"


namespace scy {
namespace anio {
namespace spot { 
namespace api { 
	
	
class IPlugin
	/// This class defines the ABI agnostic virtual interface
	/// that Spot uses to load shared libraries and plugins.
	///
	/// This class should only be used if you want to build
	/// Spot plugins using ABI incompatible compilers and
	/// dependencies to build your plugins, and still maintain
	/// compatibility. If you want to utilize the full Spot API
	/// environment, then your libraries should implement the 
	/// IPlugin interface instead.
	///
	/// See the MongoPlugin (included in the Anionu SDK) for an
	/// example of how IPlugin can be combined with the
	/// IFormProcessor to process all incoming and outgoing
	/// Spot client/server messages without compromising ABI
	/// compatibility.
	///
{
public:
	virtual ~IPlugin() = 0 {};

	virtual bool load() = 0;
		// This method initializes the shared library when it
		// is loaded by Spot, either on startup, or manually 
		// by the user.
		//
		// Any runtime or system compatibility checks they should
		// be done here. If there is an unrecoverable error, this
		// method should return false. A detailed error message may
		// be returned to the client via the errorMessage() method.

	virtual void unload() = 0;
		// This method uninitializes the library when it is unloaded
		// by Spot, either on shutdown or manually by the user.
		//
		// Any memory allocated by the library should be freed here.

	virtual void setPath(const char* path) { (void)path; };
		// The full path to the plugin shared library will be set
		// by Spot before the load() method is called.
		// Override this method if you want access to the path.

	virtual const char* errorMessage() const { return 0; };
		// Returns a detailed error message if load() fails.
		// Override this method if you want to provide an error message.

	virtual const char* docFile() { return 0; };
		// Returns the relative path (from the Spot binary dir)
		// to the optional help guide/documentation pertaining
		// to the current module.
		// Information files should be in Markdown format.
};


SOURCEY_EXTERN typedef spot::api::IPlugin* (*InitializePluginFunc)(spot::api::EnvironmentBase&);

struct PluginDetails {
	const char* sdkVersion;
	const char* fileName;
	const char* className;
	const char* pluginName;
	const char* pluginVersion;
	InitializePluginFunc initializeFunc;
};


#ifdef WIN32
# define SPOT_PLUGIN_EXPORT __declspec(dllexport)
#else
# define SPOT_PLUGIN_EXPORT // empty
#endif

#define SPOT_STANDARD_PLUGIN_STUFF \
	spot::api::SDKVersion,		   \
	__FILE__

#define SPOT_BASE_PLUGIN(classType, pluginName, pluginVersion) 		\
  extern "C" {		                                         		\
      SPOT_PLUGIN_EXPORT spot::api::IPlugin* initializePlugin(spot::api::EnvironmentBase& env) \
      {                                                     		\
	      return new classType(env);                         		\
      }																\
      SPOT_PLUGIN_EXPORT spot::api::PluginDetails pluginDetails =   \
      {                                                      		\
          SPOT_STANDARD_PLUGIN_STUFF,                        		\
          #classType,                                       		\
          pluginName,                                        		\
          pluginVersion,                                     		\
          initializePlugin,                                  		\
      };                                                     		\
  }


#define SPOT_CORE_PLUGIN(classType, pluginName, pluginVersion) 		\
  extern "C" {		                                        		\
      SPOT_PLUGIN_EXPORT spot::api::IPlugin* initializePlugin(spot::api::EnvironmentBase& env) \
      {                                                     		\
	      return new classType(dynamic_cast<spot::api::Environment&>(env)); \
      }                                                     		\
      SPOT_PLUGIN_EXPORT spot::api::PluginDetails pluginDetails =	\
      {                                                     		\
          SPOT_STANDARD_PLUGIN_STUFF,                        		\
          #classType,                                        		\
          pluginName,                                        		\
          pluginVersion,                                     		\
          initializePlugin,                                  		\
      };                                                     		\
  }


} } } } // namespace scy::anio::spot::api


#endif /// Anionu_Spot_API_IPlugin_H




/*
#define SPOT_PLUGIN(ClassName)					 \
POCO_BEGIN_MANIFEST(scy::anio::spot::api::IPlugin) \
	POCO_EXPORT_CLASS(ClassName)						 \
POCO_END_MANIFEST										 \
*/



/*

	//virtual void onSessionStart() {};
	//virtual void onSessionEnd() {};	
#ifdef Anionu_Spot_USING_CORE_API


class IPlugin: public IPlugin
	/// The plugin interface exposes the Spot API environment
	/// for building shared libraries that extend the core
	/// functionality of the Spot client.
	///
	/// IPlugin should be extended with any interfaces you
	/// intend on implementing. See the Anionu SDK for full 
	/// plugin examples.
{
public:
	//IPlugin::IPlugin() : _env(null) {};
	virtual ~IPlugin() = 0 {};	

	std::string path() const
		// Returns the full path to the plugin shared library.
		// Do not attempt to call inside constructor.
		// Available only inside and after load() has been called.
	{ 
		Mutex::ScopedLock lock(_mutex);
		assert(!_path.empty());
		return _path;
	}	

	virtual const char* error() const 
		// Overrides the IPlugin base to return a descriptive
		// error message on plugin load failure.
	{ 
		Mutex::ScopedLock lock(_mutex);
		return _error.empty() ? 0 : _error.c_str();
	}
	

	//
	/// Private methods
	//	

	void setPath(const char* path)
		// The full path of the plugin will be set by 
		// Spot before the load() method is called.
	{ 
		Mutex::ScopedLock lock(_mutex);
		_path = path; 
	}

private:
	std::string _path;
	std::string _error;
};


#endif /// Anionu_Spot_USING_CORE_API

*/

	/*
	virtual bool load() = 0;
		// This method initializes the plugin when it is loaded
		// by Spot, either on startup, or manually by the user.
		//
		// The Environment instance has already been set,
		// exposing  the Spot API environment to the plugin. 
		// See IPlugin::env()
		//
		// Any runtime or system compatibility checks they should
		// be done here. If there is an unrecoverable error, this
		// method should return false. A detailed error message may
		// be returned to the client via the error() method.

	virtual void unload() = 0;
		// This method uninitializes the library when it is unloaded
		// by Spot, either on shutdown or manually by the user.
		//
		// Any memory allocated by the library should be freed here.
		*/

/*

#define SPOT_PLUGIN(ClassName)			     \
POCO_BEGIN_MANIFEST(scy::anio::spot::api::IPlugin) \
	POCO_EXPORT_CLASS(ClassName)				     \
POCO_END_MANIFEST								     \
*/
	
	
	/*, _proc(null)
	

	//
	/// Private Methods
	//	

	
protected:


	api::IFormProcessor* proc() const
		// Returns the optional Symple Form processor pointer.
	{ 
		Mutex::ScopedLock lock(_mutex);
		return _proc; 
	}
	*/
	/*
	void setFormProcessor(api::IFormProcessor* proc)
		// The should be set during initialize() in the plugin
		// is designed to integrate with the online dashboard
		// for remote configuration. 
	{ 
		Mutex::ScopedLock lock(_mutex);
		_proc = proc; 
	}
	//api::IFormProcessor* _proc;
	*/