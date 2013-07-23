#include "RecordingModePlugin.h"
#include "RecordingMode.h"

#include "Anionu/Spot/API/IEnvironment.h"



using namespace std;
using namespace Poco;
using namespace scy::anionu::spot::api;


DEFINE_SPOT_PLUGIN(scy::anionu::spot::RecordingModePlugin)


namespace scy {
namespace anionu { 
namespace spot {


RecordingModePlugin::RecordingModePlugin()
{
}


RecordingModePlugin::~RecordingModePlugin()
{
}


bool RecordingModePlugin::load() 
{
	// Set the default logger instance, otherwise
	// a new default logger instance will be
	// created for the plugin process.
	Logger::setInstance(&env()->logger());

	log("Loading");	
	return true;
}


void RecordingModePlugin::unload() 
{	
	log("Unloading");	
}


IMode* RecordingModePlugin::createModeInstance(const char* modeName, const char* channelName)
{
	assert(strcmp(modeName, "Recording Mode") == 0);
	return new RecordingMode(*env(), channelName);
}


const char** RecordingModePlugin::modeNames() const
{
	static const char* modeNames[] = { "Recording Mode", NULL };
	return modeNames;
}


} } } // namespace scy::anionu::Spot