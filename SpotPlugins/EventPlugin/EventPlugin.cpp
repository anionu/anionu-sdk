#include "EventPlugin.h"

#include "Anionu/Spot/API/IEnvironment.h"
#include "Anionu/Spot/API/IClient.h"
#include "Anionu/Spot/API/Util.h"
#include "Anionu/Event.h"


using namespace std;


DEFINE_SPOT_PLUGIN(Scy::Anionu::Spot::EventPlugin)


namespace Scy {
namespace Anionu {
namespace Spot {


EventPlugin::EventPlugin()
{
	// :)
}


EventPlugin::~EventPlugin()
{
}


bool EventPlugin::load() 
{
	// Load all your classes and run any system checks here. 
	//
	// If there is an error and the plugin cannot continue to function
	// for any reason, we would return false here instead of true.
	//	
	// Call the logger so we know if the plugin is going to be binary 
	// compatable with the Spot client. If it is not, then Spot will 
	// most likely choke and crash at this point!
	log("Loading", "info");
	return true;
}


void EventPlugin::unload() 
{	
	// Free all allocated memory and pointers here.
	log("Unloading", "info");
}


void EventPlugin::onPresence(const char* presence) 
{
	// Parse the incoming symple presence message.
	Symple::Message p;
	p.read(presence);
	
	// Log the incoming presence message for debugging.
	log("<<<<< Presence Received: " + p.toStyledString(), "info");

	// Anionu currently uses two presence types, "User" and "Spot".
	// The "User" presence type is used for peers connecting from the dashboard,
	// and "Spot" type is used if the peers connecting from a Spot client.
	//
	// For the purposes of this plugin, we want to catch presence events originating
	// from the dashboard. This means that whenever a user connects to the presence
	// server via the dashboard, our custom "Presence Detected" event will fire.
	//
	if (p.data("type").asString() == "User" &&
		p.data("online").asBool() == true) {

		// Create the custom "Presence Detected" event using the Anionu REST API.
		//
		// When the event is created, real-time notifications will be sent to everyone
		// configured to receive notifications for the "Presence Detected" event.
		// See the tutorial for help configuring notifications.
		//
		// If you are logged into the dashboard, you will see this event displayed in
		// real-time in the sidebar notifications panel, and also on the Events page.
		//		
		string message = p.data("name").asString() + " presence detected, send in the bomb squad!";
		env()->clientBase().createEvent("Presence Detected", message.data(), 3);
						
#if 0 // Anionu_Spot_ENABLE_ABI_COMPATABILITY
		// Note that if we were inheriting from IMode rather that IModeBase,
		// we could create the event more elegently with the convenience 
		// of the C++ standard libraries:
		//
		env()->client().createEvent("Presence Detected", 
			p.data("name").asString() + " presence detected, send in the bomb squad!",
			Anionu::Event::Medium);
#endif
	}
}


} } } // namespace Scy::Anionu::Spot