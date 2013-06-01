#include "MediaPlugin.h"

#include "Anionu/Spot/API/IEnvironment.h"
#include "Sourcey/Media/FormatRegistry.h"


using namespace std;
using namespace Poco;
using namespace Scy::Media;


DEFINE_SPOT_PLUGIN(Scy::Anionu::Spot::MediaPlugin)


namespace Scy {
namespace Anionu { 
namespace Spot {


MediaPlugin::MediaPlugin()
{
}


MediaPlugin::~MediaPlugin()
{	
}


bool MediaPlugin::load() 
{
	log("Loading");

	// The Media Plugin shows how to override encoder creation
	// with custom media sources for both streaming and recording, 
	// 
	// The Media Plugin will act as the default media provider,
	// So we listen in on the signals at a lower (-1) priority to   
	// give other plugins the opportunity to create the encoder first.
	env()->streaming().InitStreamingSession += delegate(this, &MediaPlugin::onInitStreamingSession, -1);
	env()->media().InitRecordingEncoder += delegate(this, &MediaPlugin::onInitRecordingEncoder, -1);
	return true;
}


void MediaPlugin::unload() 
{	
	log("Unloading");
	
	// Detach all signal delegates.
	env()->streaming().InitStreamingSession.detach(this);
	env()->media().InitRecordingEncoder.detach(this);
}


IPacketEncoder* MediaPlugin::createEncoder(const Media::RecordingOptions& options)
{
	log("Initializing AV Encoder");	

	// Create the encoder if possible.
	// The returned encoder instance must 
	// extend from IPacketEncoder.
	IPacketEncoder* encoder = NULL;
	try {
		encoder = new AVEncoder(options);
		encoder->initialize();
	}
	catch (Exception& exc) {
		log("Encoder Initialization Failed: " + exc.displayText(), "error");
		if (encoder)
			delete encoder;
		encoder = NULL;
	}
	return encoder;
}


void MediaPlugin::onInitRecordingEncoder(void*, const API::RecordingOptions& options, IPacketEncoder*& encoder)
{
	log("Initialize Recording Encoder");

	// Create the encoder unless recording raw video.
	if (options.oformat.id != "rawvideo" &&
		encoder == NULL) {
		encoder = createEncoder(options);
	}
}


void MediaPlugin::onInitStreamingSession(void*, API::IStreamingSession& session, bool&)
{
	log("Initialize Streaming Session");
		
	// Create the encoder unless streaming raw video.
	if (session.options().oformat.id != "rawvideo" &&
		session.stream().getProcessor<IPacketEncoder>() == NULL) {	
	
		RecordingOptions options(
			session.options().iformat, 
			session.options().oformat);
		IPacketEncoder* encoder = createEncoder(options);
		
		// Attach the encoder to the packet stream.
		if (encoder)
			session.stream().attach(encoder, 5, true);			
	}
}


void MediaPlugin::registerMediaFormats()
{
	// This function is currently unused by the plugin,
	// but it shows how you can register your own custom
	// media formats.
		
	//
	// MP4 (Recording)
	Format mp4("MP4", "mp4", 
		VideoCodec("MPEG4", "mpeg4", 480, 360, 25), 
		//AudioCodec("AAC", "aac")
		AudioCodec("AC3", "ac3_fixed")
	);
	
	//
	// MJPEG (Streaming)
	Format mjpeg("MJPEG", "mjpeg", VideoCodec("MJPEG", "mjpeg", 480, 360, 15));
	mjpeg.video.pixelFmt = "yuvj420p";

	//
	// FLV Video (Streaming)
	Format flvVideo("Flash Video", "flv", 
		VideoCodec("FLV", "flv", 480, 360, 10)
		//AudioCodec("Nellymoser", "nellymoser", 1, 11025) 
		//AudioCodec("Speex", "libspeex", 1, 16000)
		//AudioCodec("AAC", "aac")
		, 100);

	//
	// Flash H264 (Streaming)
	// Restrictions for MP4 on older iPhone/iPad:
	// H.264 video, up to 1.5 Mbps, 640 by 480 pixels, 30 frames per second,
	// Low-Complexity version of the H.264 Baseline Profile with AAC-LC audio
	// up to 160 Kbps, 48kHz, stereo audio in .m4v, .mp4, and .mov file formats.
	Format flvH264("Flash H264", "flv", 
		VideoCodec("H264", "libx264")//, 
		//AudioCodec("AAC", "aac")
		);

	//
	// Speex (Streaming)
	Format flvSpeex("Flash Speex", "flv", 
		//VideoCodec("FLV", "flv", 480, 360, 10),
		//AudioCodec("Nellymoser", "nellymoser", 1, 11025) 
		AudioCodec("Speex", "libspeex", 1, 16000)
		//AudioCodec("AAC", "aac")
		, 100);

	//
	// MP3 Mono (Streaming)
	Format flvMonoMP3("Flash Mono MP3", "flv", 
		//VideoCodec("FLV", "flv", 480, 360, 10),
		//AudioCodec("Nellymoser", "nellymoser", 1, 11025) 
		AudioCodec("MP3", "libmp3lame", 1, 8000, 64000)
		//AudioCodec("AAC", "aac")
		, 100);

	API::IMediaManager& media = env()->media();

	//
	/// Register Recording Formats
	media.recordingFormats().registerFormat(mp4);
	media.recordingFormats().setDefault("MP4");
	
	//
	/// Register Video Streaming Formats
	media.videoStreamingFormats().registerFormat(mjpeg);
	media.videoStreamingFormats().registerFormat(flvVideo);
	media.videoStreamingFormats().registerFormat(flvH264);
	media.videoStreamingFormats().setDefault("Flash Video");
		
	//
	/// Register Audio Streaming Formats
	media.audioStreamingFormats().registerFormat(flvSpeex);
	media.audioStreamingFormats().registerFormat(flvMonoMP3);
	media.audioStreamingFormats().setDefault("Flash Speex");
}


} } } // namespace Scy::Anionu::Spot