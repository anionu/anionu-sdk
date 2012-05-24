#include "RecordingModePlugin.h"
#include "RecordingMode.h"

//#include "Sourcey/Spot/IPluginAPI.h"
#include "Sourcey/Spot/IEnvironment.h"
#include "Sourcey/Spot/IModeRegistry.h"

#include "Poco/ClassLibrary.h"


using namespace std;
using namespace Poco;
using namespace Sourcey;
using namespace Sourcey::Spot;


POCO_BEGIN_MANIFEST(IPlugin)
	POCO_EXPORT_CLASS(RecordingModePlugin)
POCO_END_MANIFEST


namespace Sourcey {
namespace Spot {


// ----------------------------------------------------------------------------
//
// Recording Mode Plugin
//
// ----------------------------------------------------------------------------
RecordingModePlugin::RecordingModePlugin()
{
}


RecordingModePlugin::~RecordingModePlugin()
{
}


void RecordingModePlugin::initialize() 
{
	log() << "Initializing" << endl;	
	env().modes().registerMode<RecordingMode>("Recording Mode");
}


void RecordingModePlugin::uninitialize() 
{	
	log() << "Uninitializing" << endl;	
	env().modes().unregisterMode("Recording Mode");
}


} } // namespace Sourcey::Spot