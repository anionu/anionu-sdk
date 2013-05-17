#include "RecordingModePlugin.h"
#include "RecordingMode.h"

#include "Anionu/Spot/API/IEnvironment.h"



using namespace std;
using namespace Poco;
using namespace Scy::Anionu::Spot::API;


DEFINE_SPOT_PLUGIN(Scy::Anionu::Spot::RecordingModePlugin)


namespace Scy {
namespace Anionu { 
namespace Spot {


RecordingModePlugin::RecordingModePlugin()
{
}


RecordingModePlugin::~RecordingModePlugin()
{
}


bool RecordingModePlugin::load() 
{
	log() << "Initializing" << endl;	
	//env->modes().registerMode<RecordingMode>("Recording Mode");
	return true;
}


void RecordingModePlugin::unload() 
{	
	log() << "Uninitializing" << endl;	
	//env->modes().unregisterMode("Recording Mode");
}


IMode* RecordingModePlugin::createModeInstance(const char* mode, const char* channel)
{
	assert(strcmp(mode, "Recording Mode") == 0);
	return new RecordingMode(*env, channel);
}


const char** RecordingModePlugin::modeNames() const
{
	static const char* modeNames[] = { "Recording Mode", NULL };
	return modeNames;
}


} } } // namespace Scy::Anionu::Spot