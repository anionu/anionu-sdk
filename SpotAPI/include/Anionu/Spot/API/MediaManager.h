//
// LibSourcey
// Copyright (C) 2005, Sourcey <http://sourcey.com>
//
// LibSourcey is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// LibSourcey is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.
//


#ifndef Anionu_Spot_API_MediaManager_H
#define Anionu_Spot_API_MediaManager_H


#include "Anionu/Spot/API/Config.h"

#ifdef Anionu_Spot_USING_CORE_API
#include "Sourcey/Base.h"
#include "Sourcey/Signal.h"
#include "Sourcey/PacketStream.h"
#include "Sourcey/Media/IEncoder.h"
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
	virtual void startRecording(const char* channel, const char* ofile = NULL, bool synchronize = false) = 0;
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
	virtual ~MediaManagerBase() = 0 {};
};


#ifdef Anionu_Spot_USING_CORE_API


struct RecordingOptions: public av::RecordingOptions
{
	std::string token;		// The session's unique identification token
	std::string channel;	// The channel we're recording on
	std::string user;		// The user ID of the initiating peer
	bool supressEvents;		// Suppress events for this recording session
	bool synchronizeVideo;	// Weather or not to synchronize the recorded video.
							// The user configured value will be set when creating
							// options using MediaManager::getRecordingOptions()
	RecordingOptions(const std::string& channel = "") : 
		channel(channel),
		supressEvents(false), 
		synchronizeVideo(false) {}
};


struct RecorderStream: public PacketStream
{
	api::RecordingOptions options;
	RecorderStream(const api::RecordingOptions& options) :
		PacketStream(options.token),
		options(options) {}
};


typedef PacketProcessor Recorder; 
	/// The recorder type, generally an av::IEncoder instance.
	/// May become a full type in the future.


class MediaManager: public MediaManagerBase
{
public:	
	virtual void startRecording(api::RecordingOptions& options) = 0;
		// Starts a new recorder instance from the given options.
		// Calls createRecorder() internally.
		// An exception will be thrown on error.

	virtual bool stopRecording(const std::string& token, bool whiny = true) = 0;
		// Stops the recorder instance matching the given token.
		// Returns true on success, or if whiny is set then an 
		// exception will be thrown on error.

	virtual api::RecorderStream* createRecorder(api::RecordingOptions& options) = 0;
		// Creates a new recorder instance from the given options.
		// Call RecorderStream::start() to start actual recording.
		// Using this method directly, instead of startRecording() 
		// enables custom manipulation of the PacketStream adapters.
		// The RecordingOptions must be correctly populated, 
		// ideally using the getRecordingOptions() method.
		// An exception will be thrown on error.

	virtual api::RecordingOptions getRecordingOptions(const std::string& channel) = 0;
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

	Signal2<const api::RecordingOptions&, api::Recorder*&> InitRecordingEncoder;
		// Provides listeners with the ability to instantiate the recording encoder.
		// If a valid Recorder instance is assigned to the second parameter,
		// it will be used for encoding.
		
	Signal<api::RecorderStream&> RecordingStarted;
	Signal<api::RecorderStream&> RecordingStopped;
		
protected:
	virtual ~MediaManager() = 0 {};
};


#endif // Anionu_Spot_USING_CORE_API


} } } } // namespace scy::anio::spot::api


#endif // Anionu_Spot_API_MediaManager_H