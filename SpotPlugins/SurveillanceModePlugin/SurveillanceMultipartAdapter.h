#ifndef Anionu_Spot_SurveillanceMultipartAdapter_H
#define Anionu_Spot_SurveillanceMultipartAdapter_H


#include "Sourcey/PacketStream.h"
#include "Sourcey/Signal.h"
#include "Sourcey/HTTP/Packetizers.h"
#include "Anionu/MotionDetector.h"


namespace scy {
namespace anionu { 
namespace spot {

	
/*
class SurveillanceMultipartAdapter: public scy::http::MultipartAdapter
{
public:
	SurveillanceMultipartAdapter(anionu::MotionDetector& detector) :
		http::MultipartAdapter("image/jpeg"), _motionDetector(detector)
	{
		assert(0 && "fixme"); // use proper http headers
	}
	
	virtual void writeChunkHTTPHeaders(std::ostringstream& ost)
	{
		http::MultipartAdapter::writeChunkHTTPHeaders(ost);
		ost << "X-Motion-Level: " << _motionDetector.motionLevel() << "\r\n"
			<< "X-Motion-Threshold: " << _motionDetector.options().motionThreshold << "\r\n"
			<< "X-Motion-State: " << _motionDetector.state().toString() << "\r\n";
	}

	anionu::MotionDetector& _motionDetector;
};
*/


} } } // namespace scy::anionu::Spot


#endif // Anionu_Spot_SurveillanceMultipartAdapter_H
