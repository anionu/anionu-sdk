#ifndef ANIONU_SPOT_SurveillanceMultipartPacketizer_H
#define ANIONU_SPOT_SurveillanceMultipartPacketizer_H


#include "Sourcey/IPacketizer.h"
#include "Sourcey/Signal.h"
#include "Sourcey/HTTP/MultipartPacketizer.h"
#include "Sourcey/Media/MotionDetector.h"


namespace Sourcey {
namespace Spot {


class SurveillanceMultipartPacketizer: public HTTP::MultipartPacketizer
{
public:
	SurveillanceMultipartPacketizer(Media::MotionDetector& detector) :
		HTTP::MultipartPacketizer("image/jpeg"), _motionDetector(detector)
	{
	}
	
	virtual void writeChunkHTTPHeaders(std::ostringstream& ost)
	{
		std::cout << "111111111111111 Initialize Media Connection" << std::endl;
		HTTP::MultipartPacketizer::writeChunkHTTPHeaders(ost);
		ost << "X-Motion-Level: " << _motionDetector.motionLevel() << "\r\n"
			<< "X-Motion-Threshold: " << _motionDetector.options().motionThreshold << "\r\n"
			<< "X-Motion-State: " << _motionDetector.state().toString() << "\r\n";
	}

	Media::MotionDetector& _motionDetector;
};


} } // namespace Sourcey::Spot


#endif // ANIONU_SPOT_SurveillanceMultipartPacketizer_H
