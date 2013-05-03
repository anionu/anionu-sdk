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
	_config.channelScope = format("channels.%s.modes.%s.", _channel.name(), _name);
	
	//
	// Base Scope: modes.[name].[value]
	//
	_config.defaultScope = format("modes.%s.", _name);
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
	ModeDataChanged.emit(this, _data);
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
	ModeDataChanged.emit(this, data());
}


void IMode::clearData() 
{
	{
		FastMutex::ScopedLock lock(_mutex);	
		_data.clear();
	}
	ModeDataChanged.emit(this, data());
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


IChannel& IMode::channel() const
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
void IMode::buildConfigForm(Symple::Form& form, Symple::FormElement& element, bool defaultScope) 
{	
	XMPP::FormField field = form.addField("fixed");	
	field.addValue("No configuration options are available.");		
}


void IMode::parseConfigForm(Symple::Form& form, Symple::FormElement& element, StringMap& errors) 
{
}


void IMode::infoFile(ostream& s)
{
	s << _name << ": Sorry, no help is available.";
}
*/


// ---------------------------------------------------------------------
ModeConfiguration::ModeConfiguration(IMode& mode) :
	mode(mode)
{
}


ModeConfiguration::ModeConfiguration(const ModeConfiguration& r) :
	mode(r.mode),
	channelScope(r.channelScope),
	defaultScope(r.defaultScope)
{
}
	

string ModeConfiguration::getString(const string& key, const string& defaultValue, bool forceDefaultScope) const 
{
	return forceDefaultScope ? mode.env().config().getString(getDafaultKey(key), defaultValue) :
		mode.env().config().getString(getChannelKey(key), 
			mode.env().config().getString(getDafaultKey(key), defaultValue));
}


int ModeConfiguration::getInt(const string& key, int defaultValue, bool forceDefaultScope) const 
{
	return forceDefaultScope ? mode.env().config().getInt(getDafaultKey(key), defaultValue) :
		mode.env().config().getInt(getChannelKey(key), 
			mode.env().config().getInt(getDafaultKey(key), defaultValue));
}


double ModeConfiguration::getDouble(const string& key, double defaultValue, bool forceDefaultScope) const 
{
	return forceDefaultScope ? mode.env().config().getDouble(getDafaultKey(key), defaultValue) :
		mode.env().config().getDouble(getChannelKey(key), 
			mode.env().config().getDouble(getDafaultKey(key), defaultValue));
}


bool ModeConfiguration::getBool(const string& key, bool defaultValue, bool forceDefaultScope) const 
{
	return forceDefaultScope ? mode.env().config().getBool(getDafaultKey(key), defaultValue) :
		mode.env().config().getBool(getChannelKey(key), 
			mode.env().config().getBool(getDafaultKey(key), defaultValue));
}


void ModeConfiguration::setString(const string& key, const string& value, bool defaultScope) 
{
	mode.env().config().setString(getScopedKey(key, defaultScope), value);
}


void ModeConfiguration::setInt(const string& key, int value, bool defaultScope) 
{	
	mode.env().config().setInt(getScopedKey(key, defaultScope), value);
}


void ModeConfiguration::setDouble(const string& key, double value, bool defaultScope) 
{
	mode.env().config().setDouble(getScopedKey(key, defaultScope), value);
}


void ModeConfiguration::setBool(const string& key, bool value, bool defaultScope) 
{
	mode.env().config().setBool(getScopedKey(key, defaultScope), value);
}


string ModeConfiguration::getChannelKey(const string& base) const
{	
	//if (channelScope.empty())
	return channelScope + base;
}


string ModeConfiguration::getDafaultKey(const string& base) const
{
	//if (defaultScope.empty())
	return defaultScope + base;
}


string ModeConfiguration::getScopedKey(const string& base, bool defaultScope) const
{
	return defaultScope ? getDafaultKey(base) : getChannelKey(base);
}


} } // namespace Sourcey::Spot