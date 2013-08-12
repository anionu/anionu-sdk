#ifndef Anionu_Spot_RecordingModePlugin_H
#define Anionu_Spot_RecordingModePlugin_H


#include "Anionu/Spot/API/IPlugin.h"
#include "Anionu/Spot/API/IModule.h"
#include "Anionu/Spot/API/IMode.h"


namespace scy {
namespace anio { 
namespace spot {


class RecordingModePlugin: 
	public api::IPlugin, 
	public api::IModule, 
	public api::IModeFactory
{
public:
	RecordingModePlugin(api::Environment& env);
	virtual ~RecordingModePlugin();
	
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
	const char* className() const { return "RecordingModePlugin"; }
};


} } } // namespace scy::anio::spot


#endif // Anionu_Spot_RecordingModePlugin_H