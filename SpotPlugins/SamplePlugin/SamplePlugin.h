#ifndef ANIONU_SPOT_SamplePlugin_H
#define ANIONU_SPOT_SamplePlugin_H


#include "Sourcey/Spot/IPlugin.h"
	/// Include the IPlugin.h header. This is the only
	/// header that is required for creating plugins.

#include "Sourcey/Symple/Client.h"
	/// Include the Symple client headers, as we will
	/// be listening for incoming presence messages.
	

namespace Sourcey {
namespace Spot {


class SamplePlugin: public IPlugin
	/// Define our SamplePlugin class, which extends the 
	/// IPlugin interface.
	/// The class name here must match the "class" member
	/// of the manifest.json file.
{
public:
	SamplePlugin();
	virtual ~SamplePlugin();

	void initialize();
	void uninitialize();
	
	void onRecvPresence(void*, Symple::Presence& p);

	const char* className() const { return "SamplePlugin"; }
};


} } // namespace Sourcey::Spot


#endif // ANIONU_SPOT_SamplePlugin_H