#include "RecordingModePlugin.h"
#include "RecordingMode.h"

#include "anionu/spot/api/environment.h"


namespace scy {
namespace anio { 
namespace spot {


SPOT_CORE_PLUGIN(
	RecordingModePlugin, 
	"Recording Mode Plugin", 
	"0.9.2"
)


RecordingModePlugin::RecordingModePlugin(api::Environment& env) :
	api::IModule(env)
{
	// Set the default logger instance, otherwise
	// a new logger singleton instance will be
	// created for the plugin process.
	//Logger::setInstance(&env.logger());
}


RecordingModePlugin::~RecordingModePlugin()
{
	//Logger::setInstance(NULL);
}


bool RecordingModePlugin::load() 
{
	log("Loading");	
	return true;
}


void RecordingModePlugin::unload() 
{	
	log("Unloading");	
}


api::IMode* RecordingModePlugin::createModeInstance(const char* modeName, const char* channelName)
{
	assert(strcmp(modeName, "Recording Mode") == 0);
	return new RecordingMode(env(), channelName);
}


const char** RecordingModePlugin::modeNames() const
{
	static const char* modeNames[] = { "Recording Mode", NULL };
	return modeNames;
}


} } } // namespace scy::anio::spot