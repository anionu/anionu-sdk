#ifndef ANIONU_SPOT_IMediaManager_H
#define ANIONU_SPOT_IMediaManager_H


#include "Sourcey/TimedManager.h"
#include "Sourcey/PacketStream.h"
#include "Sourcey/Media/IEncoder.h"
#include "Sourcey/Media/Format.h"
#include "Sourcey/Spot/IModule.h"

#include "Sourcey/Media/MediaFactory.h"
#include "Sourcey/Media/FormatRegistry.h"
#include "Sourcey/Media/DeviceManager.h"


namespace Sourcey {
namespace Spot {


class IChannel;


struct RecordingInfo 
{
	std::string token;
	Media::IPacketEncoder* encoder;
	RecordingInfo(const std::string& token = "", Media::IPacketEncoder* encoder = NULL) : 
		token(token), 
		encoder(encoder) {}
};


class IMediaManager: public TimedManager<std::string, PacketStream>, public IModule
{
public:
	typedef TimedManager<std::string, PacketStream>	Manager;
	typedef Manager::Base::Map						Map;

public:
	IMediaManager(IEnvironment& env);
	virtual ~IMediaManager();

	virtual void initRecorderParams(IChannel& channel, Media::RecorderParams& params) = 0;
		/// Initializes default recorder parameters from the user
		/// configuration for the current session.
	
	virtual RecordingInfo startRecording(IChannel& channel, const Media::RecorderParams& params) = 0;
		/// Initializes a recorder instance for the current channel.
		/// The default encoder format parameters are provided by
		/// the application configuration.
		/// The returned string is the recorder's index token used 
		/// as a reference by the StreamManager.

	virtual bool stopRecording(const std::string& token, bool whiny = true) = 0;
		/// Stops the recorder session matching the given token.

	Signal2<const Media::RecorderParams&, Media::IPacketEncoder*&> InitializeEncoder;
		/// Provides plugins with the ability to override creation
		/// of the default recording encoder. A valid IPacketEncoder
		/// instance must be returned. Encoders should always use
		/// the channel's device sources.
	
	virtual Media::FormatRegistry& recordingFormats() = 0;
		/// Media formats for recording media.

	virtual Media::FormatRegistry& localVideoStreamingFormats() = 0;
		/// Media formats for streaming media over the local network.

	virtual Media::FormatRegistry& remoteVideoStreamingFormats() = 0;
		/// Media formats for streaming media over the internet.

	virtual Media::FormatRegistry& audioStreamingFormats() = 0;
		/// Media formats for streaming audio over the internet.

	virtual Media::Format getRecordingFormat() = 0;
		/// Returns she current user configured recording media format.
		
	virtual Media::Format getLocalVideoStreamingFormat() = 0;
		/// Returns the current user configured local network streaming
		/// media format.

	virtual Media::Format getRemoteVideoStreamingFormat() = 0;
		/// The current user configured internet streaming media format.

	virtual Media::Format getAudioStreamingFormat() = 0;
		/// The current user configured audio streaming format.

	virtual void setRecordingFormat(const Media::Format& format) = 0;
		/// Sets the recording media format for the current user
		/// and updates configuration.
		
	virtual void setLocalVideoStreamingFormat(const Media::Format& format) = 0;
		/// Sets the local network streaming media format for the
		/// current user and updates configuration.

	virtual void setRemoteVideoStreamingFormat(const Media::Format& format) = 0;
		/// Sets the internet streaming media format for the current
		/// user and updates configuration.

	virtual void setAudioStreamingFormat(const Media::Format& format) = 0;
		/// Sets the internet streaming audio format for the current
		/// user and updates configuration.
	
	virtual const char* className() const { return "MediaManager"; }
};


} } // namespace Sourcey::Spot


#endif // ANIONU_SPOT_IMediaManager_H