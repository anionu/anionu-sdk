#include "MediaPlugin.h"
#include "CaptureMode.h"

#include "Anionu/Spot/API/IEnvironment.h"
#include "Anionu/Spot/API/ISynchronizer.h"
#include "Anionu/Spot/API/IClient.h"
#include "Sourcey/Media/FormatRegistry.h"


using namespace std;


DEFINE_SPOT_PLUGIN(scy::anionu::spot::MediaPlugin)


namespace scy {
namespace anionu { 
namespace spot {


MediaPlugin::MediaPlugin()
{
}


MediaPlugin::~MediaPlugin()
{	
	// :)
}


bool MediaPlugin::load() 
{
	try 
	{
		// Set the Spot client's default logger instance for debugging,
		// otherwise a new default logger singleton will be created for the
		// plugin process. You could also register a file logger if you want
		// to keep the plugin logs separate from Spot.
		Logger::setInstance(&env()->logger());

		// Load all your classes and run any system checks here. 
		//	
		// The load() method should never be empty because we want to know as soon
		// as possible if the plugin is not binary compatable with the Spot client. 
		// This is we always call the logger (at minimum), so if everything blows
		// up at this point, we know what we are dealing with.
		//
		log("Loading");

		// Override encoder creation with custom media sources for streaming and recording, 
		env()->streaming().InitStreamingSession += delegate(this, &MediaPlugin::onInitStreamingSession);
		env()->media().InitRecordingEncoder += delegate(this, &MediaPlugin::onInitRecordingEncoder);
	
		// To test the events system we create a custom "Media Plugin Activated"  
		// event which will notify all peers that the plugin loaded successfully.
		//
		// When the event is created, real-time notifications will be sent to everyone
		// configured to receive notifications for the "Media Plugin Activated" event.
		// See the online tutorial for help configuring notifications.
		//
		// If you are logged into the dashboard, you will see this event displayed in
		// real-time in the sidebar notifications panel, and also on the Events page.
		//		
		string message = "The Media Plugin has been activated!";						
#ifdef Anionu_Spot_USING_CORE_API
		// If the core API is being used, we create the event like so:
		env()->client().createEvent("Media Plugin Activated", message, anionu::Event::Safe);
#else
		// For base API implementations we create the string first and pass the
		// char pointer to Spot so as to avoid sharing STL containers.
		env()->clientBase().createEvent("Media Plugin Activated", message.data(), 3);
#endif			
	}
	catch (Exception& exc) 
	{
		// Swallow exceptions for ABI compatability reasons.
		// Set the error message to display to the user.
		_error = exc.message();
		log("Load failed: " + _error, "error");

		// Return false to put the plugin in error state.
		return false;
	}

	// Return true if the plugin loaded properly.
	return true;
}


void MediaPlugin::unload() 
{	
	log("Unloading");

	// All allocated memory, pointers and signals 
	// should be freed here.
	env()->streaming().InitStreamingSession.detach(this);
	env()->media().InitRecordingEncoder.detach(this);
}


api::IMode* MediaPlugin::createModeInstance(const char* modeName, const char* channelName)
{
	// Instantiate our Capture Mode for testing.
	assert(strcmp(modeName, "Capture Mode") == 0);
	return new CaptureMode(*env(), channelName);
}


const char** MediaPlugin::modeNames() const
{
	// Return the list of modes we provide.
	static const char* modeNames[] = { "Capture Mode", NULL };
	return modeNames;
}


void MediaPlugin::synchronizeTestVideo()
{
	// This method shows how to synchronize a file with online storage.
	// Be sure to change the video file path for testing.
	log("Synchronizing test video");	
	env()->synchronizer().sync("/path/to/video.mp4", "Video");
}


bool MediaPlugin::onMessage(const char* message)
{
	// Parse the Message and do something with it:
	// symple::Message m;
	// m.read(message);
	// log("Recv Message: " + m.toStyledString());
	//
	// Return true if the message has been responded to, 
	// false otherwise.
	return false;
}


bool MediaPlugin::onCommand(const char* command)
{
	// Parse the Command and do something with it:
	// symple::Command c;
	// c.read(command);
	// log("Recv Command: " + c.toStyledString());	
	//
	// Return true if the message has been responded to, 
	// false otherwise.
	return false;
}


void MediaPlugin::onEvent(const char* event)
{
	// Parse the Event and do something with it:
	// symple::Event e;
	// e.read(event);
	// log("Recv Event: " + e.toStyledString());
}


void MediaPlugin::onPresence(const char* presence) 
{
	// Parse the Presence and do something with it:
	// symple::Message p;
	// p.read(presence);	
	// log("Recv Presence: " + p.toStyledString());
}


const char* MediaPlugin::errorMessage() const 
{ 
	Poco::FastMutex::ScopedLock lock(_mutex);

	// Return a pointer to the error char buffer.
	return _error.empty() ? 0 : _error.data();
}


#ifdef Anionu_Spot_USING_CORE_API
//
/// Core API media methods and callbacks
//
void MediaPlugin::registerMediaFormats()
{
	log("Registering custom media formats.");

	// This function shows how you can register custom media
	// formats for video recording and streaming. Registered 
	// formats are selectable and configurable via the dashboard.
	// MP: in the labels below is so we can differentiate from
	// internal media formats.
		
	//
	// MP4 (Recording)
	av::Format mp4("MP: MP4", "mp4", 
		av::VideoCodec("MPEG4", "mpeg4", 480, 360, 25), 
		av::AudioCodec("AC3", "ac3_fixed")
	);
	
	//
	// MJPEG (Streaming)
	av::Format mjpeg("MP: MJPEG", "mjpeg", 
		av::VideoCodec("MJPEG", "mjpeg", 480, 360, 15));
	mjpeg.video.pixelFmt = "yuvj420p";

	//
	// FLV Video (Streaming)
	av::Format flvVideo("MP: Flash Video", "flv", 
		av::VideoCodec("FLV", "flv", 480, 360, 10), 100);

	//
	// Flash H264 (Streaming)
	//
	// Restrictions for MP4 on older iPhone/iPad:
	// H.264 video, up to 1.5 Mbps, 640 by 480 pixels, 30 frames per second,
	// Low-Complexity version of the H.264 Baseline Profile with AAC-LC audio
	// up to 160 Kbps, 48kHz, stereo audio in .m4v, .mp4, and .mov file formats.
	av::Format flvH264("MP: Flash H264", "flv", 
		av::VideoCodec("H264", "libx264"));

	//
	// Speex (Streaming)
	av::Format flvSpeex("MP: Flash Speex", "flv", 
		av::AudioCodec("Speex", "libspeex", 1, 16000), 100);

	//
	// MP3 Mono (Streaming)
	av::Format flvMonoMP3("MP: Flash Mono MP3", "flv", 
		av::AudioCodec("MP3", "libmp3lame", 1, 8000, 64000), 100);

	api::IMediaManager& media = env()->media();

	//
	/// Register Recording Formats
	media.recordingFormats().registerFormat(mp4);
	media.recordingFormats().setDefault("MP: MP4");
	
	//
	/// Register Video Streaming Formats
	media.videoStreamingFormats().registerFormat(mjpeg);
	media.videoStreamingFormats().registerFormat(flvVideo);
	media.videoStreamingFormats().registerFormat(flvH264);
	media.videoStreamingFormats().setDefault("MP: Flash Video");
		
	//
	/// Register Audio Streaming Formats
	media.audioStreamingFormats().registerFormat(flvSpeex);
	media.audioStreamingFormats().registerFormat(flvMonoMP3);
	media.audioStreamingFormats().setDefault("MP: Flash Speex");
}


av::IPacketEncoder* MediaPlugin::createEncoder(const av::RecordingOptions& options)
{
	log("Initializing AV Encoder");	
	
	// Create the encoder and return the instance pointer.	 
	// the returned pointer must extend from IPacketEncoder.
	av::IPacketEncoder* encoder = NULL;
	try {
		encoder = new av::AVEncoder(options);
		encoder->initialize();
	}
	catch (Exception& exc) {
		log("Encoder Error: " + exc.message(), "error");
		if (encoder)
			delete encoder;
		encoder = NULL;
	}
	return encoder;
}


void MediaPlugin::onInitRecordingEncoder(void*, const api::RecordingOptions& options, av::IPacketEncoder*& encoder)
{
	log("Initialize Recording Encoder");

	// Create the encoder unless recording raw video.
	if (options.oformat.id != "rawvideo" &&
		encoder == NULL) {
		encoder = createEncoder(options);
	}
}


void MediaPlugin::onInitStreamingSession(void*, api::IStreamingSession& session, bool&)
{
	log("Initialize Streaming Session");
		
	// Create the encoder unless streaming raw video.
	if (session.options().oformat.id != "rawvideo" &&
		session.stream().getProcessor<av::IPacketEncoder>() == NULL) {	
	
		av::RecordingOptions options(
			session.options().iformat, 
			session.options().oformat);
		av::IPacketEncoder* encoder = createEncoder(options);
		
		// Attach the encoder to the packet stream.
		if (encoder)
			session.stream().attach(encoder, 5, true);			
	}
}

#endif


} } } // namespace scy::anionu::Spot