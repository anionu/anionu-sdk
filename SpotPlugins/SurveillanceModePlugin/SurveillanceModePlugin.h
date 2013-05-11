#ifndef ANIONU_SPOT_SurveillanceModePlugin_H
#define ANIONU_SPOT_SurveillanceModePlugin_H


#include "Anionu/Spot/API/IPlugin.h"


namespace Scy {
namespace Anionu { 
namespace Spot {


class SurveillanceModePlugin: public API::IPlugin
{
public:
	SurveillanceModePlugin();
	virtual ~SurveillanceModePlugin();

	void initialize();
	void uninitialize();
};


} } } // namespace Scy::Anionu::Spot


#endif // ANIONU_SPOT_SurveillanceModePlugin_H