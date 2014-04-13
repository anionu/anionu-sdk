//
// Anionu SDK
// Copyright (C) 2011, Anionu <http://anionu.com>
//
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

#ifndef Anionu_Spot_API_MediaManager_H
#define Anionu_Spot_API_MediaManager_H


#include "anionu/spot/api/api.h"

#ifdef Anionu_Spot_USING_CORE_API
#include "scy/base.h"
#include "scy/signal.h"
#include "scy/packetstream.h"
#include "scy/media/iencoder.h"
#endif


namespace scy {
namespace av {
	struct Format;
	class FormatRegistry;}
namespace anio {
namespace spot { 
namespace api { 


class MediaManagerBase
	/// ABI agnostic API
{
public:
	virtual void startRecording(const char* channel, const char* ofile = nullptr, bool synchronize = false) = 0;
		// Starts recording the video input of the given
		// surveillance channel.
		
		// A medium severity "Recording Failed" event will
		// be created and propagated on the system which 
		// contains the full error message.

	virtual bool stopRecording(const char* token) = 0;
		// Stops the recorder instance matching the given token.
		// Returns true on success, or if whiny is set then an 
		// exception will be thrown on error.
		
protected:
	virtual ~MediaManagerBase() {};
};


#ifdef Anionu_Spot_USING_CORE_API


struct EncoderOptions: public av::EncoderOptions
{
	std::string token;		// The session's unique identification token
	std::string channel;	// The channel we're recording on
	std::string user;		// The user ID of the initiating peer
	std::string mime;		// The encoded video mime type ie. "video/mp4"
	bool supressEvents;		// Suppress events for this recording session
	bool synchronizeVideo;	// Weather or not to synchronize the recorded video.
							// The user configured value will be set when creating
							// options using MediaManager::getEncoderOptions()
	EncoderOptions(const std::string& channel = "") : 
		channel(channel),
		supressEvents(false), 
		synchronizeVideo(false) {}
};


struct RecordingSession//: public PacketStream
{
	PacketStream stream;
	api::EncoderOptions options;

	RecordingSession(const api::EncoderOptions& options) :
		stream(options.token), options(options) {}
};


typedef PacketProcessor Recorder; 
	/// The recorder type, generally an av::IEncoder instance.
	/// May become a full type in the future.


class MediaManager: public MediaManagerBase
{
public:	
	virtual void startRecording(api::EncoderOptions& options) = 0;
		// Starts a new recorder instance from the given options.
		// Calls createRecorder() internally.
		// An exception will be thrown on error.

	virtual bool stopRecording(const std::string& token, bool whiny = true) = 0;
		// Stops the recorder instance matching the given token.
		// Returns true on success, or if whiny is set then an 
		// exception will be thrown on error.

	virtual api::RecordingSession* createRecorder(api::EncoderOptions& options, bool whiny = true) = 0;
		// Creates a new recorder instance from the given options.
		// Call RecordingSession->stream.start() to start actual recording.
		// Using this method directly, instead of startRecording() 
		// enables custom manipulation of the PacketStream adapters.
		// The EncoderOptions must be correctly populated, 
		// ideally using the getEncoderOptions() method.
		// An exception will be thrown on error.

	virtual api::EncoderOptions getEncoderOptions(const std::string& channel) = 0;
		// Initializes default recorder and encoder input format options 
		// for the given channel, and output format options from the
		// current user configuration. 
		// An exception will be thrown if the channel does not exist, 
		// or if a video device is unavailable.

	virtual av::Format getRecordingFormat() = 0;
		// Returns the current user configured recording media format.
		
	virtual av::Format getVideoStreamingFormat() = 0;
		// Returns the current user configured local network
		// streaming media format.

	virtual av::Format getAudioStreamingFormat() = 0;
		// The current user configured audio streaming format.

	virtual void setRecordingFormat(const av::Format& format) = 0;
		// Sets the recording media format for the current user
		// and updates configuration.
		
	virtual void setVideoStreamingFormat(const av::Format& format) = 0;
		// Sets the local network streaming media format for the
		// current user and updates configuration.

	virtual void setAudioStreamingFormat(const av::Format& format) = 0;
		// Sets the internet streaming audio format for the current
		// user and updates configuration.
	
	virtual av::FormatRegistry& recordingFormats() = 0;
		// Media formats for recording media.

	virtual av::FormatRegistry& videoStreamingFormats() = 0;
		// Media formats for streaming video over the internet.

	virtual av::FormatRegistry& audioStreamingFormats() = 0;
		// Media formats for streaming audio over the internet.

	virtual void MediaManager::resetDefaultMediaFormats() = 0;
		// Resets default media formats for recording and streaming.
	
	Signal2<api::RecordingSession&, bool&> SetupRecordingSources;
		// Provides plugins with the ability to override the device capture 
		// sources for this recording session.
		//
		// If the capture source creation is overridden the handled boolean
		// (second signal argument) must return true in order to indicate to 
		// Spot and lower priority plugins that captures have been initialized.	
		//
		// Also, when overriding capture sources the output format encoder 
		// options should be updated accordingly depending the availability
		// of audio and video sources eg:
		//
		//     session.options.oformat.video.enabled = !!hasVideoSource;
		//     session.options.oformat.audio.enabled = !!hasAudioSource;

	Signal2<api::RecordingSession&, bool&> SetupRecordingEncoders;
		// Provides listeners with the ability to instantiate the recording encoder.
		// If a valid Recorder instance is assigned to the second parameter,
		// it will be used for encoding.
		
	Signal<api::RecordingSession&> RecordingStarted;
	Signal<api::RecordingSession&> RecordingStopped;
		
protected:
	virtual ~MediaManager() {};
};


#endif // Anionu_Spot_USING_CORE_API


} } } } // namespace scy::anio::spot::api


#endif // Anionu_Spot_API_MediaManager_H