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


#ifndef Anionu_Spot_API_IModule_H
#define Anionu_Spot_API_IModule_H


#include "anionu/spot/api/api.h"
#include "anionu/spot/api/environment.h"

#ifdef Anionu_Spot_USING_CORE_API
#include "scy/logger.h"
#endif


namespace scy {
namespace anio {
namespace spot { 
namespace api { 


template <class IEnvType>
class IModuleTmpl
	/// This template exposes the Spot API instance  
	/// for plugins which extend and integrate with Spot.
	///
	/// IEnvType may be a EnvironmentBase or a Environment
	/// depending on which API level you're linking with.
	/// See Module types below for both IModuleBase and IModule
	/// implementations.
{
public:
	IModuleTmpl(api::EnvironmentBase& env) : 
		_env(dynamic_cast<IEnvType&>(env))
	{
	}

	virtual IEnvType& env() const
		// Returns a reference to the Spot API environment instance.
	{ 
		return _env; 
	}

#if 0
	virtual void log(const std::string& message, const char* level = "debug") 
		// Sends a log message Spot's application logger.
	{
		// No STL types are shared with the client so we
		// can retain binary compatibility across compilers.
		env().log(message.c_str(), level, className());
	}
#endif
	
protected:
	virtual ~IModuleTmpl() {};	
	IEnvType& _env;

private:
	IModuleTmpl(const IModuleTmpl&); // = delete;
	IModuleTmpl(IModuleTmpl&&); // = delete;
	IModuleTmpl& operator=(const IModuleTmpl&); // = delete;
	IModuleTmpl& operator=(IModuleTmpl&&); // = delete;
};


//
// Module Types
//


typedef IModuleTmpl<api::EnvironmentBase> IModuleBase;
	/// IModuleBase exposes Spot's base API environment instance
	/// to plugins and modes extending this interface. 
	///
	/// See EnvironmentBase for more information.


#ifdef Anionu_Spot_USING_CORE_API

typedef IModuleTmpl<api::Environment> IModule;
	/// IModuleBase exposes Spot's core API environment instance
	/// to plugins and modes extending this interface. 
	///
    /// See Environment for more information.

#endif /// Anionu_Spot_USING_CORE_API


} } } } // namespace scy::anio::spot::api


#endif /// Anionu_Spot_API_IModule_H

	
	/*
	//
	/// Private Methods
	//

	void setEnvironment(IEnvType* env)
		// Provides Spot with the means to set the environment instance
		// for IPlugin implementations.
		//
		// Spot will set the environment instance directly after plugin
		// instantiation, so IPlugin implementations must not attempt
		// to access the environment pointer inside the constructor.
		// Once the instance is set Spot will not touch the pointer again.
	{ 
		_env = env; 
	}
	*/

/*
	

		// If the environment instance is not passed in via the constructor, 
		// then the instance is guaranteed to be set by Spot directly 
		// after instantiation. Such is the case when extending IPlugin.
		Mutex::ScopedLock lock(_mutex);
	mutable Mutex	_mutex;	


class IModuleTmpl
	/// ABI agnostic API
	///
	/// Spot's base API is ABI agnostic, so it *should* be
	/// safe to still use your favorite compiler and maintain
	/// binary compatibility with Spot.
{
public:
	IModuleTmpl(api::EnvironmentBase* env = nullptr) : _env(env) {};

	api::EnvironmentBase& env() const
		// Returns the Spot API interface pointer.
		// Do not attempt to call inside constructor.
	{ 
		Mutex::ScopedLock lock(_mutex);
		assert(_env);
		return *_env; 
	}

	api::EnvironmentBase* _env;
		// The Spot API base ABI agnostic interface pointer.
		//
		// Note that the EnvironmentBase pointer might be
		// initialized as nullptr. If the pointer is not passed
		// into the constructor, it is guaranteed to be set
		// by Spot directly after instantiation.

	virtual void log(const std::string& message, const char* level = "debug") 
		// Sends log messages the Spot application logger.
	{
		if (_env == nullptr) 
			return;
		_env->log(message.c_str(), level, className());
	}
	

	virtual const char* className() const { return "IModule"; }
		// Override for named logging.
	
protected:
	virtual ~IModuleTmpl() = 0;
	mutable Mutex	_mutex;	
};
*/

	


	

/*
// ---------------------------------------------------------------------
//
class IModule: public IModuleTmpl
{
public:
	IModule(api::Environment* env = nullptr) : env(env) {};
	virtual ~IModule() = 0;

	api::Environment* env;
		// The Spot API interface pointer.
};
*/
/*
#ifdef Anionu_Spot_USING_CORE_API
#include "Poco/Mutex.h"
#endif
*/

	/*
	
//protected:
	//api::Environment* _env;
	//mutable Mutex _mutex;
	*/
	
	/*
	/// This class is the unified interface from which all
	/// Spot classes inherit from. It ensures all classes
	/// have an Environment reference and Logger access.


#define Anionu_Spot_USING_CORE_API
	/// This is to specify that we are creating an ABI compatible library.
	/// The current compiler must match the compiler used to create the original Spot binary.
	/// If the version and dependency check fail the build will fail with an error.

	static const bool isABICompatible =
#ifdef Anionu_Spot_USING_CORE_API
		false;
#else
		false;
#endif
		*/
	
	/*, _proc(null)
	

	//
	/// Private Methods
	//	

	
protected:

private:
	mutable Mutex _mutex;

	IModule::IModule() : _env(null) {};

	virtual const char* className() const { return "Plugin"; }
		// Override this method for named logging.
	
	api::Environment& env() const
		// Returns the Spot API interface pointer.
	{ 
		Mutex::ScopedLock lock(_mutex);
		return *_env; 
	}

	std::string path() const
		// Returns the full path to the library library.
	{ 
		Mutex::ScopedLock lock(_mutex);
		return _path;
	}
	
	void setEnvironment(api::Environment* env)
		// The Environment instance will be set by
		// Spot before library initialization.
	{ 
		Mutex::ScopedLock lock(_mutex);
		_env = env; 

		// Also set the default logger instance, otherwise
		// a new default logger instance will be created
		// for the library process.
		Logger::setInstance(&_env->logger());
	}

	void setPath(const char* path)
		// The full path of the library will be set by
		// Spot before library initialization.
	{ 
		Mutex::ScopedLock lock(_mutex);
		_path = path; 
	}

	api::IFormProcessor* proc() const
		// Returns the optional Symple Form processor pointer.
	{ 
		Mutex::ScopedLock lock(_mutex);
		return _proc; 
	}
	*/
	/*
	void setFormProcessor(api::IFormProcessor* proc)
		// The should be set during initialize() in the library
		// is designed to intergrate with the online dashboard
		// for remote configuration. 
	{ 
		Mutex::ScopedLock lock(_mutex);
		_proc = proc; 
	}
	//api::IFormProcessor* _proc;
	*/