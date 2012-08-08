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


#ifndef ANIONU_SPOT_IMode_H
#define ANIONU_SPOT_IMode_H


#include "Sourcey/Base.h"
#include "Sourcey/Stateful.h"
#include "Sourcey/IStartable.h"
#include "Sourcey/Spot/IModule.h"
#include "Sourcey/Spot/IConfigurable.h"
#include "Sourcey/Symple/Form.h"

#include "Poco/Net/NameValueCollection.h"

#include <map>
#include <list>
#include <iostream>


namespace Sourcey {
namespace Spot {


class IMode;
class IChannel;
class IEnvironment;


class ModeConfiguration 
	/// ModeConfiguration enables modes to access config values at
	/// their channel specific namespace falling back to mode defaults.
	/// Default: channels.[name].modes.[name].[value]
	/// Root: modes.[name].[value]
{	
public:
	ModeConfiguration(IMode& mode);
	ModeConfiguration(const ModeConfiguration& r);

	std::string getString(const std::string& key, const std::string& defaultValue) const;
	int getInt(const std::string& key, int defaultValue) const;
	double getDouble(const std::string& key, double defaultValue) const;
	bool getBool(const std::string& key, bool defaultValue) const;

	void setString(const std::string& key, const std::string& value, bool baseScope = false);
	void setInt(const std::string& key, int value, bool baseScope = false);
	void setDouble(const std::string& key, double value, bool baseScope = false);
	void setBool(const std::string& key, bool value, bool baseScope = false);
	
	std::string getDefaultScope(const std::string& key) const;
	std::string getBaseScope(const std::string& key) const;
	std::string getScoped(const std::string& key, bool baseScope = false) const;

	IMode& mode;
	std::string defaultScope;
	std::string baseScope;
};


typedef Poco::Net::NameValueCollection ModeOptions;
typedef std::map<std::string, IMode*> IModeMap;


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


class IMode: public IModule, public IConfigurable, public StatefulSignal<ModeState>
	/// This class defines a specific operational mode for a
	/// Spot Channel.
{	
public:
	IMode(IEnvironment& env, IChannel& channel, const std::string& name);	
	virtual ~IMode();
	
	virtual void initialize();
	virtual void uninitialize();
		// If unrecoverable errors are encountered
		// during the initialization process, the
		// mode state should be set to Error and
		// an Exception thrown.

	virtual void activate();
	virtual void deactivate();

	virtual std::string name() const;
	virtual IChannel& channel();
	virtual ModeConfiguration& config();
	virtual ModeOptions& options();
	
	virtual bool isActive() const { return stateEquals(ModeState::Active); };
	virtual bool isError() const { return stateEquals(ModeState::Error); };

	virtual void setData(const std::string& name, const std::string& value);
	virtual void removeData(const std::string& name);
	virtual void clearData();
	virtual StringMap data() const;;
	
	Signal<const StringMap&> ModeDataChanged;
	
	virtual const char* className() const { return "Mode"; }

protected:	
	mutable Poco::FastMutex	_mutex;	

	IChannel&			_channel;
	ModeConfiguration	_config;
	ModeOptions			_options;
	std::string			_name;
	StringMap			_data;

	//friend class ModeConfiguration;
}; 


} } // namespace Sourcey::Spot


#endif // ANIONU_SPOT_IMode_H