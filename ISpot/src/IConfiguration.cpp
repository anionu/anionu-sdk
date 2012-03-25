#include "Sourcey/Spot/IConfiguration.h"


using namespace std;
using namespace Poco;


namespace Sourcey { 
namespace Spot {


IConfiguration::IConfiguration(IEnvironment& env) :
	IModule(env)
{	
}


IConfiguration::~IConfiguration()
{
}


void IConfiguration::setRaw(const string& key, const string& value)
{	
	//log() << "Set Raw: " << key << ": " << value << endl;
	JSON::Configuration::setRaw(key, value);
	ConfigurationChanged.dispatch(this, key, value);
}


} } // namespace Sourcey::Spot