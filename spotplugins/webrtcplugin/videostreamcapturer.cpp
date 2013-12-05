#include "videostreamcapturer.h"


using std::endl;


namespace scy {
namespace anio { 
namespace spot {


VideoStreamCapturer::VideoStreamCapturer(PacketSignal& emitter) :
	_emitter(emitter)
{	
    // Default supported formats. Use ResetSupportedFormats to over write.
    std::vector<cricket::VideoFormat> formats;
    formats.push_back(cricket::VideoFormat(1280, 720,
        cricket::VideoFormat::FpsToInterval(30), cricket::FOURCC_I420));
    formats.push_back(cricket::VideoFormat(640, 480,
        cricket::VideoFormat::FpsToInterval(30), cricket::FOURCC_I420));
    formats.push_back(cricket::VideoFormat(320, 240,
        cricket::VideoFormat::FpsToInterval(30), cricket::FOURCC_I420));
    formats.push_back(cricket::VideoFormat(160, 120,
        cricket::VideoFormat::FpsToInterval(30), cricket::FOURCC_I420));
}


VideoStreamCapturer::~VideoStreamCapturer() 
{
}


cricket::CaptureState VideoStreamCapturer::Start(const cricket::VideoFormat& capture_format)
{
	try { 
		if (capture_state() == cricket::CS_RUNNING) {
			WarnL << "Start called when it's already started." << endl;
			return capture_state();
		}
		InfoL << "Start" << endl;

		// TODO: Honour cricket::VideoFormat?

		// Connect and start the mpacket stream.
		// Output packets must be av::MatrixPacket types so we can access
		// the underlying cv::Mat. 
		_emitter += packetDelegate(this, &VideoStreamCapturer::onFrameCaptured); 

		SetCaptureFormat(&capture_format);
		return cricket::CS_RUNNING;
	} catch (...) {}
	return cricket::CS_FAILED;
}


void VideoStreamCapturer::Stop()
{
	try { 
		if (capture_state() == cricket::CS_STOPPED) {
			WarnL << "Stop called when it's already stopped." << endl;
			return;
		}
		InfoL << "Stop" << endl;

		_emitter.detach(this); // for cleanup()

		SetCaptureFormat(NULL);
		SetCaptureState(cricket::CS_STOPPED);
		return;
	} catch (...) {}
	return;
}


void VideoStreamCapturer::onFrameCaptured(void* sender, av::MatrixPacket& packet) 
{	
	// Note: CV_BGR2YUV_I420 to conversion is fixed for now.
	// This function should updated to check the source packet 
	// fourcc on the next iteration.
	cv::Mat yuv(packet.width, packet.height, CV_8UC4);
	cv::cvtColor(*packet.mat, yuv, CV_BGR2YUV_I420);

	cricket::CapturedFrame frame;
	frame.width = packet.width;
	frame.height = packet.height;
	frame.fourcc = cricket::FOURCC_I420;
	frame.data_size = yuv.rows * yuv.step;
	frame.data = yuv.data;

	SignalFrameCaptured(this, &frame);
}


bool VideoStreamCapturer::IsRunning()
{
	return capture_state() == cricket::CS_RUNNING;
}


bool VideoStreamCapturer::GetPreferredFourccs(std::vector<uint32>* fourccs)
{
	if (!fourccs)
		return false;
	fourccs->push_back(cricket::FOURCC_I420);
	return true;
}


bool VideoStreamCapturer::GetBestCaptureFormat(const cricket::VideoFormat& desired, cricket::VideoFormat* best_format)
{
	if (!best_format)
		return false;

	// VideoStreamCapturer does not support capability enumeration.
	// Use the desired format as the best format.
	best_format->width = desired.width;
	best_format->height = desired.height;
	best_format->fourcc = cricket::FOURCC_I420;
	best_format->interval = desired.interval;
	return true;
}


bool VideoStreamCapturer::IsScreencast() const 
{
	return false;
}


} } } // namespace scy::anio::spot