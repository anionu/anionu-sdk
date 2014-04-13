//
// LibSourcey
// Copyright (C) 2005, Sourcey <http://sourcey.com>
//
// LibSourcey is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// LibSourcey is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.
//


#include "anionu/motiondetector.h"

#if defined(HAVE_OPENCV) && defined(HAVE_FFMPEG)

using namespace cv;
using namespace std;
using namespace scy::av;


namespace scy {
namespace anio {


MotionDetector::MotionDetector(const Options& options) : 
	PacketProcessor(this->emitter),
	_options(options),
	_processing(false),
	_motionLevel(0),
	_motionFrameCount(0),
	_motionCanStartAt(0), 
	_motionStartedAt(0), 
	_motionSegmentEndingAt(0)
{
}


MotionDetector::~MotionDetector() 
{
	assert(!isProcessing());
}


void MotionDetector::onStreamStateChange(const PacketStreamState&)
{
	DebugLS(this) << "Reset Stream State" << endl;
	
	setState(this, MotionDetectorState::Idle);

	Mutex::ScopedLock lock(_mutex); 
	_motionLevel = 0;
	_motionFrameCount = 0;
	_motionCanStartAt = 0; 
	_motionStartedAt = 0; 
	_motionSegmentEndingAt = 0;
}


void MotionDetector::process(IPacket& packet)
{
	TraceLS(this) << "Processing" << endl;

	MatrixPacket* mpacket = dynamic_cast<MatrixPacket*>(&packet);		
	if (!mpacket) {
		assert(0 && "unknown packet");
		emit(packet);
		return;
	}
	
	if (mpacket->mat == NULL)
		throw std::runtime_error("Video packets must contain an OpenCV image.");

	VideoPacket opacket;
	cv::Mat& source = *mpacket->mat;
	cv::Mat mask(source.size(), CV_8UC1);
	{
		Mutex::ScopedLock lock(_mutex); 	
		_processing = true;
		_timestamp = (double)clock() / CLOCKS_PER_SEC; // mpacket->time;
	}
	updateMHI(source);
	computeMotionState();			
	{
		Mutex::ScopedLock lock(_mutex); 

		// Create the single channel mask.
		_mhi.convertTo(mask, CV_8UC1, 255.0 / _options.stableMotionLifetime, 
			(_options.stableMotionLifetime - _timestamp) * (255.0 / _options.stableMotionLifetime));

#if 0
		cv::imshow("Mask", mask);
		cv::imshow("MHI", _mhi);
		cv::waitKey(10);
#endif

		opacket = MatrixPacket(&mask);
		_processing = false;	
	}

	// NOTE: Dispatched images are GRAY8 so encoders will
	// need to adjust the input pixel format accordingly.
	emit(opacket);
}


void MotionDetector::updateMHI(cv::Mat& source)
{
	Mutex::ScopedLock lock(_mutex); 

	cv::Mat grey; //(source.size(), CV_8UC1);
	cv::cvtColor(source, grey, CV_RGB2GRAY);

	if (_last.type() != CV_8UC1 ||
		_last.size() != grey.size())
		cv::cvtColor(source, _last, CV_RGB2GRAY);

	cv::absdiff(_last, grey, _silhouette);
	cv::threshold(_silhouette, _silhouette, _options.minPixelValue, 255, cv::THRESH_BINARY);
	_last = grey;	
		
	// Computing motion history	
	if (!_options.useFastAlgorithm) {
		if (_mhi.size() != source.size() ||
			_mhi.type() != CV_32FC1)
			_mhi = cv::Mat(source.size(), CV_32FC1);

		cv::updateMotionHistory(_silhouette, _mhi, _timestamp, _options.stableMotionLifetime);
		_motionLevel = countNonZero(_mhi);
	}
}


void MotionDetector::computeMotionState() 
{ 
	//DebugLS(this) 
	//	<< "Update Motion State: " << state() << endl;	
	Mutex::ScopedLock lock(_mutex); 

	std::time_t currentTime = time::now();
		
	switch (state().id()) {
	case MotionDetectorState::Idle:
		{
			// The motion detector has just started, compute
			// our start time and set our state to Waiting...
			_motionCanStartAt = currentTime + _options.preSurveillanceDelay;
			setState(this, MotionDetectorState::Waiting);
			
			DebugLS(this) << "Updating State: Set => Waiting" << endl;
		}
		break;
	case MotionDetectorState::Waiting: 
		{
			// If the timer is expired then set our state back to Vigilant...
			if (currentTime > _motionCanStartAt) {				
				setState(this, MotionDetectorState::Vigilant);			
				DebugLS(this) << "Updating State: Set => Vigilant" << endl;
			}
		}
		break;
	case MotionDetectorState::Vigilant:
		{
			// If motion threshold is exceeded then set our state to Triggered...
			if (_motionLevel > _options.motionThreshold) {
				DebugLS(this) << "Updating State: Motion Detected: " 
					<< _motionLevel << ">" << _options.motionThreshold << endl;

				// We need x number of motion frames before we 
				// consider motion stable and trigger the alarm.				
		
				// Our current motion count will be valid for 
				// 'stable motion duration' of nanoseconds.
				if (_motionFrameCount == 0) _stopwatch.start();
				if (_stopwatch.elapsed() < _options.stableMotionLifetime * 1000000) {
					_motionFrameCount++;
										
					// Stable motion detected, set our state to Triggered
					if (_motionFrameCount >= _options.stableMotionNumFrames) {	
						_motionFrameCount = 0;
						_stopwatch.stop();
						_stopwatch.reset();						
						_motionStartedAt = currentTime;
						_motionSegmentEndingAt = min<time_t>(
							currentTime + _options.minTriggeredDuration, 				
							_motionStartedAt + _options.maxTriggeredDuration);
						setState(this, MotionDetectorState::Triggered);
						DebugLS(this) << "Updating State: Vigilant => Triggered" << endl;
					}
				} else {
					_stopwatch.stop();
					_stopwatch.reset();
					_motionSegmentEndingAt = 0;
					_motionFrameCount = 0;
					DebugLS(this) << "Updating State: Motion Timer Expired" << endl;
				}			
			}
		}
		break;
	case MotionDetectorState::Triggered: 
		{
			// If the timer is expired then set our state back to Waiting...
			if (currentTime > _motionSegmentEndingAt) {				
				_motionCanStartAt = currentTime + _options.postMotionEndedDelay;
				setState(this, MotionDetectorState::Waiting);
				DebugLS(this) << "Updating State: Triggered => Waiting" << endl;
			}

			// If motion threshold is exceeded while triggered then extend the timer...
			else if (_motionLevel > _options.motionThreshold) {
				_motionSegmentEndingAt = min<time_t>(
					currentTime + _options.minTriggeredDuration, 				
					_motionStartedAt + _options.maxTriggeredDuration);	
				DebugLS(this) << "Updating State: Extend Triggered Duration: " 
					<< _motionSegmentEndingAt << endl;			
			}
		}
		break;
	}
}


bool MotionDetector::accepts(IPacket& packet) 
{ 
	return dynamic_cast<const MatrixPacket*>(&packet) != 0;
}


int MotionDetector::motionLevel() const
{
	Mutex::ScopedLock lock(_mutex); 
	return _motionLevel;
}


void MotionDetector::setOptions(Options& opts)
{
	Mutex::ScopedLock lock(_mutex); 
	_options = opts;
}


MotionDetector::Options& MotionDetector::options()
{
	Mutex::ScopedLock lock(_mutex); 
	return _options;
}


bool MotionDetector::isProcessing() const 
{ 
	Mutex::ScopedLock lock(_mutex); 
	return _processing;
}


bool MotionDetector::isActive() const 
{ 
	Mutex::ScopedLock lock(_mutex); 
	return !stateEquals(MotionDetectorState::Idle);
}


} } // namespace scy::anio

#endif

