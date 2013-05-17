#ifndef ANIONU_SPOT_SurveillanceMultipartPacketizer_H
#define ANIONU_SPOT_SurveillanceMultipartPacketizer_H


#include "Sourcey/IPacketizer.h"
#include "Sourcey/Signal.h"
#include "Sourcey/HTTP/MultipartPacketizer.h"
#include "Anionu/MotionDetector.h"


namespace Scy {
namespace Anionu { 
namespace Spot {


class SurveillanceMultipartPacketizer: public HTTP::MultipartPacketizer
{
public:
	SurveillanceMultipartPacketizer(Anionu::MotionDetector& detector) :
		HTTP::MultipartPacketizer("image/jpeg"), _motionDetector(detector)
	{
	}
	
	virtual void writeChunkHTTPHeaders(std::ostringstream& ost)
	{
		HTTP::MultipartPacketizer::writeChunkHTTPHeaders(ost);
		ost << "X-Motion-Level: " << _motionDetector.motionLevel() << "\r\n"
			<< "X-Motion-Threshold: " << _motionDetector.options().motionThreshold << "\r\n"
			<< "X-Motion-State: " << _motionDetector.state().toString() << "\r\n";
	}

	Anionu::MotionDetector& _motionDetector;
};


} } } // namespace Scy::Anionu::Spot


#endif // ANIONU_SPOT_SurveillanceMultipartPacketizer_H
