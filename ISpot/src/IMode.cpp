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


#include "Sourcey/Spot/IMode.h"
#include "Sourcey/Spot/IChannel.h"
#include "Sourcey/Spot/IEnvironment.h"
#include "Sourcey/Spot/IConfiguration.h"
#include "Sourcey/Symple/Form.h"
#include "Sourcey/Spot/IModule.h"
#include "Poco/Format.h"


using namespace std;
using namespace Poco;


namespace Sourcey {
namespace Spot {


IMode::IMode(IEnvironment& env, IChannel& channel, const string& name) : 
	IModule(env), 
	_channel(channel), 
	_name(name), 
	_config(*this)
{
	assert(!_name.empty());

	//
	// Default Scope: channels.[name].modes.[name].[value]
	//
	_config.defaultScope = format("channels.%s.modes.%s.", _channel.name(), _name);
	
	//
	// Base Scope: modes.[name].[value]
	//
	_config.baseScope = format("modes.%s.", _name);
}


IMode::~IMode()
{
}


void IMode::initialize()
{
}


void IMode::uninitialize()
{
}


void IMode::activate()
{
	setState(this, ModeState::Active);
}


void IMode::deactivate()
{
	setState(this, ModeState::Inactive);
}


void IMode::setData(const string& name, const string& value)
{
	{
		FastMutex::ScopedLock lock(_mutex);	
		_data[name] = value;
	}
	ModeDataChanged.dispatch(this, _data);
}


void IMode::removeData(const string& name) 
{
	{
		FastMutex::ScopedLock lock(_mutex);	
		StringMap::iterator it = _data.find(name);
		if (it != _data.end()) {
			_data.erase(it);
		}
	}
	ModeDataChanged.dispatch(this, data());
}


void IMode::clearData() 
{
	{
		FastMutex::ScopedLock lock(_mutex);	
		_data.clear();
	}
	ModeDataChanged.dispatch(this, data());
}


StringMap IMode::data() const 
{ 
	FastMutex::ScopedLock lock(_mutex);	
	return _data; 
}



string IMode::name() const		
{ 
	FastMutex::ScopedLock lock(_mutex);	
	return _name; 
}


IChannel& IMode::channel() 			
{ 
	FastMutex::ScopedLock lock(_mutex);	
	return _channel; 
}


ModeConfiguration& IMode::config() 	
{ 
	FastMutex::ScopedLock lock(_mutex);	
	return _config;
}


ModeOptions& IMode::options()  		
{ 
	FastMutex::ScopedLock lock(_mutex);	
	return _options; 
}


/*
void IMode::buildConfigForm(Symple::Form& form, Symple::FormElement& element, bool baseScope) 
{	
	XMPP::FormField field = form.addField("fixed");	
	field.addValue("No configuration options are available.");		
}


void IMode::parseConfigForm(Symple::Form& form, Symple::FormElement& element, StringMap& errors) 
{
}


void IMode::printInformation(ostream& s)
{
	s << _name << ": Sorry, no help is available.";
}
*/


// ---------------------------------------------------------------------
//
ModeConfiguration::ModeConfiguration(IMode& mode) :
	mode(mode)
{
}


ModeConfiguration::ModeConfiguration(const ModeConfiguration& r) :
	mode(r.mode),
	defaultScope(r.defaultScope),
	baseScope(r.baseScope)
{
}
	

string ModeConfiguration::getString(const string& key, const string& defaultValue) const 
{
	return mode.env().config().getString(getDefaultScope(key), 
		mode.env().config().getString(getBaseScope(key), defaultValue));
}


int ModeConfiguration::getInt(const string& key, int defaultValue) const 
{
	return mode.env().config().getInt(getDefaultScope(key), 
		mode.env().config().getInt(getBaseScope(key), defaultValue));
}


double ModeConfiguration::getDouble(const string& key, double defaultValue) const 
{
	return mode.env().config().getDouble(getDefaultScope(key), 
		mode.env().config().getDouble(getBaseScope(key), defaultValue));
}


bool ModeConfiguration::getBool(const string& key, bool defaultValue) const 
{
	return mode.env().config().getBool(getDefaultScope(key), 
		mode.env().config().getBool(getBaseScope(key), defaultValue));
}


void ModeConfiguration::setString(const string& key, const string& value, bool baseScope) 
{
	mode.env().config().setString(getScoped(key, baseScope), value);
}


void ModeConfiguration::setInt(const string& key, int value, bool baseScope) 
{	
	mode.env().config().setInt(getScoped(key, baseScope), value);
}


void ModeConfiguration::setDouble(const string& key, double value, bool baseScope) 
{
	mode.env().config().setDouble(getScoped(key, baseScope), value);
}


void ModeConfiguration::setBool(const string& key, bool value, bool baseScope) 
{
	mode.env().config().setBool(getScoped(key, baseScope), value);
}


string ModeConfiguration::getDefaultScope(const string& key) const
{	
	//if (defaultScope.empty())
	return defaultScope + key;
}


string ModeConfiguration::getBaseScope(const string& key) const
{
	//if (baseScope.empty())
	return baseScope + key;
}


string ModeConfiguration::getScoped(const string& key, bool baseScope) const
{
	return baseScope ? getBaseScope(key) : getDefaultScope(key);
}


} } // namespace Sourcey::Spot