//
// LibSourcey
// Copyright (C) 2005, Sourcey <http://sourcey.com>
//
// LibSourcey is is distributed under a dual license that allows free, 
// open source use and closed source use under a standard commercial
// license.
//
// Non-Commercial Use:
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
// 
// Commercial Use:
// Please contact mail@sourcey.com
//



#ifndef ANIONU_SPOT_IMediaManager_H
#define ANIONU_SPOT_IMediaManager_H


#include "Sourcey/Util/TimedManager.h"
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
	std::string channel;
	Media::IPacketEncoder* encoder;
	bool synchronize;
	RecordingInfo(const std::string& token = "", const std::string& channel = "", 
		Media::IPacketEncoder* encoder = NULL, bool synchronize = false) : 
		token(token), 
		channel(channel), 
		encoder(encoder), 
		synchronize(synchronize) {}
};


class IMediaManager: public TimedManager<std::string, PacketStream>, public IModule
{
public:
	typedef TimedManager<std::string, PacketStream>	Manager;
	typedef Manager::Base::Map						Map;

public:
	IMediaManager(IEnvironment& env);
	virtual ~IMediaManager();

	virtual void initRecorderOptions(IChannel& channel, Media::RecorderOptions& options) = 0;
		/// Initializes default recorder options from the current
		/// user's configuration.
	
	virtual RecordingInfo* startRecording(IChannel& channel, const Media::RecorderOptions& options) = 0; //, bool disableSync = false
		/// Initializes a recorder instance for the current channel.
		/// If disableSync then the video file NOT be synchronized
		/// with the user account.
		/// The returned RecordingInfo pointer is managed internally and,
		/// and will be freed when the recording session ends.

	virtual bool stopRecording(const std::string& token, bool whiny = true) = 0;
		/// Stops the recorder session matching the given token.

	Signal2<const Media::RecorderOptions&, Media::IPacketEncoder*&> InitializeEncoder;
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