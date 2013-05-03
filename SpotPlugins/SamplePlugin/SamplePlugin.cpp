#include "SamplePlugin.h"

#include "Sourcey/Spot/IEnvironment.h"
#include "Sourcey/Anionu/Event.h"


using namespace std;
using namespace Sourcey;
using namespace Sourcey::Spot;


POCO_BEGIN_MANIFEST(IPlugin)
	POCO_EXPORT_CLASS(SamplePlugin)
POCO_END_MANIFEST


namespace Sourcey {
namespace Spot {


SamplePlugin::SamplePlugin()
{
}


SamplePlugin::~SamplePlugin()
{
	log() << "Destroying" << endl;
}


void SamplePlugin::initialize() 
{
	log() << "Initializing" << endl;
	
	// Attach an incoming presence message listener, which
	// will notify us any of our account peers come online.
	env().attachMessageListener(Symple::presenceDelegate(this, &SamplePlugin::onRecvPresence));
}


void SamplePlugin::uninitialize() 
{	
	log() << "Uninitializing" << endl;

	// Detach the presence message listener.
	env().detachMessageListener(Symple::presenceDelegate(this, &SamplePlugin::onRecvPresence));
}

	
void SamplePlugin::onRecvPresence(void*, Symple::Presence& p)
{
	// Print the presence message for debugging.
	log("info") << "<<<<<<<<<<<<<<<<<<<<<< Presence Received: " 
		<< p.id() << ":\n" 
		<< JSON::stringify(p, true) 
		<< std::endl;

	// Anionu currently uses two presence types, "User" and "Spot". The presence
	// type will be "User" if the peer is logging on from the dashboard, and "Spot"
	// if the peer is logging on from a Spot client.
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
		Anionu::Event event("Presence Detected", 
			p.data("name").asString() + " presence detected, send in the bomb squad!",
			Anionu::Event::SpotLocal, Anionu::Event::Medium);
		env().createEvent(event);
	}
}


} } // namespace Sourcey::Spot