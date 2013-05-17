//
// LibSourcey
// Copyright (C) 2005, Sourcey <http://sourcey.com>
//
// LibSourcey is is distributed under a dual license that allows free, 
// open source use and closed source use under a standard commercial
// license.
//
// Non-Commercial Use:
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
// Commercial Use:
// Please contact mail@sourcey.com
//


#ifndef Anionu_Spot_API_IPlugin_H
#define Anionu_Spot_API_IPlugin_H


#include "Anionu/Spot/API/Config.h"
#include "Poco/ClassLibrary.h"


namespace Scy {
namespace Anionu {
namespace Spot { 
namespace API { 
	
	
class IPlugin
	/// This class defines the ABI agnostic virtual interface
	/// that Spot uses to load shared libraries and plugins.
	///
	/// This class should only be used if you want to build
	/// Spot plugins using ABI incompatable compilers and
	/// dependencies to build your plugins, and still maintain
	/// compatability. If you want to utilize the full Spot API
	/// environment, then your libraries should implement the 
	/// IPlugin interface instead.
	///
	/// See the EventPlugin (included in the Anionu SDK) for an
	/// example of how IPlugin can be combined with the
	/// ISympleProcessors to process all incoming and outgoing
	/// Spot client/server messages without compormising ABI
	/// compatability.
	///
{
public:
	virtual ~IPlugin() = 0 {};

	virtual bool load() = 0;
		/// This method initializes the shared library when it
		/// is loaded by Spot, either on startup, or manually 
		/// by the user.
		///
		/// Any runtime or system compatibility checks they should
		/// be done here. If there is an unrecoverable error, this
		/// method should return false. A detailed error message may
		/// be returned to the client via the error() method.

	virtual void unload() = 0;
		/// This method uninitializes the library when it is unloaded
		/// by Spot, either on shutdown or manually by the user.
		///
		/// Any memory allocated by the library should be freed here.

	virtual const char* error() const { return 0; };
		/// Returns a detailed error message if load() fails.
		/// Override this method if you want to provide an error message.

	virtual void setPath(const char* path) {};
		/// The full path to the plugin shared library will be set
		/// by Spot before the load() method is called.
		/// Override this method if you want access to the path.

	virtual const char* helpFile() { return 0; };
		/// Returns the relative path (from the Spot binary dir)
		/// to the optional help guide/documentation pertaining
		/// to the current module.
		/// Information files should be in Markdown format.
};


} } } } // namespace Scy::Anionu::Spot::API


#define DEFINE_SPOT_PLUGIN(ClassName)					 \
POCO_BEGIN_MANIFEST(Scy::Anionu::Spot::API::IPlugin) \
	POCO_EXPORT_CLASS(ClassName)						 \
POCO_END_MANIFEST										 \


#endif /// Anionu_Spot_API_IPlugin_H





/*

	//virtual void onSessionStart() {};
	//virtual void onSessionEnd() {};	
#ifdef ENFORCE_STRICT_ABI_COMPATABILITY


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
	//IPlugin::IPlugin() : _env(NULL) {};
	virtual ~IPlugin() = 0 {};	

	std::string path() const
		/// Returns the full path to the plugin shared library.
		/// Do not attempt to call inside constructor.
		/// Available only inside and after load() has been called.
	{ 
		Poco::FastMutex::ScopedLock lock(_mutex);
		assert(!_path.empty());
		return _path;
	}	

	virtual const char* error() const 
		/// Overrides the IPlugin base to return a descriptive
		/// error message on plugin load failure.
	{ 
		Poco::FastMutex::ScopedLock lock(_mutex);
		return _error.empty() ? 0 : _error.data();
	}
	

	//
	/// Private methods
	//	

	void setPath(const char* path)
		/// The full path of the plugin will be set by 
		/// Spot before the load() method is called.
	{ 
		Poco::FastMutex::ScopedLock lock(_mutex);
		_path = path; 
	}

private:
	std::string _path;
	std::string _error;
};


#endif /// ENFORCE_STRICT_ABI_COMPATABILITY

*/

	/*
	virtual bool load() = 0;
		/// This method initializes the plugin when it is loaded
		/// by Spot, either on startup, or manually by the user.
		///
		/// The IEnvironment instance has already been set,
		/// exposing  the Spot API environment to the plugin. 
		/// See IPlugin::env()
		///
		/// Any runtime or system compatibility checks they should
		/// be done here. If there is an unrecoverable error, this
		/// method should return false. A detailed error message may
		/// be returned to the client via the error() method.

	virtual void unload() = 0;
		/// This method uninitializes the library when it is unloaded
		/// by Spot, either on shutdown or manually by the user.
		///
		/// Any memory allocated by the library should be freed here.
		*/

/*

#define DEFINE_SPOT_PLUGIN(ClassName)			     \
POCO_BEGIN_MANIFEST(Scy::Anionu::Spot::API::IPlugin) \
	POCO_EXPORT_CLASS(ClassName)				     \
POCO_END_MANIFEST								     \
*/
	
	
	/*, _proc(NULL)
	

	//
	/// Private Methods
	//	

	
protected:


	API::IFormProcessor* proc() const
		/// Returns the optional Symple Form processor pointer.
	{ 
		Poco::FastMutex::ScopedLock lock(_mutex);
		return _proc; 
	}
	*/
	/*
	void setFormProcessor(API::IFormProcessor* proc)
		/// The should be set during initialize() in the plugin
		/// is designed to intergrate with the online dashboard
		/// for remote configuration. 
	{ 
		Poco::FastMutex::ScopedLock lock(_mutex);
		_proc = proc; 
	}
	//API::IFormProcessor* _proc;
	*/