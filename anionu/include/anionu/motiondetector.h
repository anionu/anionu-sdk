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


#ifndef Anionu_MotionDetector_H
#define Anionu_MotionDetector_H


#include "scy/base.h"
#include "scy/stateful.h"
#include "scy/signal.h"
#include "scy/interface.h"
#include "scy/packetstream.h"
#include "scy/timer.h"

#if defined(HAVE_OPENCV) && defined(HAVE_FFMPEG)

#include "scy/media/videocapture.h"


#include <opencv/cv.h>
#include <opencv/highgui.h>


namespace scy {
namespace anio {


struct MotionDetectorState: public State
{
	enum Type
	{
		Idle,
		Waiting,
		Vigilant,
		Triggered
	};
	
	std::string str(unsigned int id) const 
	{ 	
		switch (id) {
		case Idle:		return "Idle";
		case Waiting:	return "Waiting";
		case Vigilant:	return "Vigilant";
		case Triggered: return "Triggered";
		}
		return "None";
	};
};


class MotionDetector: public Stateful<MotionDetectorState>, public PacketProcessor
{
public:
	struct Options 
	{	
		double motionThreshold;
		int minPixelValue;
		int preSurveillanceDelay;
		int postMotionEndedDelay;
		int minTriggeredDuration;
		int maxTriggeredDuration;	
		int stableMotionNumFrames;
		double stableMotionLifetime;	
		int captureFPS;
		bool useFastAlgorithm;
		Options() {		
			motionThreshold			= 10000;	// 10000 [50 - 500000]
			minPixelValue			= 30;		// 30 [0 - 255]
			preSurveillanceDelay	= 0;		// 0 seconds
			postMotionEndedDelay	= 3;		// 3 seconds
			minTriggeredDuration	= 5;		// 5 seconds
			maxTriggeredDuration	= 60;		// 60 seconds (1 min)
			stableMotionNumFrames	= 3;		// 3 frames
			stableMotionLifetime	= 1.0;		// 1 second
			captureFPS				= 10;		// 10 frames per second
			useFastAlgorithm		= false;	// disabled
		}
	};

public:
	MotionDetector(const MotionDetector::Options& options = MotionDetector::Options());
	virtual ~MotionDetector();
	
	virtual int motionLevel() const;
	virtual Options& options();	
	virtual void setOptions(Options& opts);	
	
	virtual bool isActive() const;
	virtual bool isProcessing() const;

	virtual bool accepts(IPacket& packet);
	virtual void process(IPacket& packet);
			
	PacketSignal emitter;

protected:
	void updateMHI(cv::Mat& source);
	void computeMotionState();	

	virtual void onStreamStateChange(const PacketStreamState&);	

private:
	mutable Mutex _mutex;

	Options _options;
	Stopwatch _stopwatch;	
	bool	_processing;	
	
	int		_motionLevel;	// The current motion level (non zero pixel count in MHI)
	time_t	_motionCanStartAt;
	time_t	_motionStartedAt;
	time_t	_motionSegmentEndingAt;
	int		_motionFrameCount;
	double	_timestamp;

	cv::Mat _last;			// Last image for comparison
	cv::Mat _mhi;			// Motion history image
	cv::Mat _silhouette;	// The threshold image
};


} } // namespace scy::anio


#endif
#endif