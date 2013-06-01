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


#ifndef Anionu_Spot_API_IMode_H
#define Anionu_Spot_API_IMode_H


#include "Anionu/Spot/API/Config.h"

#ifdef Anionu_Spot_ENABLE_ABI_COMPATABILITY
#include "Sourcey/Signal.h"
#endif


namespace Scy {
namespace Anionu {
namespace Spot { 
namespace API { 


class IMode
	/// This class defines the ABI agnostic virtual interface
	/// for all IMode implementations.
{
public:
	virtual ~IMode() = 0 {}
	
	virtual bool activate() = 0;
	virtual void deactivate() = 0;
		/// Activation logic should be implemented here.
		/// If there was an error, activate() hould return false.

	virtual bool isActive() const = 0;
		/// Returns true if the mode is currently active.
	
	virtual const char* modeName() const = 0;
		/// Returns the name of this mode.
		/// Alpha numerics and spaces allowed.
	
	virtual const char* channelName() const = 0;
		/// Returns the channel name this mode is activated on.

	virtual const char* errorMessage() const { return 0; };
		/// Returns a detailed error message if initialize() or 
		/// activate() fails.
	
	virtual const char* docFile() { return 0; };
		/// Returns the relative path (from the Spot binary dir)
		/// to the optional help guide/documentation pertaining
		/// to the current module.
		/// Information files should be in Markdown format.
};


// ---------------------------------------------------------------------
//
class IModeFactory
{
public:
	virtual IMode* createModeInstance(const char* modeName, const char* channelName) = 0;
		/// Instantiates the given mode for the given channel.
		/// The channel name should be passed to the IMode instance
		/// via the constructor.

	virtual const char** modeNames() const = 0;
		/// Returns a NULL terminated array of modes names 
		/// implemented by this plugin.
};


#ifdef Anionu_Spot_ENABLE_ABI_COMPATABILITY

// ---------------------------------------------------------------------
//
class IDataMode
{	
public:
	virtual ~IDataMode() = 0 {};

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
		/// Signals the outside application when
		/// internal mode data changes.

protected:		
	mutable Poco::FastMutex	_mutex;	
	StringMap _data;
}; 

#endif /// Anionu_Spot_ENABLE_ABI_COMPATABILITY


} } } } // namespace Scy::Anionu::Spot::API


#endif // Anionu_Spot_API_IMode_H






/*
	
	//virtual bool initialize() = 0;
	//virtual void uninitialize() = 0;
		/// Initialization logic should be implemented here.
#ifdef Anionu_Spot_ENABLE_ABI_COMPATABILITY
#include "Sourcey/Base.h"
#include "Sourcey/Stateful.h"
#include "Sourcey/IConfiguration.h"
#include "Anionu/Spot/API/IEnvironment.h"
#include "Anionu/Spot/API/IChannel.h"
#include "Poco/Net/NameValueCollection.h"
#include "Poco/Format.h"
#endif
*/


/*const char* channel
		/// See IMode for implementation specifics.
//namespace Symple {
	//class IFormProcessor;
	/// The symple form processor can be optionally included
	/// for integrating your plugin with the Anionu dashboard
	/// for remote configuration.
//}


TODO: Move to mode wrapper
struct ModeState: public State 
{
	enum Type
	{
		None = 0,
		Inactive,
		Active,
		Error
	};

	std::string str(unsigned int id) const 
	{ 
		switch (id) {
		case None:		return "None";
		case Inactive:	return "Inactive";
		case Active:	return "Active";
		case Error:	    return "Error";
		}
		return "undefined"; 
	};
};	
typedef Poco::Net::NameValueCollection ModeOptions;
*/
	
/*
#ifdef Anionu_Spot_ENABLE_ABI_COMPATABILITY


class IMode: public IMode//, public StatefulSignal<ModeState>
	/// The IMode interface defines a specific operational mode
	/// for a Spot channel that can be activated and deactivated.
	/// See the Surveillance Mode and Recording Mode plugins for
	/// implementation ideas.
{	
public:
	IMode(API::IEnvironment& env, API::IChannel& channel, const std::string& name) : 
		_env(env), 
		_channel(channel), 
		_name(name), 
		_config(env.config(), 
			//
			/// Channel Scope: channels.[name].modes.[name].[value]
			Poco::format("channels.%s.modes.%s.", channel.name(), name),
			//
			/// Base Scope: modes.[name].[value]
			Poco::format("modes.%s.", name))
	{
		assert(!_name.empty());
	}

	virtual ~IMode()
	{
	}


	//
	/// Initializers
	//
	
	virtual void initialize() {}
	virtual void uninitialize() {}
		/// If unrecoverable errors are encountered during
		/// the the initialization process, the mode state
		/// should be set to Error and an Exception thrown.

	virtual void activate()
	{
		// override me, calling base method
		//setState(this, ModeState::Active);
	}

	virtual void deactivate()
	{
		// override me, calling base method
		//setState(this, ModeState::Inactive);
	}
	

	//
	/// Accessors
	//

	const char* name() const
	{ 
		Poco::FastMutex::ScopedLock lock(_mutex);	
		return _name.data(); 
	}
	
	API::IEnvironment& env() const
	{ 
		Poco::FastMutex::ScopedLock lock(_mutex);	
		return _env; 
	}

	API::IChannel& channel() const
	{ 
		Poco::FastMutex::ScopedLock lock(_mutex);	
		return _channel; 
	}
	
	ScopedConfiguration& config()
	{ 
		Poco::FastMutex::ScopedLock lock(_mutex);	
		return _config;
	}

	ModeOptions& options()
	{ 
		Poco::FastMutex::ScopedLock lock(_mutex);	
		return _options; 
	}
	
	bool isActive() const { return stateEquals(ModeState::Active); }
	bool isError() const { return stateEquals(ModeState::Error); }

	//
	/// Logging and help
	//

	LogStream log(const char* level = "debug") const
		/// This method sends log messages the Spot application logger.
	{ 
		return env().logger().send(level, this, className()); 
	}
	
	virtual const char* className() const { return "Mode"; }

protected:		
	mutable Poco::FastMutex	_mutex;	
	API::IEnvironment&	_env;
	API::IChannel&		_channel;
	//ScopedConfiguration	_config;
	//ModeOptions			_options;
	std::string			_name;
	//StringMap			_data;
}; 


#endif /// Anionu_Spot_ENABLE_ABI_COMPATABILITY

	*/

	
	/*
	virtual std::string docFile() const { return ""; }
		/// Returns the relative path (from the Spot directory) to the optional
		/// information file pertaining to this configurable module. 
		/// Information files should contain configuration assistance, 
		/// and are in Markdown format.

	API::IFormProcessor* proc() const
		/// Returns the optional Symple Form processor pointer.
	{ 
		Poco::FastMutex::ScopedLock lock(_mutex);
		return _proc; 
	}

	void setFormProcessor(API::IFormProcessor* proc)
		/// The should be set during initialize() in the mode
		/// is designed to intergrate with the online dashboard
		/// for remote configuration. 
	{ 
		Poco::FastMutex::ScopedLock lock(_mutex);
		_proc = proc; 
	}
		_proc(NULL),
	API::IFormProcessor* _proc;
	*/