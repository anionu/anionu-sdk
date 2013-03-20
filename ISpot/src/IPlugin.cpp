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


#include "Sourcey/Spot/IPlugin.h"


using namespace std;
using namespace Poco;


namespace Sourcey {
namespace Spot {


IPlugin::IPlugin() : 
	_env(NULL)
{
}


IPlugin::~IPlugin() 
{	
}


IEnvironment& IPlugin::env() const 
{ 
	return *_env; 
}

	
void IPlugin::setEnvironment(IEnvironment* env) 
{ 
	_env = env; 

	// Set the default logger instance.
	//Logger::uninitialize();
	//Logger::instance().add(&_env->logger().getDefault());
	Logger::setInstance(&_env->logger());
}


std::string IPlugin::path() const 
{ 
	return _path;
}


void IPlugin::setPath(const std::string& path) 
{ 
	_path = path; 
}


LogStream IPlugin::log(const char* level) const 
{ 
	// The Environment instance will be NULL until
	// the constructor has been called, therefore
	// messages logged inside the constructor will
	// be lost.
	if (_env == NULL)
		return LogStream();
	
	return _env->logger().send(level, this); 
	//LogStream stream(_env->logger().send(level));
	//printLog(stream);
	//return stream;
}


/*
void IPlugin::printLog(LogStream& ost) const
{
	ost << "["
		<< name()
		<< ":"
		<< this
		<< "] ";
}
*/


} } // namespace Sourcey::Spot