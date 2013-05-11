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


#ifndef ANIONU_SPOT_API_IMediaManager_H
#define ANIONU_SPOT_API_IMediaManager_H


#include "Sourcey/Base.h"
#include "Sourcey/Signal.h"
#include "Sourcey/PacketStream.h"
#include "Sourcey/Media/IEncoder.h"


namespace Scy {
namespace Media {
	struct Format;
	class FormatRegistry;}
namespace Anionu {
namespace Spot { 
namespace API { 


struct RecordingOptions: public Media::RecordingOptions
{
	std::string token;		// The session's unique identification token
	std::string channel;	// The channel we're recording on
	std::string user;		// The user ID of the initiating peer
	bool synchronize;		// Weather or not to synchronize the recorded video
	bool supressEvents;		// Supress events for this recording session
	RecordingOptions(const std::string& channel = "") : 
		channel(channel), 
		synchronize(false),
		supressEvents(false) {}
};


struct RecorderStream: public PacketStream
{
	API::RecordingOptions options;
	RecorderStream(const API::RecordingOptions& options) :
		PacketStream(options.token),
		options(options) {}
};


// ---------------------------------------------------------------------
//
class IMediaManager
{
public:	
	virtual void startRecording(API::RecordingOptions& options) = 0;
		/// Starts a new recorder instance from the given options.
		/// Calls createRecorder() internally.
		/// An exception will be thrown on error.

	virtual bool stopRecording(const std::string& token, bool whiny = true) = 0;
		/// Stops the recorder instance matching the given token.
		/// Returns true on success, or if whiny is set then an 
		/// exception will be thrown on error.

	virtual API::RecorderStream* createRecorder(API::RecordingOptions& options) = 0;
		/// Creates a new recorder instance from the given options.
		/// Call RecorderStream::start() to start actual recording.
		/// Using this method directly, instead of startRecording() 
		/// enables custom manipulation of the PacketStream adapters.
		/// The RecordingOptions must be correctly populated, 
		/// ideally using the getRecordingOptions() method.
		/// An exception will be thrown on error.

	virtual API::RecordingOptions getRecordingOptions(const std::string& channel) = 0;
		/// Initializes default recorder and encoder input format options 
		/// for the given channel, and output format options from the
		/// current user configuration. 
		/// An exception will be thrown if the channel does not exist, 
		/// or if a video device is unavailable.

	virtual Media::Format getRecordingFormat() = 0;
		/// Returns the current user configured recording media format.
		
	virtual Media::Format getVideoStreamingFormat() = 0;
		/// Returns the current user configured local network
		/// streaming media format.

	virtual Media::Format getAudioStreamingFormat() = 0;
		/// The current user configured audio streaming format.

	virtual void setRecordingFormat(const Media::Format& format) = 0;
		/// Sets the recording media format for the current user
		/// and updates configuration.
		
	virtual void setVideoStreamingFormat(const Media::Format& format) = 0;
		/// Sets the local network streaming media format for the
		/// current user and updates configuration.

	virtual void setAudioStreamingFormat(const Media::Format& format) = 0;
		/// Sets the internet streaming audio format for the current
		/// user and updates configuration.
	
	virtual Media::FormatRegistry& recordingFormats() = 0;
		/// Media formats for recording media.

	virtual Media::FormatRegistry& videoStreamingFormats() = 0;
		/// Media formats for streaming video over the internet.

	virtual Media::FormatRegistry& audioStreamingFormats() = 0;
		/// Media formats for streaming audio over the internet.

	Signal2<const Media::RecordingOptions&, Media::IPacketEncoder*&> CreateEncoder;
		/// Provides listeners with the ability to instantiate the recording encoder.
		/// If a valid IPacketEncoder instance is assigned to the second parameter,
		/// it will be used for encoding.
		
	Signal<API::RecorderStream&> RecordingStarted;
	Signal<API::RecorderStream&> RecordingStopped;
		
protected:
	virtual ~IMediaManager() = 0 {};
};


} } } } // namespace Scy::Anionu::Spot::API


#endif // ANIONU_SPOT_API_IMediaManager_H