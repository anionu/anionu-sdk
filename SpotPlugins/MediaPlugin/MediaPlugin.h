#ifndef ANIONU_SPOT_MediaPlugin_H
#define ANIONU_SPOT_MediaPlugin_H


#include "Anionu/Spot/API/IPlugin.h"
#include "Anionu/Spot/API/IModule.h"
#include "Anionu/Spot/API/IStreamingManager.h"
#include "Anionu/Spot/API/IMediaManager.h"
#include "Sourcey/Media/AVEncoder.h"


namespace Scy {
namespace Anionu { 
namespace Spot {


class MediaPlugin: 
	public API::IPlugin, 
	public API::IModule
{
public:
	MediaPlugin();
	virtual ~MediaPlugin();

	bool load();
	void unload();
	
protected:
	Media::IPacketEncoder* createEncoder(const Media::RecordingOptions& options);
	
	void onInitStreamingSession(void*, API::IStreamingSession& session, bool&);
	void onInitRecordingEncoder(void*, const API::RecordingOptions& options, Media::IPacketEncoder*&);

	void registerMediaFormats();
};


} } } // namespace Scy::Anionu::Spot


#endif // ANIONU_SPOT_MediaPlugin_H