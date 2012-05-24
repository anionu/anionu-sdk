#include "Sourcey/Spot/IConfigurable.h"
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


IConfigurable::IConfigurable()
{
}


IConfigurable::~IConfigurable()
{
}


bool IConfigurable::isConfigurable() const 
{
	return false;
}


bool IConfigurable::hasParsableConfig(Symple::Form& form) const
{
	return isConfigurable();
}


void IConfigurable::buildConfigForm(Symple::Form& form, Symple::FormElement& element, bool useBase) 
{	
}


void IConfigurable::parseConfigForm(Symple::Form& form, Symple::FormElement& element)
{
}


void IConfigurable::printInformation(ostream& ost)
{
	ost << "No information is available for this module." << endl;
}


} } // namespace Sourcey::Spot