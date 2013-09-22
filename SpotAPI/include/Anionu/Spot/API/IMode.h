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


#ifndef Anionu_Spot_API_IMode_H
#define Anionu_Spot_API_IMode_H


#include "Anionu/Spot/API/Config.h"

#ifdef Anionu_Spot_USING_CORE_API
#include "Sourcey/Signal.h"
#endif


namespace scy {
namespace anio {
namespace spot { 
namespace api { 


class IMode
	/// This class defines the ABI agnostic virtual interface
	/// for all IMode implementations.
{
public:
	virtual ~IMode() = 0
	
	virtual bool activate() = 0;
	virtual void deactivate() = 0;
		// Activation logic should be implemented here.
		// If there was an error, activate() hould return false.

	virtual bool isActive() const = 0;
		// Returns true if the mode is currently active.
	
	virtual const char* modeName() const = 0;
		// Returns the name of this mode.
		// Alpha numerics and spaces allowed.
	
	virtual const char* channelName() const = 0;
		// Returns the channel name this mode is activated on.

	virtual const char* errorMessage() const { return 0; };
		// Returns a detailed error message if initialize() or 
		// activate() fails.
	
	virtual const char* docFile() { return 0; };
		// Returns the relative path (from the Spot binary dir)
		// to the optional help guide/documentation pertaining
		// to the current module.
		// Information files should be in Markdown format.
};


// ---------------------------------------------------------------------
//
class IModeFactory
{
public:
	virtual IMode* createModeInstance(const char* modeName, const char* channelName) = 0;
		// Instantiates the given mode for the given channel.
		// The channel name should be passed to the IMode instance
		// via the constructor.

	virtual const char** modeNames() const = 0;
		// Returns a NULL terminated array of modes names 
		// implemented by this plugin.
};


#ifdef Anionu_Spot_USING_CORE_API

// ---------------------------------------------------------------------
//
class IDataMode
{	
public:
	typedef std::map<std::string, std::string> DataMap;

	virtual ~IDataMode() = 0;

	void setData(const std::string& name, const std::string& value)
	{
		{
			Mutex::ScopedLock lock(_mutex);	
			_data[name] = value;
		}
		DataChanged.emit(this, _data);
	}

	void removeData(const std::string& name)
	{
		{
			Mutex::ScopedLock lock(_mutex);	
			DataMap::iterator it = _data.find(name);
			if (it != _data.end()) {
				_data.erase(it);
			}
		}
		DataChanged.emit(this, data());
	}

	void clearData()
	{
		{
			Mutex::ScopedLock lock(_mutex);	
			_data.clear();
		}
		DataChanged.emit(this, data());
	}

	DataMap data() const
	{ 
		Mutex::ScopedLock lock(_mutex);	
		return _data; 
	}
	
	Signal<const DataMap&> DataChanged;
		// Signals the outside application when
		// internal mode data changes.

protected:		
	mutable Mutex	_mutex;	
	DataMap _data;
}; 

#endif /// Anionu_Spot_USING_CORE_API


} } } } // namespace scy::anio::spot::api


#endif // Anionu_Spot_API_IMode_H






/*
	
	//virtual bool initialize() = 0;
	//virtual void uninitialize() = 0;
		// Initialization logic should be implemented here.
#ifdef Anionu_Spot_USING_CORE_API
#include "Sourcey/Base.h"
#include "Sourcey/Stateful.h"
#include "Sourcey/Configuration.h"
#include "Anionu/Spot/API/Environment.h"
#include "Anionu/Spot/API/Channel.h"
//#include "Poco/Net/NVCollection.h"
#include "Poco/Format.h"
#endif
*/


/*const char* channel
		// See IMode for implementation specifics.
//namespace smpl {
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
typedef NVCollection ModeOptions;
*/
	
/*
#ifdef Anionu_Spot_USING_CORE_API


class IMode: public IMode//, public Stateful<ModeState>
	/// The IMode interface defines a specific operational mode
	/// for a Spot channel that can be activated and deactivated.
	/// See the Surveillance Mode and Recording Mode plugins for
	/// implementation ideas.
{	
public:
	IMode(api::Environment& env, api::Channel& channel, const std::string& name) : 
		_env(env), 
		_channel(channel), 
		_name(name), 
		_config(env.config(), 
			//
			// Channel Scope: channels.[name].modes.[name].[value]
			Poco::format("channels.%s.modes.%s.", channel.name(), name),
			//
			// Base Scope: modes.[name].[value]
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
		// If unrecoverable errors are encountered during
		// the the initialization process, the mode state
		// should be set to Error and an Exception thrown.

	virtual void activate()
	{
		// override me, calling base method
		//setState(/this, /ModeState::Active);
	}

	virtual void deactivate()
	{
		// override me, calling base method
		//setState(/this, /ModeState::Inactive);
	}
	

	//
	/// Accessors
	//

	const char* name() const
	{ 
		Mutex::ScopedLock lock(_mutex);	
		return _name.c_str(); 
	}
	
	api::Environment& env() const
	{ 
		Mutex::ScopedLock lock(_mutex);	
		return _env; 
	}

	api::Channel& channel() const
	{ 
		Mutex::ScopedLock lock(_mutex);	
		return _channel; 
	}
	
	ScopedConfiguration& config()
	{ 
		Mutex::ScopedLock lock(_mutex);	
		return _config;
	}

	ModeOptions& options()
	{ 
		Mutex::ScopedLock lock(_mutex);	
		return _options; 
	}
	
	bool isActive() const { return stateEquals(ModeState::Active); }
	bool isError() const { return stateEquals(ModeState::Error); }

	//
	/// Logging and help
	//

	LogStream& log(const char* level = "debug") const
		// This method sends log messages the Spot application logger.
	{ 
		return env().logger().send(level, this, className()); 
	}
	
	virtual const char* className() const { return "Mode"; }

protected:		
	mutable Mutex	_mutex;	
	api::Environment&	_env;
	api::Channel&		_channel;
	//ScopedConfiguration	_config;
	//ModeOptions			_options;
	std::string			_name;
	//DataMap			_data;
}; 


#endif /// Anionu_Spot_USING_CORE_API

	*/

	
	/*
	virtual std::string docFile() const { return ""; }
		// Returns the relative path (from the Spot directory) to the optional
		// information file pertaining to this configurable module. 
		// Information files should contain configuration assistance, 
		// and are in Markdown format.

	api::IFormProcessor* proc() const
		// Returns the optional Symple Form processor pointer.
	{ 
		Mutex::ScopedLock lock(_mutex);
		return _proc; 
	}

	void setFormProcessor(api::IFormProcessor* proc)
		// The should be set during initialize() in the mode
		// is designed to intergrate with the online dashboard
		// for remote configuration. 
	{ 
		Mutex::ScopedLock lock(_mutex);
		_proc = proc; 
	}
		_proc(null),
	api::IFormProcessor* _proc;
	*/