#ifndef ANIONU_SPOT_RecordingModePlugin_H
#define ANIONU_SPOT_RecordingModePlugin_H


#include "Sourcey/Spot/IPlugin.h"


namespace Sourcey {
namespace Spot {


class RecordingModePlugin: public IPlugin
{
public:
	RecordingModePlugin();
	virtual ~RecordingModePlugin();

	void initialize();
	void uninitialize();

	virtual const char* className() const { return "RecordingModePlugin"; }
};


} } // namespace Sourcey::Spot


#endif // ANIONU_SPOT_RecordingModePlugin_H