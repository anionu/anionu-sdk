#ifndef ANIONU_SPOT_MediaPlugin_H
#define ANIONU_SPOT_MediaPlugin_H


#include "Sourcey/Spot/IPlugin.h"
#include "Sourcey/Spot/IStreamingManager.h"
#include "Sourcey/Spot/IMediaManager.h"
#include "Sourcey/Media/AVEncoder.h"

#include "Poco/Thread.h"
#include "Poco/ClassLibrary.h"


namespace Sourcey {
namespace Spot {


class MediaPlugin: public IPlugin
{
public:
	MediaPlugin();
	virtual ~MediaPlugin();

	void initialize();
	void uninitialize();
	
protected:
	Media::IPacketEncoder* createEncoder(const Media::RecorderParams& params);
	
	void onInitializeStreamingSession(void*, IStreamingSession& session, bool&);
	void onInitializeRecordingEncoder(void*, const Media::RecorderParams& params, Media::IPacketEncoder*&);
	
	/*
	std::string title() const
	{
		return "Media Provider";
	}	

	std::string name() const
	{
		return "Media";
	}
	
	std::string author() const
	{
		return "Anionu";
	}
	
	std::string version() const
	{
		return "1";
	}
	*/
};


} } // namespace Sourcey::Spot


#endif // ANIONU_SPOT_MediaPlugin_H