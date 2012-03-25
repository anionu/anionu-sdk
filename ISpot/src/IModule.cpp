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
	return _env.logger().send(this, level); 
}		


} } // namespace Sourcey::Spot