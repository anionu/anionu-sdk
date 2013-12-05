#ifndef Anionu_Spot_SurveillanceModePlugin_H
#define Anionu_Spot_SurveillanceModePlugin_H


#include "anionu/spot/api/iplugin.h"
#include "anionu/spot/api/imodule.h"
#include "anionu/spot/api/imode.h"
#include "anionu/spot/api/streamingsession.h"


namespace scy {
namespace anio { 
namespace spot {


class SurveillanceModePlugin: 
	public api::IPlugin, 
	public api::IModule, 
	public api::IModeFactory
{
public:
	SurveillanceModePlugin(api::Environment& env);
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


} } } // namespace scy::anio::spot


#endif // Anionu_Spot_SurveillanceModePlugin_H