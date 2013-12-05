#include "SurveillanceModePlugin.h"
#include "SurveillanceMode.h"
#include "anionu/spot/api/environment.h"


using std::endl;


namespace scy {
namespace anio { 
namespace spot {


SPOT_CORE_PLUGIN(
	SurveillanceModePlugin, 
	"Surveillance Mode Plugin",
	"0.9.3"
)


SurveillanceModePlugin::SurveillanceModePlugin(api::Environment& env) :
	api::IModule(env)
{
	Logger::setInstance(&env.logger());
}


SurveillanceModePlugin::~SurveillanceModePlugin()
{
	Logger::setInstance(NULL);
}


bool SurveillanceModePlugin::load() 
{
	DebugL << "Loading" << endl;
	return true;
}


void SurveillanceModePlugin::unload() 
{	
	DebugL << "Unloading" << endl;
}


api::IMode* SurveillanceModePlugin::createModeInstance(const char* modeName, const char* channelName)
{
	assert(strcmp(modeName, "Surveillance Mode") == 0);
	return new SurveillanceMode(env(), channelName);
}


const char** SurveillanceModePlugin::modeNames() const
{
	static const char* modeNames[] = { "Surveillance Mode", NULL };
	return modeNames;
}


} } } // namespace scy::anio::spot
