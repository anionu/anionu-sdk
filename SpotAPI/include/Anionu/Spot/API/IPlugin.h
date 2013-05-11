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


#ifndef ANIONU_SPOT_API_IPlugin_H
#define ANIONU_SPOT_API_IPlugin_H


#include "Sourcey/Logger.h"
#include "Anionu/Spot/API/IEnvironment.h"
#include "Poco/ClassLibrary.h"
#include "Poco/Mutex.h"


namespace Scy {
namespace Symple {
	class IFormProcessor;
	/// The symple form processor can be optionally included
	/// for integrating your plugin with the Anionu dashboard
	/// for remote configuration.
}
namespace Anionu {
namespace Spot { 
namespace API { 
	class IEnvironment;
	

class IPlugin
	/// The plugin interface exposes the Spot API environment
	/// for building shared libraries that extend the core
	/// functionality of the Spot client.
{
public:
	IPlugin::IPlugin() : _env(NULL), _proc(NULL) {};
	virtual ~IPlugin() = 0 {};

	virtual void initialize() = 0;
		/// This method initializes the plugin when the plugin is
		/// loaded by Spot, either on startup, or manually via Spot 
		/// or the online dashboard.
		///
		/// The IEnvironment instance has already been set, exposing 
		/// the Spot API interface to the plugin. See IPlugin::env().
		///
		/// Any system compatibility or runtime checks they should
		/// be done here. If there is an unrecoverable error, an 
		/// Exception with a descriptive message should be thrown
		/// to set the plugin in an error state.

	virtual void uninitialize() = 0;
		/// This method uninitializes the plugin when the plugin is
		/// unloaded by Spot, either on shutdown or manually via
		/// Spot or online dashboard.
		///
		/// Any memory allocated but he plugin should be freed.
	
	API::IEnvironment* env() const
		/// Returns the Spot API interface pointer.
	{ 
		Poco::FastMutex::ScopedLock lock(_mutex);
		return _env; 
	}
	
	Symple::IFormProcessor* proc() const
		/// Returns the optional Symple Form processor pointer.
	{ 
		Poco::FastMutex::ScopedLock lock(_mutex);
		return _proc; 
	}

	std::string path() const
		/// Returns the full path to the plugin library.
	{ 
		Poco::FastMutex::ScopedLock lock(_mutex);
		return _path;
	}
		
	LogStream log(const char* level = "debug") const
		/// This method sends log messages the Spot logger.
	{ 
		Poco::FastMutex::ScopedLock lock(_mutex);

		// The IEnvironment instance will be NULL
		// until setEnvironment is been called, 
		// therefore messages logged before the 
		// plugin is intialized will be lost.
		if (_env == NULL)
			return LogStream();
	
		return _env->logger().send(level, this, className()); 
	}


	//
	/// Private Methods
	//	

	void setEnvironment(API::IEnvironment* env)
		/// The IEnvironment instance will be set by
		/// Spot before plugin initialization.
	{ 
		Poco::FastMutex::ScopedLock lock(_mutex);
		_env = env; 

		// Also set the default logger instance, otherwise
		// a new default logger instance will be created
		// for the plugin process.
		Logger::setInstance(&_env->logger());
	}

	void setFormProcessor(Symple::IFormProcessor* proc)
		/// The should be set during initialize() in the plugin
		/// is designed to intergrate with the online dashboard
		/// for remote configuration. 
	{ 
		Poco::FastMutex::ScopedLock lock(_mutex);
		_proc = proc; 
	}

	void setPath(const std::string& path)
		/// The full path of the plugin will be set by
		/// Spot before plugin initialization.
	{ 
		Poco::FastMutex::ScopedLock lock(_mutex);
		_path = path; 
	}

	virtual const char* className() const { return "Plugin"; }
		/// Override this method for named logging.
	
private:
	std::string _path;
	API::IEnvironment* _env;
	Symple::IFormProcessor* _proc;
	mutable Poco::FastMutex _mutex;
};


} } } } // namespace Scy::Anionu::Spot::API


#define DEFINE_SPOT_PLUGIN(ClassName)			     \
POCO_BEGIN_MANIFEST(Scy::Anionu::Spot::API::IPlugin) \
	POCO_EXPORT_CLASS(ClassName)				     \
POCO_END_MANIFEST								     \


#endif /// ANIONU_SPOT_API_IPlugin_H