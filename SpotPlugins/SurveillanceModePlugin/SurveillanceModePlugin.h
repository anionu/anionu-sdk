#ifndef ANIONU_SPOT_SurveillanceModePluginPlugin_H
#define ANIONU_SPOT_SurveillanceModePluginPlugin_H


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
	
	/*
	std::string name() const
	{
		return "SurveillanceModePlugin";
	}
	
	std::string title() const
	{
		return "Surveillance Mode Plugin";
	}
	
	std::string author() const
	{
		return "Sourcey";
	}
	
	std::string version() const
	{
		return "0.8.0";
	}
	*/
};


} } // namespace Sourcey::Spot


#endif // ANIONU_SPOT_SurveillanceModePluginPlugin_H