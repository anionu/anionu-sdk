#ifndef RECORDER_PLUGIN_H
#define RECORDER_PLUGIN_H


#include "Sourcey/Spot/IPlugin.h"


namespace Sourcey {
namespace Spot {


class RecordingMode: public IPlugin
{
public:
	RecordingMode();
	virtual ~RecordingMode();

	void initialize();
	void uninitialize();
	
	std::string name() const
	{
		return "RecordingMode";
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
};


} } // namespace Sourcey::Spot


#endif // RECORDER_PLUGIN_H