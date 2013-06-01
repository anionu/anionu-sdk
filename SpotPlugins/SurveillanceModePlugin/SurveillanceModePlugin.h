#ifndef ANIONU_SPOT_SurveillanceModePlugin_H
#define ANIONU_SPOT_SurveillanceModePlugin_H


#include "Anionu/Spot/API/IPlugin.h"
#include "Anionu/Spot/API/IModule.h"
#include "Anionu/Spot/API/IMode.h"


namespace Scy {
namespace Anionu { 
namespace Spot {


class SurveillanceModePlugin: 
	public API::IPlugin, 
	public API::IModule, 
	public API::IModeFactory
{
public:
	SurveillanceModePlugin();
	virtual ~SurveillanceModePlugin();
	
	//
	/// IPlugin
	bool load();
	void unload();

	//
	/// IModeFactory
	API::IMode* createModeInstance(const char* modeName, const char* channelName);
	const char** modeNames() const;
		
	//
	/// IModule
	const char* className() const { return "SurveillanceModePlugin"; }
};


} } } // namespace Scy::Anionu::Spot


#endif // ANIONU_SPOT_SurveillanceModePlugin_H