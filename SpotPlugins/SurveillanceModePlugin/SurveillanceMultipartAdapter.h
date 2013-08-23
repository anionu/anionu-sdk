#ifndef Anionu_Spot_SurveillanceMultipartAdapter_H
#define Anionu_Spot_SurveillanceMultipartAdapter_H


#include "Sourcey/PacketStream.h"
#include "Sourcey/Signal.h"
#include "Anionu/MotionDetector.h"


namespace scy {
namespace anio { 
namespace spot {


class SurveillanceMultipartAdapter: public PacketProcessor
	/// Our own HTTP multipart adaptor for Surveillance Mode 
	/// live configuration. This enables us to share packet information
	/// with the client without having to create a separate messaging 
	/// connection.
	/// Notice that we haven't included any files from HTTP module so
	/// as to better maintain cross-version compatibility.
{
public:
	std::string contentType;
	anio::MotionDetector& detector;

	SurveillanceMultipartAdapter(anio::MotionDetector& detector) : 
		PacketProcessor(Emitter), contentType("image/jpeg"), detector(detector)
	{
		traceL("SurveillanceMultipartAdapter", this) << "Create" << std::endl;
	}
	
	virtual ~SurveillanceMultipartAdapter() 
	{
		traceL("SurveillanceMultipartAdapter", this) << "Destroy" << std::endl;
	}
	
	virtual void writeChunkHeader(std::ostringstream& ost)
	{
		ost << "--end\r\n"
			<< "Content-Type: " << contentType << "\r\n"
			<< "X-Motion-Level: " << detector.motionLevel() << "\r\n"
			<< "X-Motion-Threshold: " << detector.options().motionThreshold << "\r\n"
			<< "X-Motion-State: " << detector.state().toString() << "\r\n"
			<< "\r\n";
	}
	
	virtual void process(IPacket& packet)
	{
		traceL("SurveillanceMultipartAdapter", this) << "Processing: " << packet.className() << std::endl;
					
		// Write the chunk header
		std::ostringstream header;
		writeChunkHeader(header);

		// Broadcast the HTTP header separately 
		// so we don't need to copy any data.
		emit(header.str());			

		// Proxy the input packet
		emit(packet);
	}

	PacketSignal Emitter;
};


} } } // namespace scy::anio::spot


#endif // Anionu_Spot_SurveillanceMultipartAdapter_H



	
/*

		if (!packet.hasArray())
			throw std::runtime_error("Incompatible packet type");

		try {
			RawPacket& raw = dynamic_cast<RawPacket&>(packet);
		}
		catch(const std::bad_cast&) {
			// Must be a RawPacket type
			throw std::runtime_error("Incompatible packet type");
		}
class SurveillanceMultipartAdapter: public scy::http::MultipartAdapter
{
public:
	SurveillanceMultipartAdapter(anio::MotionDetector& detector) :
		http::MultipartAdapter("image/jpeg"), detector(detector)
	{

		//assert(0 && "fixme"); // use proper http headers
	}
	
	virtual void writeChunkHTTPHeaders(std::ostringstream& ost)
	{
		http::MultipartAdapter::writeChunkHTTPHeaders(ost);
		ost << "X-Motion-Level: " << detector.motionLevel() << "\r\n"
			<< "X-Motion-Threshold: " << detector.options().motionThreshold << "\r\n"
			<< "X-Motion-State: " << detector.state().toString() << "\r\n";
	}

	anio::MotionDetector& detector;
};
*/

