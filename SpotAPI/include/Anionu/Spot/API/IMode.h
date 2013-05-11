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


#ifndef ANIONU_SPOT_API_IMode_H
#define ANIONU_SPOT_API_IMode_H


#include "Sourcey/Base.h"
#include "Sourcey/Stateful.h"
#include "Sourcey/Symple/Form.h"
#include "Sourcey/Util/ScopedConfiguration.h"
#include "Anionu/Spot/API/IEnvironment.h"
//#include "Anionu/Spot/API/IFormProcessor.h"
#include "Anionu/Spot/API/IConfiguration.h"
#include "Anionu/Spot/API/IChannel.h"

#include "Poco/Net/NameValueCollection.h"
#include "Poco/Format.h"


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


class IMode: public StatefulSignal<ModeState>
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
		_proc(NULL),
		_config(env.config(), 
			//
			/// Cannel Scope: channels.[name].modes.[name].[value]
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
	
	virtual void initialize() { /* override me */ }
	virtual void uninitialize() { /* override me */ }
		/// If unrecoverable errors are encountered during
		/// the the initialization process, the mode state
		/// should be set to Error and an Exception thrown.

	virtual void activate()
	{
		// override me, calling base method
		setState(this, ModeState::Active);
	}

	virtual void deactivate()
	{
		// override me, calling base method
		setState(this, ModeState::Inactive);
	}
	

	//
	/// Accessors
	//

	std::string name() const
	{ 
		Poco::FastMutex::ScopedLock lock(_mutex);	
		return _name; 
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
	/// Mode data manipulation
	//

	void setData(const std::string& name, const std::string& value)
	{
		{
			Poco::FastMutex::ScopedLock lock(_mutex);	
			_data[name] = value;
		}
		ModeDataChanged.emit(this, _data);
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
		ModeDataChanged.emit(this, data());
	}

	void clearData()
	{
		{
			Poco::FastMutex::ScopedLock lock(_mutex);	
			_data.clear();
		}
		ModeDataChanged.emit(this, data());
	}

	StringMap data() const
	{ 
		Poco::FastMutex::ScopedLock lock(_mutex);	
		return _data; 
	}
	
	Signal<const StringMap&> ModeDataChanged;
		/// Signals when mode data changes.
		
	
	//
	/// Remote configuration
	//
	
	Symple::IFormProcessor* proc() const
		/// Returns the optional Symple Form processor pointer.
	{ 
		Poco::FastMutex::ScopedLock lock(_mutex);
		return _proc; 
	}

	void setFormProcessor(Symple::IFormProcessor* proc)
		/// The should be set during initialize() in the mode
		/// is designed to intergrate with the online dashboard
		/// for remote configuration. 
	{ 
		Poco::FastMutex::ScopedLock lock(_mutex);
		_proc = proc; 
	}


	//
	/// Logging and help
	//

	virtual std::string documentFile() const { return ""; }
		/// Returns the relative path (from the Spot directory) to the optional
		/// information file pertaining to this configurable module. 
		/// Information files should contain configuration assistance, 
		/// and are in Markdown format.

	LogStream log(const char* level = "debug") const
		/// This method sends log messages the Spot logger.
	{ 
		Poco::FastMutex::ScopedLock lock(_mutex);
		return _env.logger().send(level, this, className()); 
	}
	
	virtual const char* className() const { return "Mode"; }

protected:		
	mutable Poco::FastMutex	_mutex;	
	API::IEnvironment&	_env;
	API::IChannel&		_channel;
	ScopedConfiguration	_config;
	ModeOptions			_options;
	Symple::IFormProcessor* _proc;
	std::string			_name;
	StringMap			_data;
}; 


typedef std::vector<IMode*> IModeList;


} } } } // namespace Scy::Anionu::Spot::API


#endif // ANIONU_SPOT_API_IMode_H