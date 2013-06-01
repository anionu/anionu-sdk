#ifndef ANIONU_SPOT_RecordingModePlugin_H
#define ANIONU_SPOT_RecordingModePlugin_H


#include "Anionu/Spot/API/IPlugin.h"
#include "Anionu/Spot/API/IModule.h"
#include "Anionu/Spot/API/IMode.h"


namespace Scy {
namespace Anionu { 
namespace Spot {


class RecordingModePlugin: 
	public API::IPlugin, 
	public API::IModule, 
	public API::IModeFactory
{
public:
	RecordingModePlugin();
	virtual ~RecordingModePlugin();
	
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
	const char* className() const { return "RecordingModePlugin"; }
};


} } } // namespace Scy::Anionu::Spot


#endif // ANIONU_SPOT_RecordingModePlugin_H