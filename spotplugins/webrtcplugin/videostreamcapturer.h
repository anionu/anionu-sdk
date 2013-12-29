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


#if 0
#endif

#endif