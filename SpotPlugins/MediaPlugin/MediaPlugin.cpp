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


void MediaPlugin::initialize() 
{
	log() << "Initializing" << endl;	

	API::IMediaManager& media = env()->media();

	// Since we will be the default media provider in most
	// cases, we should listen in on the signals at a lower
	// priority so other providers can create the encoder first.
	env()->streaming().SetupStreamingSession += delegate(this, &MediaPlugin::onSetupStreamingSession, -1);
	media.CreateEncoder += delegate(this, &MediaPlugin::onInitializeRecordingEncoder, -1);
		
	/*
	//
	// MJPEG High (Streaming)
	//
	Format mjpegHigh("MJPEG High", "mjpeg", VideoCodec(Codec::MJPEG, "MJPEG", 640, 480, 15));
	mjpegHigh.video.quality = 100;
		

	//
	// MJPEG Low (Streaming)
	//
	Format mjpegLow("MJPEG Low", "mjpeg", VideoCodec(Codec::MJPEG, "MJPEG", 400, 300, 10));
	mjpegLow.video.quality = 70;
	

//#ifdef USING_FFMPEG
	//
	// MP4 (Recording)
	//
	Format mp4("MP4", "mp4", 
		VideoCodec(Codec::MPEG4, "MPEG4", 400, 300, 25), 
		//AudioCodec(Codec::AAC, "AAC")
		AudioCodec(Codec::AC3, "AC3")
	);

	//
	// FLV Low (Streaming)
	//
	Format flvVideo("Flash Video", "flv", 
		VideoCodec(Codec::FLV, "FLV", 400, 300, 10)
		//AudioCodec(Codec::NellyMoser, "NellyMoser", 1, 11025) 
		//AudioCodec(Codec::Speex, "Speex", 1, 16000)
		//AudioCodec(Codec::AAC, "AAC")
		, 100);

	//
	// Flash H264 (Streaming)
	//		
	// Restrictions for MP4 on older iPhone/iPad:
	// H.264 video, up to 1.5 Mbps, 640 by 480 pixels, 30 frames per second,
	// Low-Complexity version of the H.264 Baseline Profile with AAC-LC audio
	// up to 160 Kbps, 48kHz, stereo audio in .m4v, .mp4, and .mov file formats.
	Format flvH264("Flash H264", 
		"flv", VideoCodec(Codec::H264, "H264")//, 
		//AudioCodec(Codec::AAC, "AAC")
		);

	//
	// Speex (Streaming)
	//
	Format flvSpeex("Flash Speex", "flv", 
		//VideoCodec(Codec::FLV, "FLV", 400, 300, 10),
		//AudioCodec(Codec::NellyMoser, "NellyMoser", 1, 11025) 
		AudioCodec(Codec::Speex, "Speex", 1, 16000)
		//AudioCodec(Codec::AAC, "AAC")
		, 100);
//#endif

	//
	// Recording Formats
	//
	{
	log() << "Initializing 3" << endl;	
		media.recordingFormats().clear();
		media.recordingFormats().registerFormat(mjpegHigh);

//#ifdef USING_FFMPEG
		media.recordingFormats().registerFormat(mp4);
		media.recordingFormats().setDefault("MP4");
//#endif
	log() << "Initializing 4" << endl;	
	}
	
	//
	// Local Video Streaming Formats
	//
	{
	log() << "Initializing 5" << endl;	
		media.videoStreamingFormats().clear();
		media.videoStreamingFormats().registerFormat(mjpegHigh);
		
//#ifdef USING_FFMPEG
		media.videoStreamingFormats().registerFormat(flvVideo);
		media.videoStreamingFormats().registerFormat(flvH264);
		media.videoStreamingFormats().setDefault("Flash Video");
//#endif
	log() << "Initializing 6" << endl;	
	}	
	
	//
	// Remote Video Streaming Formats
	//
	{
	log() << "Initializing 7" << endl;	
		media.remoteVideoStreamingFormats().clear();
		media.remoteVideoStreamingFormats().registerFormat(mjpegLow);
		
//#ifdef USING_FFMPEG
		media.remoteVideoStreamingFormats().registerFormat(flvVideo);
		media.remoteVideoStreamingFormats().registerFormat(flvH264);
		media.remoteVideoStreamingFormats().setDefault("Flash Video");
//#endif
	log() << "Initializing 8" << endl;	
	}
	
	//
	// Audio Streaming Formats
	//
	{
	log() << "Initializing 9" << endl;	
		media.audioStreamingFormats().clear();
		media.audioStreamingFormats().registerFormat(flvSpeex);
		media.audioStreamingFormats().setDefault("Flash Speex");
	log() << "Initializing 10" << endl;	
	}
	*/





	/* OLD
	// MP4
	Format mp4("MP4", "mp4", 
		VideoCodec(Codec::MPEG4, "MPEG4", 400, 300, 20)//, 
		//AudioCodec(Codec::AAC, "AAC")
	);

	// FLV
	Format flv("FLV", "flv", 
		VideoCodec(Codec::FLV, "FLV", 400, 300, 15), 
		//AudioCodec(Codec::NellyMoser, "NellyMoser", 1, 11025)
		//AudioCodec(Codec::Speex, "Speex", 1, 16000)
		//AudioCodec(Codec::Speex, "Speex", 2, 44100)
		//AudioCodec(Codec::AAC, "AAC")
		, 100);

	//
	// Recording Formats
	//
	{
		media.recordingFormats().registerFormat(mp4);
	}
		
	//
	// Streaming Formats
	//
	{
		media.videoStreamingFormats().registerFormat(flv);
		media.remoteVideoStreamingFormats().registerFormat(flv);
		media.videoStreamingFormats().setDefault("FLV");
		media.remoteVideoStreamingFormats().setDefault("FLV");

#if HAVE_H264

		// Flash H264 (streaming)
		//		
		// Restrictions for MP4 on older iPhone/iPad:
		// H.264 video, up to 1.5 Mbps, 640 by 480 pixels, 30 frames per second,
		// Low-Complexity version of the H.264 Baseline Profile with AAC-LC audio
		// up to 160 Kbps, 48kHz, stereo audio in .m4v, .mp4, and .mov file formats.
		Format fh264("Flash H264", "flv", 
			VideoCodec(Codec::H264, "H264"), 
			AudioCodec(Codec::AAC, "AAC"));
		
		media.videoStreamingFormats().registerFormat(fh264);
		media.remoteVideoStreamingFormats().registerFormat(fh264);
		media.videoStreamingFormats().setDefault("Flash H264");
		media.remoteVideoStreamingFormats().setDefault("Flash H264");
//#endif
	}
	*/

	/*
	try 
	{

		//setState(PluginState::Enabled);
	} 
	catch (Poco::Exception& exc) 
	{
		//setState(PluginState::Error, exc.displayText());
		throw exc;
	}
	*/
}


void MediaPlugin::uninitialize() 
{	
	log() << "Uninitializing" << endl;
	
	env()->streaming().SetupStreamingSession.detach(this);
	env()->media().CreateEncoder.detach(this);

	/*
	try 
	{
		
		env()->media().recordingFormats().unregisterFormat("MP4");
		env()->media().videoStreamingFormats().unregisterFormat("FLV");
		env()->media().remoteVideoStreamingFormats().unregisterFormat("FLV");
#if HAVE_H264
		env()->media().videoStreamingFormats().unregisterFormat("Flash H264");
		env()->media().remoteVideoStreamingFormats().unregisterFormat("Flash H264");
#endif
	
		//setState(PluginState::Disabled);
	} 
	catch (Poco::Exception& exc) 
	{
		//setState(PluginState::Error, exc.displayText());
		throw exc;
	}
	*/
}


IPacketEncoder* MediaPlugin::createEncoder(const RecordingOptions& options)
{
	log() << "Initializing AV Encoder" << endl;	

	IPacketEncoder* encoder = NULL;
	try {
		encoder = new AVEncoder(options);
		encoder->initialize();
	}
	catch (Exception& exc) {
		log("error") << "Encoder Initialization Failed: " << exc.displayText() << endl;
		if (encoder)
			delete encoder;
		encoder = NULL;
	}
	return encoder;
}


void MediaPlugin::onInitializeRecordingEncoder(void*, const RecordingOptions& options, IPacketEncoder*& encoder)
{
	log() << "Initialize Recording Encoder" << endl;

	if (options.oformat.id != "rawvideo" &&
		encoder == NULL) {
		encoder = createEncoder(options);
	}
}


void MediaPlugin::onSetupStreamingSession(void*, API::IStreamingSession& session, bool&)
{
	log() << "Initialize Streaming Session" << endl;
		
	if (session.options().oformat.id != "rawvideo" &&
		session.stream().getProcessor<IPacketEncoder>() == NULL) {	
	
		RecordingOptions options(
			session.options().iformat, 
			session.options().oformat);
		IPacketEncoder* encoder = createEncoder(options);
		if (encoder)
			session.stream().attach(encoder, 5, true);			
	}
}


} } } // namespace Scy::Anionu::Spot