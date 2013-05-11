#ifndef ANIONU_SPOT_RecordingModePlugin_H
#define ANIONU_SPOT_RecordingModePlugin_H


#include "Anionu/Spot/API/IPlugin.h"


namespace Scy {
namespace Anionu { 
namespace Spot {


class RecordingModePlugin: public API::IPlugin
{
public:
	RecordingModePlugin();
	virtual ~RecordingModePlugin();

	void initialize();
	void uninitialize();

	virtual const char* className() const { return "RecordingModePlugin"; }
};


} } } // namespace Scy::Anionu::Spot


#endif // ANIONU_SPOT_RecordingModePlugin_H