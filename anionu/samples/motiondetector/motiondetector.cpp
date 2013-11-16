#include "scy/base.h"
#include "scy/packetstream.h"
#include "scy/media/mediafactory.h"
#include "scy/media/motiondetector.h"


using namespace std;
using namespace Poco;
using namespace Scy;
using namespace Scy::Media;


// Detect Memory Leaks
/*
#ifdef _DEBUG
#include "MemLeakDetect/MemLeakDetect.h"
CMemLeakDetect memLeakDetect;
#endif<DataPacket>
*/


namespace Scy { 
namespace Media {	


// ----------------------------------------------------------------------------
//
// Motion Detector Stream
//-------
class MotionDetectorStream: public PacketStream
{
public:
	MotionDetectorStream()
	{	
		VideoCapture* video = new VideoCapture(0); //MediaFactory::instance()->video.getCapture(0);		//new VideoCapture(0); //
		attach(video, true);

		Format format;
		AllocateOpenCVInputFormat(video, format);
		assert(format.video.width && format.video.height);
		
		MotionDetector::Options options;
		MotionDetector* detector = new MotionDetector(options);
		attach(detector, 3, true);	

		attach(packetDelegate(this, &MotionDetectorStream::onVideoEncoded));
	}

	virtual ~MotionDetectorStream() 
	{
	}

	void onVideoEncoded(void* sender, MatPacket& packet)
	{
		LogTrace() << "[MotionDetectorApp] Video Encoded" << endl;

		cv::imshow("Motion Image", *packet.mat);
		cv::waitKey(10);
	}
};


} } // namespace Scy::Media


int main(int argc, char** argv)
{
	Logger::instance().add(new ConsoleChannel("debug", TraceLevel));

	MotionDetectorStream test;
	test.start();
	/*	
	MotionDetectorStream test1;
	test1.start();	
	MotionDetectorStream test2;
	test2.start();
	*/
	Util::pause();

	return 0;
}
