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

#ifndef Anionu_Spot_SurveillanceMode_H
#define Anionu_Spot_SurveillanceMode_H


#include "anionu/spot/api/imode.h"
#include "anionu/spot/api/imodule.h"
#include "anionu/spot/api/imodule.h"
#include "anionu/spot/api/mediamanager.h"
#include "anionu/spot/api/streamingsession.h"
#include "anionu/spot/api/iformprocessor.h"
#include "anionu/motiondetector.h"
#include "scy/packetstream.h"
#include "scy/timer.h"


namespace scy {
namespace anio { 
namespace spot {
	

class SurveillanceMode: 
	public api::IMode, 
	public api::IModule, 
	public api::IFormProcessor
{
public:
	SurveillanceMode(api::Environment& env, const std::string& channel);
	virtual ~SurveillanceMode();
	
	//
	/// IMode methods
	virtual bool activate();
	virtual void deactivate();
	
	virtual bool isActive() const;

	virtual const char* modeName() const;
	virtual const char* channelName() const;
	virtual const char* errorMessage() const;
	virtual const char* docFile() const;	

	//
	/// IFormProcessor methods
	virtual void buildForm(smpl::Form& form, smpl::FormElement& element);
	virtual void parseForm(smpl::Form& form, smpl::FormElement& element);
	
	//
	/// Local methods
	void loadConfig();

	void startMotionDetector(bool whiny = true);
	void stopMotionDetector(bool whiny = true);
	
	void startRecording(bool whiny = true);
	void stopRecording(bool whiny = true);
	bool isRecording() const;
	
	TimedToken* createStreamingToken(long duration = 20000);
	TimedToken* getStreamingToken(const std::string& token);
	bool removeStreamingToken(const std::string& token);

	//
	/// Callbacks
	void onMotionStateChange(void* sender, anio::MotionDetectorState& state, const anio::MotionDetectorState&);
	void onSetupStreamingSources(void*, api::StreamingSession& session, bool& handled);
	void onSetupStreamingConnection(void*, api::StreamingSession& session, PacketStream& stream, bool& handled);
	void onStreamingSessionStateChange(void*, api::StreamingState& state, const api::StreamingState&);
	
protected:	
	bool _isActive;
	bool _isConfiguring;
	bool _synchronizeVideos;
	std::string	_error;
	std::string _channel;
	std::string	_recordingToken;
	std::vector<TimedToken> _streamingTokens;
	anio::MotionDetector _motionDetector;
	anio::MotionDetector* _tempMotionDetector;
	PacketStream _motionStream;
	mutable Mutex _mutex;
};


} } } // namespace scy::anio::spot


#endif // Anionu_Spot_SurveillanceMode_H