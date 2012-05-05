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
	Logger::uninitialize();
	Logger::instance().add(&_env->logger().getDefault());
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
	
	return _env->logger().send(this, level); 
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