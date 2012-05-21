#include "SurveillanceModePlugin.h"
#include "SurveillanceMode.h"

#include "Sourcey/Spot/IEnvironment.h"
#include "Sourcey/Spot/IModeRegistry.h"

#include "Poco/ClassLibrary.h"


using namespace std;
using namespace Poco;
using namespace Sourcey;
using namespace Sourcey::Spot;


POCO_BEGIN_MANIFEST(IPlugin)
	POCO_EXPORT_CLASS(SurveillanceModePlugin)
POCO_END_MANIFEST


namespace Sourcey {
namespace Spot {


SurveillanceModePlugin::SurveillanceModePlugin()
{
}


SurveillanceModePlugin::~SurveillanceModePlugin()
{
	log() << "Destroying" << endl;
}


void SurveillanceModePlugin::initialize() 
{
	log() << "Initializing" << endl;	

	env().modes().registerMode<SurveillanceMode>("Surveillance Mode");
}


void SurveillanceModePlugin::uninitialize() 
{	
	log() << "Uninitializing" << endl;

	env().modes().unregisterMode("Surveillance Mode");
}


} } // namespace Sourcey::Spot
