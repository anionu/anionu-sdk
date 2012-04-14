#include "RecordingMode.h"
#include "RecordingModeImpl.h"

//#include "Sourcey/Spot/IPluginAPI.h"
#include "Sourcey/Spot/IEnvironment.h"
#include "Sourcey/Spot/IModeRegistry.h"

#include "Poco/ClassLibrary.h"


using namespace std;
using namespace Poco;
using namespace Sourcey;
using namespace Sourcey::Spot;


POCO_BEGIN_MANIFEST(IPlugin)
	POCO_EXPORT_CLASS(RecordingMode)
POCO_END_MANIFEST


namespace Sourcey {
namespace Spot {


// ----------------------------------------------------------------------------
//
// Recording Mode Plugin
//
// ----------------------------------------------------------------------------
RecordingMode::RecordingMode()
{
	//log() << "[RecordingMode] RecordingMode()" << endl;
}


RecordingMode::~RecordingMode()
{	
	//log() << "Destroying" << endl;
	//if (isEnabled()) disable();
}


/*
void RecordingMode::initialize() 
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


void RecordingMode::enable() 
{
	log() << "Enabling" << endl;

	try 
	{
		env().modes().registerMode<RecordingModeImpl>("Recording Mode");

		//setState(PluginState::Enabled);
	} 
	catch (Poco::Exception& exc) 
	{
		//setState(PluginState::Error);
		throw exc;
	}
}


void RecordingMode::disable() 
{	
	log() << "Disabling" << endl;

	try 
	{
		env().modes().unregisterMode("Recording Mode");
	
		//setState(PluginState::Disabled);
	} 
	catch (Poco::Exception& exc) 
	{
		//setState(PluginState::Error);
		throw exc;
	}
}


} } // namespace Sourcey::Spot