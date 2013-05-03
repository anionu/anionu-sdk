#ifndef ANIONU_SPOT_SurveillanceModePlugin_H
#define ANIONU_SPOT_SurveillanceModePlugin_H


#include "Sourcey/Spot/IPlugin.h"


namespace Sourcey {
namespace Spot {


class SurveillanceModePlugin: public IPlugin
{
public:
	SurveillanceModePlugin();
	virtual ~SurveillanceModePlugin();

	void initialize();
	void uninitialize();
};


} } // namespace Sourcey::Spot


#endif // ANIONU_SPOT_SurveillanceModePlugin_H