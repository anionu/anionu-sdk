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


#ifndef ANIONU_SPOT_IPlugin_H
#define ANIONU_SPOT_IPlugin_H


#include "Sourcey/Stateful.h"
#include "Sourcey/Spot/IEnvironment.h"
#include "Sourcey/Spot/IConfigurable.h"

#include <string>


namespace Sourcey {
namespace Spot {


class IPlugin: public IConfigurable, public IPolymorphic
	/// A plugin is a runtime module that may be loaded to extend 
	/// the core functionality of Spot.
	/// An IEnvironment instance is available to the plugin before
	/// initialization which exposes the ISpot API and class tree
	/// to the plugin instance.
{
public:
	IPlugin();
	virtual ~IPlugin();
	
	virtual void initialize() = 0;
		/// If the plugin needs to run any system compatibility
		/// or runtime checks they should be done here. 
		/// An Exception with a descriptive message should be
		/// thrown on failure to notify Spot that the plugin
		/// has failed.

	virtual void uninitialize() = 0;

	virtual IEnvironment& env() const;
	virtual void setEnvironment(IEnvironment* env);
		/// The IEnvironment instance will be set by Spot 
		/// before plugin initialization.

	virtual std::string path() const;
	virtual void setPath(const std::string& path);
		/// The system path of the plugin will be set by
		/// Spot before plugin initialization.
		
	virtual LogStream log(const char* level = "debug") const;
		/// This method sends log messages the Spot logger.
	
	virtual const char* className() const { return "Plugin"; }

protected: 
	IEnvironment* _env;
	std::string _path;
};


} } /// namespace Sourcey::Spot


#endif /// ANIONU_SPOT_IPlugin_H

	


	//virtual void printLog(LogStream& ost) const;	

	//virtual std::string name() const = 0;
		/// The display name of this plugin.
		/// Other information is contained within manifest.json
//class IEnvironment;

/*
*/
	//, public Stateful<PluginState> //public IModule, 

	//virtual void enable() = 0;
	//virtual void disable() = 0;
	
	//virtual std::string title() const = 0;
	//virtual std::string author() const = 0;
	//virtual std::string version() const = 0;
	//virtual bool isDisabled() const { return stateEquals(PluginState::Disabled); };
	//virtual bool isEnabled() const { return stateEquals(PluginState::Enabled); };
	//virtual bool isError() const { return stateEquals(PluginState::Error); };

	/*//, public IStartable
	enum State 
	{
		None = 0,
		//Initialized,
		Disabled,
		Enabled,
		Error,
	}; : _state(None)
	*/
	//State _state;

	//virtual State state() const { return _state; };
	//virtual void setState(const State& state) { _state = state; };
	
	//virtual bool isInitialized() const { return _state == Initialized; };
	//virtual bool isEnabled() const { return _state == Enabled; };
	//virtual bool isError() const { return _state == Error; };