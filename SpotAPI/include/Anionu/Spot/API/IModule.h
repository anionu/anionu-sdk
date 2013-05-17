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

#ifdef ENFORCE_STRICT_ABI_COMPATABILITY
#include "Anionu/Spot/API/IEnvironment.h"
#include "Sourcey/Logger.h"
#include "Poco/Mutex.h"
#endif


namespace Scy {
namespace Anionu {
namespace Spot { 
namespace API { 

		
class IModuleBase
	/// ABI agnostic API
	///
	/// Spot's base API is ABI agnostic, so it *should* be
	/// safe to still use your favoirate compiler and maintain
	/// binary compatability with Spot.
{
public:
	IModuleBase(API::IEnvironmentBase* env = NULL) : env(env) {};

	API::IEnvironmentBase* env;
		/// The Spot API base interface pointer.
		///
		/// Take care as the IEnvironmentBase pointer may be
		/// initialized as NULL. If not passed, it is guaranteed
		/// to be set by the outside application directly after
		/// instation.

	const char* className() const { return "IModule"; }
		/// Override for named logging.
	
protected:
	virtual ~IModuleBase() = 0 {};
};


// ---------------------------------------------------------------------
//
#ifdef ENFORCE_STRICT_ABI_COMPATABILITY
	
class IModule: public IModuleBase
{
public:
	IModule(API::IEnvironment* env = NULL) : env(env) {};
	virtual ~IModule() = 0 {};

	API::IEnvironment* env;
	
	LogStream log(const char* level = "debug") const
		/// This method sends log messages the Spot logger.
	{ 
		return env->logger().send(level, this, className()); 
	}
};


#endif /// ENFORCE_STRICT_ABI_COMPATABILITY


} } } } // namespace Scy::Anionu::Spot::API


#endif /// Anionu_Spot_API_IModule_H


	


/*
// ---------------------------------------------------------------------
//
class IDataModule
{	
public:
	void setData(const std::string& name, const std::string& value)
	{
		{
			Poco::FastMutex::ScopedLock lock(_mutex);	
			_data[name] = value;
		}
		DataChanged.emit(this, _data);
	}

	void removeData(const std::string& name)
	{
		{
			Poco::FastMutex::ScopedLock lock(_mutex);	
			StringMap::iterator it = _data.find(name);
			if (it != _data.end()) {
				_data.erase(it);
			}
		}
		DataChanged.emit(this, data());
	}

	void clearData()
	{
		{
			Poco::FastMutex::ScopedLock lock(_mutex);	
			_data.clear();
		}
		DataChanged.emit(this, data());
	}

	StringMap data() const
	{ 
		Poco::FastMutex::ScopedLock lock(_mutex);	
		return _data; 
	}
	
	Signal<const StringMap&> DataChanged;
		/// Signals when data changes.

protected:		
	mutable Poco::FastMutex	_mutex;	
	StringMap _data;
}; 
*/

	/*
	
//protected:
	//API::IEnvironment* _env;
	//mutable Poco::FastMutex _mutex;
	API::IEnvironment& env() const
		/// Returns the Spot API interface pointer.
		/// Do not attempt to call inside constructor.
	{ 
		Poco::FastMutex::ScopedLock lock(_mutex);
		assert(_env);
		return *_env; 
	}
	
	void setEnvironment(API::IEnvironment* env)
		/// The IEnvironment instance will be set by 
		/// Spot after the module is instantiated.
	{ 
		Poco::FastMutex::ScopedLock lock(_mutex);
		_env = env; 

		// Set the default logger instance, otherwise
		// a new default logger instance will be
		// created for the plugin process.
		//Logger::setInstance(&_env->logger());
	}
	*/
	
	/*
	/// This class is the unified interface from which all
	/// Spot classes inherit from. It ensures all classes
	/// have an IEnvironment reference and Logger access.


#define ENFORCE_STRICT_ABI_COMPATABILITY
	/// This is to specify that we are creating an ABI compatible library.
	/// The current compiler must match the compiler used to create the original Spot binary.
	/// If the version and dependency check fail the build will fail with an error.

	static const bool isABICompatible =
#ifdef ENFORCE_STRICT_ABI_COMPATABILITY
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