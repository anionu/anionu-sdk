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
	//log() << "[RecordingModePlugin] RecordingModePlugin()" << endl;
}


RecordingModePlugin::~RecordingModePlugin()
{	
	//log() << "Destroying" << endl;
	//if (isEnabled()) disable();
}


/*
void RecordingModePlugin::initialize() 
{
	log() << "Initializing" << endl;	
	try 
	{
		//setAPI(api);

		//if (!File(path()).exists())
		//	throw Exception(format("CaptureRecorder plugin missing files at: %s", path()));


	}
	catch (Poco::Exception& exc) 
	{
		//setState(PluginState::Error);
		throw exc;
	}
}
*/


void RecordingModePlugin::initialize() 
{
	log() << "Initializing" << endl;
	
		env().modes().registerMode<RecordingMode>("Recording Mode");

	/*
	try 
	{

		//setState(PluginState::Enabled);
	} 
	catch (Poco::Exception& exc) 
	{
		//setState(PluginState::Error);
		throw exc;
	}
	*/
}


void RecordingModePlugin::uninitialize() 
{	
	log() << "Uninitializing" << endl;
	
		env().modes().unregisterMode("Recording Mode");

		/*
	try 
	{
	
		//setState(PluginState::Disabled);
	} 
	catch (Poco::Exception& exc) 
	{
		//setState(PluginState::Error);
		throw exc;
	}
	*/
}


} } // namespace Sourcey::Spot