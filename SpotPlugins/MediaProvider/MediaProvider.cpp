#include "MediaProvider.h"

#include "Sourcey/Spot/IEnvironment.h"
#include "Sourcey/Media/FormatRegistry.h"


using namespace Poco;
using namespace std;
using namespace Sourcey::Spot;
using namespace Sourcey::Media;


POCO_BEGIN_MANIFEST(IPlugin)
	POCO_EXPORT_CLASS(MediaProvider)
POCO_END_MANIFEST


namespace Sourcey {
namespace Spot {


MediaProvider::MediaProvider()
{
}


MediaProvider::~MediaProvider()
{	
}


void MediaProvider::initialize() 
{
	log() << "Initializing" << endl;	

	try 
	{
		// Since we will be the default media provider in most
		// cases, we should listen in on the signals at a lower
		// priority so other providers can create the encoder first.
		env().streaming().InitializeStreamingSession += delegate(this, &MediaProvider::onInitializeStreamingSession, -1);
		env().media().InitializeEncoder += delegate(this, &MediaProvider::onInitializeRecordingEncoder, -1);
			
		// MP4
		Format mp4("MP4", Format::MP4, 
			VideoCodec(Codec::MPEG4, "MPEG4", 400, 300, 20), 
			AudioCodec(Codec::AAC, "AAC")
		);

		// FLV
		Format flv("FLV", Format::FLV, 
			VideoCodec(Codec::FLV, "FLV", 400, 300, 15), 
			//AudioCodec(Codec::NellyMoser, "NellyMoser", 1, 11025)
			AudioCodec(Codec::Speex, "Speex", 1, 16000)
			//AudioCodec(Codec::Speex, "Speex", 2, 44100)
			//AudioCodec(Codec::AAC, "AAC")
			, 100);

		//
		// Recording Formats
		//
		{
			env().media().recordingFormats().registerFormat(mp4);
		}
		
		//
		// Streaming Formats
		//
		{
			env().media().localVideoStreamingFormats().registerFormat(flv);
			env().media().remoteVideoStreamingFormats().registerFormat(flv);
			env().media().localVideoStreamingFormats().setDefault("FLV");
			env().media().remoteVideoStreamingFormats().setDefault("FLV");

#if HAVE_H264

			// Flash H264 (streaming)
			//		
			// Restrictions for MP4 on older iPhone/iPad:
			// H.264 video, up to 1.5 Mbps, 640 by 480 pixels, 30 frames per second,
			// Low-Complexity version of the H.264 Baseline Profile with AAC-LC audio
			// up to 160 Kbps, 48kHz, stereo audio in .m4v, .mp4, and .mov file formats.
			Format fh264("Flash H264", Format::FLV, 
				VideoCodec(Codec::H264, "H264"), 
				AudioCodec(Codec::AAC, "AAC"));
		
			env().media().localVideoStreamingFormats().registerFormat(fh264);
			env().media().remoteVideoStreamingFormats().registerFormat(fh264);
			env().media().localVideoStreamingFormats().setDefault("Flash H264");
			env().media().remoteVideoStreamingFormats().setDefault("Flash H264");
#endif
		}

		//setState(PluginState::Enabled);
	} 
	catch (Poco::Exception& exc) 
	{
		//setState(PluginState::Error, exc.displayText());
		throw exc;
	}
}


void MediaProvider::uninitialize() 
{	
	log() << "Uninitializing" << endl;

	try 
	{
		env().streaming().InitializeStreamingSession -= delegate(this, &MediaProvider::onInitializeStreamingSession);
		env().media().InitializeEncoder -= delegate(this, &MediaProvider::onInitializeRecordingEncoder);
		
		/*
		env().media().recordingFormats().unregisterFormat("MP4");
		env().media().localVideoStreamingFormats().unregisterFormat("FLV");
		env().media().remoteVideoStreamingFormats().unregisterFormat("FLV");
#if HAVE_H264
		env().media().localVideoStreamingFormats().unregisterFormat("Flash H264");
		env().media().remoteVideoStreamingFormats().unregisterFormat("Flash H264");
#endif
		*/
	
		//setState(PluginState::Disabled);
	} 
	catch (Poco::Exception& exc) 
	{
		//setState(PluginState::Error, exc.displayText());
		throw exc;
	}
}


IPacketEncoder* MediaProvider::createEncoder(const RecorderParams& params)
{
	log() << "Initializing AV Encoder" << endl;	

	IPacketEncoder* encoder = NULL;
	try {
		encoder = new AVEncoder(params);
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


void MediaProvider::onInitializeRecordingEncoder(void*, const RecorderParams& params, IPacketEncoder*& encoder)
{
	log() << "Initialize Recording Encoder" << endl;

	if (params.oformat.id != Format::Raw &&
		encoder == NULL) {
		encoder = createEncoder(params);
	}
}


void MediaProvider::onInitializeStreamingSession(void*, IStreamingSession& session, bool&)
{
	log() << "Initialize Streaming Session" << endl;
		
	if (session.params().oformat.id != Format::Raw &&
		session.stream().getProcessor<IPacketEncoder>() == NULL) {	
	
		RecorderParams params(
			session.params().iformat, 
			session.params().oformat);
		IPacketEncoder* encoder = createEncoder(params);
		if (encoder)
			session.stream().attach(encoder, 5, true);			
	}
}


} } // namespace Sourcey::Spot