#include "Sourcey/Spot/IModule.h"
#include "Sourcey/Spot/IEnvironment.h"


using namespace std;
using namespace Poco;


namespace Sourcey {
namespace Spot {


IModule::IModule(IEnvironment& env) : 
	_env(env)
{
}


IModule::~IModule()
{
}


LogStream IModule::log(const char* level) const 
{ 
	FastMutex::ScopedLock lock(_mutex);
	return _env.logger().send(this, level); 
}	


/*
IEnvironment& IModule::env() 
{ 
	FastMutex::ScopedLock lock(_mutex);
	return _env; 
}	
*/


} } // namespace Sourcey::Spot