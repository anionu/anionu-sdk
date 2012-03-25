#ifndef ANIONU_SPOT_IConfiguration_H
#define ANIONU_SPOT_IConfiguration_H


#include "Sourcey/Signal.h"
#include "Sourcey/JSON/Configuration.h"
#include "Sourcey/Spot/IModule.h"
#include "Sourcey/Symple/Form.h"


namespace Sourcey { 
namespace Spot {


class IConfiguration: public JSON::Configuration, public IModule
{
public:
	IConfiguration(IEnvironment& env);
	virtual ~IConfiguration();

	Signal2<const std::string&, const std::string&> ConfigurationChanged;
		/// The Key and Value of the changed configuration property.

	virtual const char* className() const { return "Configuration"; }

protected:
	virtual void setRaw(const std::string& key, const std::string& value);
		/// The base method gets called after the config value is
		/// updated to trigger the ConfigurationChanged callback.
};


} } // namespace Sourcey::Spot


#endif