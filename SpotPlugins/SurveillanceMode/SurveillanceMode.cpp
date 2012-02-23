#include "SurveillanceMode.h"
#include "SurveillanceModeImpl.h"

#include "Sourcey/Spot/IEnvironment.h"
#include "Sourcey/Spot/IModeRegistry.h"

#include "Poco/ClassLibrary.h"


using namespace std;
using namespace Poco;
using namespace Sourcey;
using namespace Sourcey::Spot;


POCO_BEGIN_MANIFEST(IPlugin)
	POCO_EXPORT_CLASS(SurveillanceMode)
POCO_END_MANIFEST


namespace Sourcey {
namespace Spot {


SurveillanceMode::SurveillanceMode()
{
}


SurveillanceMode::~SurveillanceMode()
{
	log() << "[SurveillanceMode] Destroying" << endl;
}


void SurveillanceMode::enable() 
{
	log() << "[SurveillanceMode] Enabling" << endl;

	try 
	{
		env().modes().registerMode<SurveillanceModeImpl>("Surveillance Mode");
		setState(PluginState::Enabled);
	} 
	catch (Poco::Exception& exc) 
	{
		setState(PluginState::Error, exc.displayText());
		throw exc;
	}
}


void SurveillanceMode::disable() 
{	
	log() << "[SurveillanceMode] Disabling" << endl;

	try 
	{
		env().modes().unregisterMode("Surveillance Mode");
		setState(PluginState::Disabled);
	} 
	catch (Poco::Exception& exc) 
	{
		setState(PluginState::Error, exc.displayText());
		throw exc;
	}
}


/*
void SurveillanceMode::onRecvStanza(Stanza& s)
{	
	Command c = static_cast<IQ&>(s).command();
	Log("debug") << "[SurveillanceMode] onRecvStanza: " << c.node() << endl;

	if (c.matches("channels:*:modes:surveillance:activate")) 
	{
		try
		{	
			if (!api().session().isActive())		
				throw Exception("Recording failed: An active session is required.");

			const Channel* channel = env().channels().get(c.param(2));


			c.setNote("info", format("%s: Surveillance Mode activated!", channel->name()));
		}
		catch (Exception& exc)
		{
			s.setType("error");
			c.setNote("error", exc.displayText());
		}

		c.setStatus("completed");
		//s.respond();
	}
}


void SurveillanceMode::startMotionDetector(const Channel* channel, const MotionDetectorParams& params)
{	
	log() << "[SurveillanceMode] startMotionDetector: " << channel->name() << endl;
	assert(!channel->name().empty());
	
	// Ensure the current channel is not already recording
	MotionDetectorMap::const_iterator it = _motionMap.find(channel->name());	
	if (it != _motionMap.end()) {
		throw Exception(format("%s: Surveillance Mode is already active.", channel->name()));
	}
	
	if (channel->videoDevice().id < 0)		
		throw Exception(format("%s: You need a video device for Surveillance Mode.", channel->name()));

#ifdef _DEBUG
	VideoCapture* capture = CaptureFactory::instance()->video.getCapture("c:/test.avi");
#else
	VideoCapture* capture = CaptureFactory::instance()->audio.getCapture(channel->audioDevice().id);
#endif //_DEBUG

	MotionDetector* motion = NULL;		
	try
	{	
		// NOTE: The Motion Detector is responsible for it's own destruction
		motion = new MotionDetector(params, capture);
		motion->StateChange += delegate(this, &SurveillanceMode::onMotionStateChange);
		motion->enable();
	}
	catch (Exception& exc)
	{
		if (motion)
			delete motion;
		throw exc;
	}

	// The recorder has been initialized, add it to our map.
	_motionMap[channel->name()] = motion;
}


void SurveillanceMode::stopMotionDetector(std::string& channel)
{
	log() << "[SurveillanceMode] stopMotionDetector: " << channel << endl;

	MotionDetectorMap::const_iterator it = _motionMap.find(channel);	
	if (it == _motionMap.end())
		throw Exception(format("%s: Surveillance Mode is not active!", channel));

	it->second->disable();

	// NOTE: The channel recorder reference will be deleted on callback
}


void SurveillanceMode::onMotionStateChange(const void* sender, StateT& state)
{
	log() << "[SurveillanceMode] onMotionStateChange: " << state.str() << endl;

	switch (state.id()) {
		case MotionDetectorState::Idle:	
			log() << "[RecorderPlugin] onMotionStateChange: Idle" << endl;
		break;
		case MotionDetectorState::Vigilant:
			log() << "[RecorderPlugin] onMotionStateChange: Vigilant" << endl;
		break;
		case MotionDetectorState::Triggered:
			log() << "[RecorderPlugin] onMotionStateChange: Triggered" << endl;
			for (MotionDetectorMap::iterator it=_motionMap.begin(); it!=_motionMap.end(); ++it) {
				if (it->second == sender) {
					_motionMap.erase(it);
					return;
				}
			}
		break;
	}

}
*/


} } // namespace Sourcey::Spot
