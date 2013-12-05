#ifndef SCY_VideoStreamCaturer_H
#define SCY_VideoStreamCaturer_H


#include "scy/media/videocapture.h"
#include <iostream>
#include <string>
#include "talk/media/base/videocapturer.h"


namespace scy { 
namespace anio { 
namespace spot {


// VideoStreamCapturer implements a simple cricket::VideoCapturer which
// gets decoded remote video frames from media channel.
// It's used as the remote video source's VideoCapturer so that the remote video
// can be used as a cricket::VideoCapturer and in that way a remote video stream
// can implement the MediaStreamSourceInterface.
class VideoStreamCapturer : 
	public cricket::VideoCapturer {
public:
	VideoStreamCapturer(PacketSignal& emitter);
	virtual ~VideoStreamCapturer();

	// cricket::VideoCapturer implementation.
	virtual cricket::CaptureState Start(
		const cricket::VideoFormat& capture_format) OVERRIDE;
	virtual void Stop() OVERRIDE;
	virtual bool IsRunning() OVERRIDE;
	virtual bool GetPreferredFourccs(std::vector<uint32>* fourccs) OVERRIDE;
	virtual bool GetBestCaptureFormat(const cricket::VideoFormat& desired,
		cricket::VideoFormat* best_format) OVERRIDE;
	virtual bool IsScreencast() const OVERRIDE;

private:
	PacketSignal& _emitter;
	
	void onFrameCaptured(void* sender, av::MatrixPacket& packet);

	DISALLOW_COPY_AND_ASSIGN(VideoStreamCapturer);
};


/*
class VideoCapturerFactoryOCV : public cricket::VideoCapturerFactory {
public:
	VideoCapturerFactoryOCV() {}
	virtual ~VideoCapturerFactoryOCV() {}

	virtual cricket::VideoCapturer* Create(const cricket::Device& device) {

		// TODO: WebRTC uses name to instantiate the capture, id always 0.
		return new VideoStreamCapturer(); //util::strtoi<int>(device.id)
	}
};
*/


} } } // namespace scy::anio::spot


#endif