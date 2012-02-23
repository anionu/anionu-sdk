#ifndef SURVEILLANCE_PLUGIN_H
#define SURVEILLANCE_PLUGIN_H


#include "Sourcey/Spot/IPlugin.h"


namespace Sourcey {
namespace Spot {


class SurveillanceMode: public IPlugin
{
public:
	SurveillanceMode();
	~SurveillanceMode();

	void enable();
	void disable();
	
	std::string name() const
	{
		return "SurveillanceMode";
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
};


} } // namespace Sourcey::Spot


#endif // SURVEILLANCE_PLUGIN_H