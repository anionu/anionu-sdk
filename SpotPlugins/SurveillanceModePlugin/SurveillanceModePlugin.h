#ifndef Anionu_Spot_SurveillanceModePlugin_H
#define Anionu_Spot_SurveillanceModePlugin_H


#include "Anionu/Spot/API/IPlugin.h"
#include "Anionu/Spot/API/IModule.h"
#include "Anionu/Spot/API/IMode.h"


namespace scy {
namespace anionu { 
namespace spot {


class SurveillanceModePlugin: 
	public api::IPlugin, 
	public api::IModule, 
	public api::IModeFactory
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
	api::IMode* createModeInstance(const char* modeName, const char* channelName);
	const char** modeNames() const;
		
	//
	/// IModule
	const char* className() const { return "SurveillanceModePlugin"; }
};


} } } // namespace scy::anionu::Spot


#endif // Anionu_Spot_SurveillanceModePlugin_H