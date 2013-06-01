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


#ifndef Anionu_Spot_API_IModule_H
#define Anionu_Spot_API_IModule_H


#include "Anionu/Spot/API/Config.h"
#include "Anionu/Spot/API/IEnvironment.h"

#ifdef Anionu_Spot_ENABLE_ABI_COMPATABILITY
#include "Sourcey/Logger.h"
#endif


namespace Scy {
namespace Anionu {
namespace Spot { 
namespace API { 


template <class IEnvType>
class IModuleTmpl
	/// This module template lets you link your compiled plugins and modes  
	/// with different levels of the Spot API environment.
	///
	/// See IModuleBase and IModule for the two different implementations.
{
public:
	IModuleTmpl(IEnvType* env = NULL) : _env(env) {};

	virtual IEnvType* env() const
		/// Returns a pointer to the Spot API environment instance.
		///
		/// Do not attempt to access the environment pointer from inside
		/// the constructor for IPlugin implementations. See setEnvironment() 
		/// below for details. This is not an issue for IMode implementations
		/// as the pointer should be passed in via the consatructor.
		///
		/// Override this method if you require mutex synchronization.
	{ 
		return _env; 
	}

	virtual void log(const std::string& message, const char* level = "debug") 
		/// Sends log messages the Spot application logger.
		///
		/// Note that no STL types are shared with the client so we
		/// don't hace to break binary compatability between versions.
	{
		env()->log(message.data(), level, className());
	}	

	virtual const char* className() const { return "IModule"; }
		/// Override for named logging.
	

	//
	/// Private Methods
	//

	void setEnvironment(IEnvType* env)
		/// Provides Spot with the means to set the environment instance
		/// for IPlugin implementations.
		///
		/// Spot will set the environment instance directly after plugin
		/// instantiation, so IPlugin implementations must not attempt
		/// to access the environment pointer inside the constructor.
		/// Once the instance is set Spot will not touch the pointer again.
	{ 
		_env = env; 
	}
	
protected:
	virtual ~IModuleTmpl() = 0 {};	
	IEnvType* _env;
};


// ---------------------------------------------------------------------
//	
typedef IModuleTmpl<API::IEnvironmentBase> IModuleBase;
	/// IModuleBase exposes Spot's base API environment instance to plugins
	/// and modes that extend for here. 
	///
	/// See IEnvironmentBase for more information.


#ifdef Anionu_Spot_ENABLE_ABI_COMPATABILITY

// ---------------------------------------------------------------------
//		
typedef IModuleTmpl<API::IEnvironment> IModule;
	/// IModuleBase exposes Spot's core API environment instance to plugins 
	/// and modes that extend for here.
	///
    /// See IEnvironment for more information.

#endif /// Anionu_Spot_ENABLE_ABI_COMPATABILITY


} } } } // namespace Scy::Anionu::Spot::API


#endif /// Anionu_Spot_API_IModule_H


/*
	

		/// If the environment instance is not passed in via the constructor, 
		/// then the instance is guaranteed to be set by Spot directly 
		/// after instantiation. Such is the case when extending IPlugin.
		Poco::FastMutex::ScopedLock lock(_mutex);
	mutable Poco::FastMutex	_mutex;	


class IModuleTmpl
	/// ABI agnostic API
	///
	/// Spot's base API is ABI agnostic, so it *should* be
	/// safe to still use your favoirate compiler and maintain
	/// binary compatability with Spot.
{
public:
	IModuleTmpl(API::IEnvironmentBase* env = NULL) : _env(env) {};

	API::IEnvironmentBase& env() const
		/// Returns the Spot API interface pointer.
		/// Do not attempt to call inside constructor.
	{ 
		Poco::FastMutex::ScopedLock lock(_mutex);
		assert(_env);
		return *_env; 
	}

	API::IEnvironmentBase* _env;
		/// The Spot API base ABI agnostic interface pointer.
		///
		/// Note that the IEnvironmentBase pointer might be
		/// initialized as NULL. If the pointer is not passed
		/// into the constructor, it is guaranteed to be set
		/// by Spot directly after instantiation.

	virtual void log(const std::string& message, const char* level = "debug") 
		/// Sends log messages the Spot application logger.
	{
		if (_env == NULL) 
			return;
		_env->log(message.data(), level, className());
	}
	

	virtual const char* className() const { return "IModule"; }
		/// Override for named logging.
	
protected:
	virtual ~IModuleTmpl() = 0 {};
	mutable Poco::FastMutex	_mutex;	
};
*/

	


	

/*
// ---------------------------------------------------------------------
//
class IModule: public IModuleTmpl
{
public:
	IModule(API::IEnvironment* env = NULL) : env(env) {};
	virtual ~IModule() = 0 {};

	API::IEnvironment* env;
		/// The Spot API interface pointer.
};
*/
/*
#ifdef Anionu_Spot_ENABLE_ABI_COMPATABILITY
#include "Poco/Mutex.h"
#endif
*/

	/*
	
//protected:
	//API::IEnvironment* _env;
	//mutable Poco::FastMutex _mutex;
	*/
	
	/*
	/// This class is the unified interface from which all
	/// Spot classes inherit from. It ensures all classes
	/// have an IEnvironment reference and Logger access.


#define Anionu_Spot_ENABLE_ABI_COMPATABILITY
	/// This is to specify that we are creating an ABI compatible library.
	/// The current compiler must match the compiler used to create the original Spot binary.
	/// If the version and dependency check fail the build will fail with an error.

	static const bool isABICompatible =
#ifdef Anionu_Spot_ENABLE_ABI_COMPATABILITY
		false;
#else
		false;
#endif
		*/
	
	/*, _proc(NULL)
	

	//
	/// Private Methods
	//	

	
protected:

private:
	mutable Poco::FastMutex _mutex;

	IModule::IModule() : _env(NULL) {};

	virtual const char* className() const { return "Plugin"; }
		/// Override this method for named logging.
	
	API::IEnvironment& env() const
		/// Returns the Spot API interface pointer.
	{ 
		Poco::FastMutex::ScopedLock lock(_mutex);
		return *_env; 
	}

	std::string path() const
		/// Returns the full path to the library library.
	{ 
		Poco::FastMutex::ScopedLock lock(_mutex);
		return _path;
	}
	
	void setEnvironment(API::IEnvironment* env)
		/// The IEnvironment instance will be set by
		/// Spot before library initialization.
	{ 
		Poco::FastMutex::ScopedLock lock(_mutex);
		_env = env; 

		// Also set the default logger instance, otherwise
		// a new default logger instance will be created
		// for the library process.
		Logger::setInstance(&_env->logger());
	}

	void setPath(const char* path)
		/// The full path of the library will be set by
		/// Spot before library initialization.
	{ 
		Poco::FastMutex::ScopedLock lock(_mutex);
		_path = path; 
	}

	API::IFormProcessor* proc() const
		/// Returns the optional Symple Form processor pointer.
	{ 
		Poco::FastMutex::ScopedLock lock(_mutex);
		return _proc; 
	}
	*/
	/*
	void setFormProcessor(API::IFormProcessor* proc)
		/// The should be set during initialize() in the library
		/// is designed to intergrate with the online dashboard
		/// for remote configuration. 
	{ 
		Poco::FastMutex::ScopedLock lock(_mutex);
		_proc = proc; 
	}
	//API::IFormProcessor* _proc;
	*/