#include "SurveillanceModePlugin.h"
#include "SurveillanceMode.h"

#include "Anionu/Spot/API/IEnvironment.h"


using namespace std;


DEFINE_SPOT_PLUGIN(scy::anionu::spot::SurveillanceModePlugin)


namespace scy {
namespace anionu { 
namespace spot {


SurveillanceModePlugin::SurveillanceModePlugin()
{
}


SurveillanceModePlugin::~SurveillanceModePlugin()
{
}


bool SurveillanceModePlugin::load() 
{
	log("Loading");	
	return true;
}


void SurveillanceModePlugin::unload() 
{	
	log("Unloading");	
}


api::IMode* SurveillanceModePlugin::createModeInstance(const char* modeName, const char* channelName)
{
	assert(strcmp(modeName, "Surveillance Mode") == 0);
	return new SurveillanceMode(*env(), channelName);
}


const char** SurveillanceModePlugin::modeNames() const
{
	static const char* modeNames[] = { "Surveillance Mode", NULL };
	return modeNames;
}

} } } // namespace scy::anionu::Spot
