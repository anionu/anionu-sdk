#ifndef SOURCEY_MEDIA_MatrixPacketizer_H
#define SOURCEY_MEDIA_MatrixPacketizer_H


#include "Sourcey/IPacketizer.h"
#include "Sourcey/Signal.h"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
}

namespace Sourcey { 
namespace Media {


class MatrixPacketizer: public IPacketizer
{
public:
	MatrixPacketizer() : 
		convCtx(NULL)
	{
	}
	
	virtual bool accepts(IPacket& packet) 
	{ 
		return dynamic_cast<const Media::VideoPacket*>(&packet) != 0; 
	}

	virtual void process(IPacket& packet)
	{
		VideoPacket& vpacket = reinterpret_cast<VideoPacket&>(packet);
		VideoDecoderContext* video = reinterpret_cast<VideoDecoderContext*>(packet.opaque);	
		if (video == NULL)
			throw Exception("Video packets must contain a VideoDecoderContext");		
	
		// Convert the image from its native format to BGR.
		if (convCtx == NULL) {
			convCtx = sws_getContext(
				video->codec->width, video->codec->height, video->codec->pix_fmt, 
				video->codec->width, video->codec->height, PIX_FMT_BGR24, 
				SWS_BICUBIC, NULL, NULL, NULL);
			mat.create(video->codec->height, video->codec->width, CV_8UC(3));
		}
		if (convCtx == NULL)
			throw Exception("Matrix Packetizer: Unable to initialize the conversion context");	
			
		// Scales the data in src according to our settings in our SwsContext.
		if (sws_scale(convCtx,
			video->iframe->data, video->iframe->linesize, 0, video->codec->height,
			video->oframe->data, video->oframe->linesize) < 0)
			throw Exception("Matrix Packetizer: Pixel format conversion not supported");

		// Populate the OpenCV Matrix.
		for (int y = 0; y < video->codec->height; y++) {
			for (int x = 0; x < video->codec->width; x++) {
				mat.at<cv::Vec3b>(y,x)[0]=video->oframe->data[0][y * video->oframe->linesize[0] + x * 3 + 0];
				mat.at<cv::Vec3b>(y,x)[1]=video->oframe->data[0][y * video->oframe->linesize[0] + x * 3 + 1];
				mat.at<cv::Vec3b>(y,x)[2]=video->oframe->data[0][y * video->oframe->linesize[0] + x * 3 + 2];
			}
		}

		vpacket.mat = &mat;
		dispatch(this, vpacket);
	}

	cv::Mat mat;
	struct SwsContext* convCtx;

	//std::string contentType;
};


} } // namespace Sourcey::MEDIA


#endif
