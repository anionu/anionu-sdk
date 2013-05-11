#include "SurveillanceModePlugin.h"
#include "SurveillanceMode.h"

#include "Anionu/Spot/API/IEnvironment.h"
#include "Anionu/Spot/API/ModeRegistry.h"


using namespace std;


DEFINE_SPOT_PLUGIN(Scy::Anionu::Spot::SurveillanceModePlugin)


namespace Scy {
namespace Anionu { 
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

	env()->modes().registerMode<SurveillanceMode>("Surveillance Mode");
}


void SurveillanceModePlugin::uninitialize() 
{	
	log() << "Uninitializing" << endl;

	env()->modes().unregisterMode("Surveillance Mode");
}


} } } // namespace Scy::Anionu::Spot
