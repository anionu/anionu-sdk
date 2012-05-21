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
	
	/*
	std::string name() const
	{
		return "RecordingModePlugin";
	}
	
	std::string title() const
	{
		return "Recording Mode Plugin";
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


#endif // ANIONU_SPOT_RecordingModePlugin_H