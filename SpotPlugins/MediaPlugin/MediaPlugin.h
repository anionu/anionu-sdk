#ifndef ANIONU_SPOT_MediaPlugin_H
#define ANIONU_SPOT_MediaPlugin_H


#include "Anionu/Spot/API/IPlugin.h"
#include "Anionu/Spot/API/IStreamingManager.h"
#include "Anionu/Spot/API/IMediaManager.h"
#include "Sourcey/Media/AVEncoder.h"

#include "Poco/Thread.h"
#include "Poco/ClassLibrary.h"


namespace Scy {
namespace Anionu { 
namespace Spot {


class MediaPlugin: public API::IPlugin
{
public:
	MediaPlugin();
	virtual ~MediaPlugin();

	void load();
	void unload();
	
protected:
	Media::IPacketEncoder* createEncoder(const Media::RecordingOptions& options);
	
	void onSetupStreamingSession(void*, API::IStreamingSession& session, bool&);
	void onInitializeRecordingEncoder(void*, const Media::RecordingOptions& options, Media::IPacketEncoder*&);
};


} } } // namespace Scy::Anionu::Spot


#endif // ANIONU_SPOT_MediaPlugin_H