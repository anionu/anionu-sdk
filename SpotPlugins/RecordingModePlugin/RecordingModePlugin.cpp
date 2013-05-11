#include "RecordingModePlugin.h"
#include "RecordingMode.h"

#include "Anionu/Spot/API/IEnvironment.h"
#include "Anionu/Spot/API/ModeRegistry.h"


using namespace std;
using namespace Poco;


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


void RecordingModePlugin::initialize() 
{
	log() << "Initializing" << endl;	
	env()->modes().registerMode<RecordingMode>("Recording Mode");
}


void RecordingModePlugin::uninitialize() 
{	
	log() << "Uninitializing" << endl;	
	env()->modes().unregisterMode("Recording Mode");
}


} } } // namespace Scy::Anionu::Spot