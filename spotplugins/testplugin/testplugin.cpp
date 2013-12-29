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

#include "testplugin.h"
#include "capturemode.h"
#include "anionu/spot/api/environment.h"
#include "anionu/spot/api/synchronizer.h"
#include "anionu/spot/api/client.h"
#include "scy/media/formatregistry.h"


using std::endl;


namespace scy {
namespace anio { 
namespace spot {

	
	// Declare your plugin like so, using SPOT_CORE_PLUGIN or 
	// SPOT_BASE_PLUGIN depending on the API you are implementing.
#ifdef Anionu_Spot_USING_CORE_API
SPOT_CORE_PLUGIN(TestPlugin, "Test Plugin", "0.1.0")
#else
SPOT_BASE_PLUGIN(TestPlugin, "Test Plugin", "0.1.0")
#endif	
	
	// The video file to synchronize with clous storage.
	// The default path is invalid and will generate an error.
#define SYNC_VIDEO_PATH "/path/to/video.mp4"
	
	// Override media encoders for streaming an recording.
	// The FFmpeg shard library (.dll) files must be copied
	// to the plugin folder, otherwise the plugin will fail to load.
	// You can use the pre-compiled FFmpeg binaries from 
	// ~/anionu-abi/ffmpeg/bin, or you can build your own.
#define ENABLE_MEDIA 0 //Anionu_Spot_USING_CORE_API
	
	// Set this to 1 in order to enable h.264 streaming
	// and recording for Spot via FFmpeg. Note that x264
	// support must be compiled into FFmpeg, so you will
	// need to build FFmpeg yourself.
#define ENABLE_H264 0


TestPlugin::TestPlugin(api::Environment& env) :
	api::IModule(env)
{
	// Set the Spot client's default logger instance for debugging,
	// otherwise a new default logger singleton will be created for the
	// plugin process. You could also register a file logger if you want
	// to keep the plugin logs separate from Spot.
	Logger::setInstance(&env.logger());
}


TestPlugin::~TestPlugin()
{	
	Logger::setInstance(NULL, false);
}


bool TestPlugin::load() 
{
	try {
		// Load all your classes and run any system checks here. 
		//	
		// The load() method should never be empty because we want to know as soon
		// as possible if the plugin is not binary compatible with the Spot client. 
		// This is we always call the logger (at minimum), so if everything blows
		// up at this point, we know what we are dealing with.
		//
		DebugL << "Loading" << endl;
		
#if ENABLE_MEDIA
		// Override encoder creation with custom media sources for streaming and recording.
		// Note that we handle these signals at the lowest priority to give other plugins
		// first chance at instantiating media sources and encoders.
		env().streaming().SetupStreamingSources += delegate(this, &TestPlugin::onSetupStreamingSources, -1);
		env().streaming().SetupStreamingEncoders += delegate(this, &TestPlugin::onSetupStreamingEncoders, -1);
		env().media().SetupRecordingSources += delegate(this, &TestPlugin::onSetupRecordingSources, -1);
		env().media().SetupRecordingEncoders += delegate(this, &TestPlugin::onSetupRecordingEncoders, -1);
#endif

		// To test the events system we create a custom "Test Plugin Activated"  
		// event which will notify all peers that the plugin loaded successfully.
		//
		// When the event is created, real-time notifications will be sent to everyone
		// configured to receive notifications for the "Test Plugin Activated" event.
		// See the online tutorial for help configuring notifications.
		//
		// If you are logged into the dashboard, you will see this event displayed in
		// real-time in the sidebar notifications panel, and also on the Events page.
		//		
		std::string message = "The Test Plugin has been activated!";						
#ifdef Anionu_Spot_USING_CORE_API
		// If the core API is being used, we create the event like so:
		env().client().createEvent("Test Plugin Activated", message, anio::Event::Safe);
#else
		// For base API implementations we create the string first and pass the
		// char pointer to Spot so as to avoid sharing STL containers.
		env().clientBase().createEvent("Test Plugin Activated", message.c_str(), 3);
#endif			
	}
	catch (std::exception& exc) {
		ErrorL << "Load failed: " << exc.what() << endl;

		// Swallow exceptions for ABI compatibility reasons.
		// Set the error message to display to the user.
		_error.assign(exc.what());

		// Return false to put the plugin in error state.
		return false;
	}

	// Return true to indicate the plugin loaded properly.
	return true;
}


void TestPlugin::unload() 
{	
	DebugL << "Unloading" << endl;
	
#if ENABLE_MEDIA
	// All allocated memory and delegates should be freed here.
	env().streaming().SetupStreamingSources.detach(this);
	env().streaming().SetupStreamingEncoders.detach(this);
	env().media().SetupRecordingSources.detach(this);
	env().media().SetupRecordingEncoders.detach(this);
#endif
}


api::IMode* TestPlugin::createModeInstance(const char* modeName, const char* channelName)
{
	// Instantiate our Capture Mode for testing.
	assert(strcmp(modeName, "Capture Mode") == 0);
	return new CaptureMode(env(), channelName);
}


const char** TestPlugin::modeNames() const
{
	// Return the list of modes we provide.
	static const char* modeNames[] = { "Capture Mode", NULL };
	return modeNames;
}


void TestPlugin::synchronizeTestVideo()
{
	// This method shows how to synchronize a file with online storage.
	// Be sure to change the video file path for testing.
	DebugL << "Synchronizing test video: " << SYNC_VIDEO_PATH << endl;
	env().synchronizer().sync("Video", SYNC_VIDEO_PATH, "Test Video");
}


bool TestPlugin::onMessage(const char* /* message */)
{
	// Parse the Message and do something with it:
	// smpl::Message m;
	// m.read(message);
	// DebugL << "Recv Message: " << m.toStyledString());
	//
	// Return true if the message has been responded to, 
	// false otherwise.
	return false;
}


bool TestPlugin::onCommand(const char* /* command */)
{
	// Parse the Command and do something with it:
	// smpl::Command c;
	// c.read(command);
	// DebugL << "Recv Command: " << p.toStyledString() << endl;
	//
	// Return true if the command has been handled and
	// responded to, false otherwise.
	return false;
}


void TestPlugin::onEvent(const char* /* event */)
{
	// Parse the Event and do something with it:
	// smpl::Event e;
	// e.read(event);
	// DebugL << "Recv Event: " << p.toStyledString() << endl;
}


void TestPlugin::onPresence(const char* /* presence */) 
{
	// Parse the Presence and do something with it:
	// smpl::Message p;
	// p.read(presence);	
	// DebugL << "Recv Presence: " << p.toStyledString() << endl;
}


const char* TestPlugin::errorMessage() const 
{ 
	// Return a pointer to the error char buffer.
	return _error.empty() ? 0 : _error.c_str();
}


#if ENABLE_MEDIA
//
/// Core API media methods and callbacks
//
void TestPlugin::registerMediaFormats()
{
	DebugL << "Registering custom media formats" << endl;

	// This function registers our custom media formats for 
	// video recording and streaming. Registered formats are  
	// selectable and configurable via the dashboard.	
		
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


bool TestPlugin::createEncoder(PacketStream& stream, av::EncoderOptions& options)
{	
	// Instantiate the encoder and return the pointer
	av::AVPacketEncoder* encoder = nullptr;
	try {
		// Create and initialize the encoder
		encoder = new av::AVPacketEncoder(options);
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


bool TestPlugin::createCaptures(PacketStream& stream, av::EncoderOptions& options, const api::Channel* channel)
{
	try {
		// Initialize video capture
		if (options.oformat.video.enabled) {

			if (options.iformat.video.width == 0 ||
				options.iformat.video.height == 0)
				throw std::runtime_error("No video input dimensions specified.");

			if (options.oformat.video.width == 0 ||
				options.oformat.video.height == 0) {
				throw std::runtime_error("No video output dimensions specified.");	
			}

			// Get the device or throw an exception
			av::VideoCapture::Ptr video(channel->videoCapture(true));	

			// Update the encoder input format from the device
			video->getEncoderFormat(options.iformat);
				
			// Attach video capture
			stream.attachSource<av::VideoCapture>(video, true);
		}
		
		// Initialize audio capture
		if (options.oformat.audio.enabled) {			
	
			// Audio input parameters should match output parameters
			options.iformat.audio = options.oformat.audio;
				
			// Get the device or throw an exception
			av::AudioCapture::Ptr audio = channel->audioCapture(
				options.iformat.audio.channels, 
				options.iformat.audio.sampleRate, true);

			// Update the encoder input format from the device
			audio->getEncoderFormat(options.iformat);

			// Attach audio capture
			stream.attachSource<av::AudioCapture>(audio, true);
		}
	}
	catch (std::exception& exc) {
		ErrorL << "Cannot initialize streaming capture sources: " << exc.what() << endl;
		return false;
	}
	return true;
}


void TestPlugin::onSetupStreamingSources(void*, api::StreamingSession& session, bool& handled)
{
	DebugL << "Initialize streaming captures: " << session.token() << endl;
	
	// Initialize the streaming captures and set the handled flag on success
	handled = TestPlugin::createCaptures(
		session.captureStream(), session.options(), 
		env().channels().getChannel(session.options().channel));
}


void TestPlugin::onSetupStreamingEncoders(void*, api::StreamingSession& session, bool& handled)
{
	DebugL << "Initialize streaming encoder: " << session.token() << endl;
		
	// Create the encoder unless streaming raw video
	if (session.options().oformat.id == "rawvideo" || handled)
		return;
	
	assert(!session.encodeStream().getProcessor<av::IPacketEncoder>());
		
	// Initialize the session and set the handled flag on success
	handled = createEncoder(session.encodeStream(), session.options());
}


void TestPlugin::onSetupRecordingSources(void*, api::RecordingSession& session, bool& handled)
{
	DebugL << "Initialize recording captures: " << session.options.token << endl;
	
	// Initialize the recording captures and set the handled flag on success
	handled = TestPlugin::createCaptures(
		session.stream, session.options, 
		env().channels().getChannel(session.options.channel));
}


void TestPlugin::onSetupRecordingEncoders(void*, api::RecordingSession& session, bool& handled)
{	
	DebugL << "Initialize recording encoder: " << session.options.token << endl;
		
	// Create the encoder unless streaming raw video
	if (session.options.oformat.id == "rawvideo" || handled)
		return;
	
	assert(!session.stream.getProcessor<av::IPacketEncoder>());
		
	// Initialize the session and set the handled flag on success
	handled = createEncoder(session.stream, session.options);
}

#endif


} } } // namespace scy::anio::spot