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

#include "mediaplugin.h"
#include "anionu/spot/api/environment.h"
#include "anionu/spot/api/synchronizer.h"
#include "anionu/spot/api/client.h"
#include "scy/media/formatregistry.h"
#include "scy/media/thumbnailer.h"


using std::endl;


namespace scy {
namespace anio { 
namespace spot {


SPOT_CORE_PLUGIN(
	MediaPlugin, 
	"Media Plugin", 
	"0.9.0"
)


MediaPlugin::MediaPlugin(api::Environment& env) :
	api::IModule(env)
{
	Logger::setInstance(&env.logger());
}


MediaPlugin::~MediaPlugin()
{	
	Logger::setInstance(NULL, false);
}


bool MediaPlugin::load() 
{
	try {
		DebugL << "Loading" << endl;

		// Override encoder creation for streaming and recording
		env().streaming().SetupStreamingEncoders += delegate(this, &MediaPlugin::onSetupStreamingEncoders);
		env().media().SetupRecordingEncoders += delegate(this, &MediaPlugin::onSetupRecordingEncoders);

		// Override thumbnail creation
		env().media().CreateThumbnail += delegate(this, &MediaPlugin::onCreateThumbnail);

		// Register our custom media formats
		registerCustomMediaFormats();
	}
	catch (std::exception& exc) {
		ErrorL << "Load failed: " << exc.what() << endl;
		_error.assign(exc.what());
		return false;
	}

	return true;
}


void MediaPlugin::unload() 
{	
	DebugL << "Unloading" << endl;
	
	env().streaming().SetupStreamingEncoders.detach(this);
	env().media().SetupRecordingEncoders.detach(this);
	env().media().CreateThumbnail.detach(this);
	env().media().resetDefaultMediaFormats();
}


bool MediaPlugin::createEncoder(PacketStream& stream, av::EncoderOptions& options)
{	
	av::AVPacketEncoder* encoder = nullptr;
	try {
		// Create and initialize the encoder
		encoder = new av::AVPacketEncoder(options, 
			options.oformat.video.enabled && 
			options.oformat.audio.enabled);
		encoder->initialize(); // may throw
		
		// Attach the encoder to the packet stream
		// The PacketStream will take ownership of the encoder
		stream.attach(encoder, 5, true);	
	}
	catch (std::exception& exc) {
		ErrorL << "Encoder initialization failed: " << exc.what() << endl;
		if (encoder)
			delete encoder;
		encoder = nullptr;
	}
	return !!encoder;
}


void MediaPlugin::onSetupStreamingEncoders(api::StreamingSession& session, bool& handled)
{
	DebugL << "Initialize streaming encoder: " << session.token() << endl;
		
	// Create the encoder unless streaming raw video
	if (session.options().oformat.id == "rawvideo" || handled)
		return;
	
	assert(!session.encodeStream().getProcessor<av::IPacketEncoder>());
		
	// Initialize the session and set the handled flag on success
	handled = createEncoder(session.encodeStream(), session.options());
}


void MediaPlugin::onSetupRecordingEncoders(api::RecordingSession& session, bool& handled)
{	
	DebugL << "Initialize recording encoder: " << session.options.token << endl;
		
	// Create the encoder unless streaming raw video
	if (session.options.oformat.id == "rawvideo" || handled)
		return;
	
	assert(!session.stream.getProcessor<av::IPacketEncoder>());
		
	// Initialize the session and set the handled flag on success
	handled = createEncoder(session.stream, session.options);
}


void MediaPlugin::onCreateThumbnail(const av::ThumbnailerOptions& options, bool& handled)
{
	DebugL << "Create thumbnail: " << options.ifile << endl;
		
	// Create the thumbnail only if required
	if (handled)
		return;
	
	try {
		av::Thumbnailer thumb(options);			
		thumb.open(); // output default thumb: (ofile)_thumb.jpg
		thumb.grab();
		handled = true;
	}
	catch (std::exception& exc) {
		ErrorL << "Create thumbnail failed: " << exc.what() << endl;
		handled = false;
	}
}


void MediaPlugin::registerCustomMediaFormats()
{
	DebugL << "Registering custom media formats" << endl;

	// This function registers our custom media formats for 
	// video recording and streaming. Registered formats are  
	// selectable and configurable via the dashboard.

#define ENABLE_H264 0
	// Set this to 1 in order to enable h.264 streaming
	// and recording for Spot via FFmpeg. Note that x264
	// support must be compiled into FFmpeg.
		
	//
	// MP4 (Recording)
	av::Format mp4("MP4", "mp4", 
		av::VideoCodec("MPEG4", "mpeg4", 480, 360, 25), 
		av::AudioCodec("AC3", "ac3_fixed")
	);

#if ENABLE_H264
	//
	// MP4 H.264 (Recording)
	av::Format mp4h264("MP4 (h.264)", "mp4", 
		av::VideoCodec("H.264", "libx264", 480, 360, 25), 
		av::AudioCodec("AC3", "ac3_fixed")
	);

	//
	// Flash H.264 (Streaming)
	//
	// Restrictions for MP4 on older iPhone/iPad:
	// H.264 video, up to 1.5 Mbps, 640 by 480 pixels, 30 frames per second,
	// Low-Complexity version of the H.264 Baseline Profile with AAC-LC audio
	// up to 160 Kbps, 48kHz, stereo audio in .m4v, .mp4, and .mov file formats.
	av::Format flvH264("Flash H.264", "flv", 
		av::VideoCodec("H.264", "libx264"));
#endif

	//
	// FLV Video (Streaming)
	av::Format flvVideo("Flash Video", "flv", 
		av::VideoCodec("FLV", "flv", 480, 360, 10), 100);

	//
	// Speex (Streaming)
	av::Format flvSpeex("Flash Speex", "flv", 
		av::AudioCodec("Speex", "libspeex", 1, 16000), 100);

	//
	// MP3 Mono (Streaming)
	av::Format flvMonoMP3("Flash Mono MP3", "flv", 
		av::AudioCodec("MP3", "libmp3lame", 1, 8000, 64000), 100);

	api::MediaManager& media = env().media();

	//
	/// Register Recording Formats
	media.recordingFormats().registerFormat(mp4);
#if ENABLE_H264
	media.recordingFormats().registerFormat(mp4h264);
	media.recordingFormats().setDefault("MP4 (h.264)");
#else
	media.recordingFormats().setDefault("MP4");
#endif
	
	//
	/// Register Video Streaming Formats
	media.videoStreamingFormats().registerFormat(flvVideo);
#if ENABLE_H264
	media.videoStreamingFormats().registerFormat(flvH264);
	media.videoStreamingFormats().setDefault("Flash H.264");
#else
	media.videoStreamingFormats().setDefault("Flash Video");
#endif
		
	//
	/// Register Audio Streaming Formats
	media.audioStreamingFormats().registerFormat(flvSpeex);
	media.audioStreamingFormats().registerFormat(flvMonoMP3);
	media.audioStreamingFormats().setDefault("Flash Speex");
}


const char* MediaPlugin::errorMessage() const 
{ 
	return _error.empty() ? 0 : _error.c_str();
}


} } } // namespace scy::anio::spot