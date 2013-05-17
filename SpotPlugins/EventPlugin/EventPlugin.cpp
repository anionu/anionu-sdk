#include "EventPlugin.h"

#include "Anionu/Spot/API/IEnvironment.h"
#include "Anionu/Spot/API/IEventManager.h"
#include "Anionu/Event.h"


using namespace std;


DEFINE_SPOT_PLUGIN(Scy::Anionu::Spot::EventPlugin)


namespace Scy {
namespace Anionu {
namespace Spot {


EventPlugin::EventPlugin()
{
}


EventPlugin::~EventPlugin()
{
	// :)
}


bool EventPlugin::load() 
{
	//log() << "Initializing" << endl;
	
	// Attach a listener for incoming presence messages.
	// Presence messages are sent to all peers to notify them about a peer's online status.
	//env->attachListener(Symple::presenceDelegate(this, &EventPlugin::onRecvPresence));


	return true;
}


void EventPlugin::unload() 
{	
	//log() << "Uninitializing" << endl;

	// Detach the listener.
	//env->detachListener(Symple::presenceDelegate(this, &EventPlugin::onRecvPresence));
}


void EventPlugin::onPresence(const char* presence) 
{
	Symple::Presence p(presence);

	// Anionu currently uses two presence types, "User" and "Spot".
	// The presence type "User" is used for peers connecting from the dashboard,
	// and type "Spot" is used if the peers connecting from a Spot client.
	//
	// For the purposes of this plugin, we want to catch presence events originating
	// from the dashboard. This means that whenever a user connects to the presence
	// server via the dashboard, our custom "Presence Detected" event will fire.
	//
	if (p.data("type").asString() == "User") {

		// Create our custom "Presence Detected" event via the Anionu REST API.
		//
		// When the event is created, real-time notifications will be sent to everyone
		// configured to receive notifications for the "Presence Detected" event.
		// See the tutorial for help configuring notifications.
		//
		// If you are logged into the dashboard, you will see this event displayed in
		// real-time in the sidebar notifications panel, and also on the Events page.
		//
		//Anionu::Event event("Presence Detected", 
		//	p.data("name").asString() + " presence detected, send in the bomb squad!",
		//	Anionu::Event::Medium, Anionu::Event::SpotLocal);
		//env->createEvent(event);event
		//const char* name, const char* message, int severity = 0
		string message = p.data("name").asString() + " presence detected, send in the bomb squad!";
		env->eventsBase().createEvent("Presence Detected", message.data(), 3);
	}
}


} } } // namespace Scy::Anionu::Spot



	
/*
	// Print the entire presence message for debugging.
	//log("info") << "<<<<<<<<<<<<<<<<<<<<<< Presence Received: " 
	//	<< p.id() << ":\n" 
	//	<< JSON::stringify(p, true) 
	//	<< std::endl;

void EventPlugin::onRecvPresence(void*, Symple::Presence& p)
{
	// Print the entire presence message for debugging.
	log("info") << "<<<<<<<<<<<<<<<<<<<<<< Presence Received: " 
		<< p.id() << ":\n" 
		<< JSON::stringify(p, true) 
		<< std::endl;
}
*/