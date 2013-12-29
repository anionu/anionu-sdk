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

#ifndef Anionu_Spot_RecordingMode_H
#define Anionu_Spot_RecordingMode_H


#include "anionu/spot/api/imode.h"
#include "anionu/spot/api/imodule.h"
#include "anionu/spot/api/channel.h"
#include "anionu/spot/api/mediamanager.h"
#include "anionu/spot/api/iformprocessor.h"


namespace scy {
namespace anio { 
namespace spot {


class RecordingMode: 
	public api::IMode, 
	public api::IModule, 
	public api::IFormProcessor
{
public:
	RecordingMode(api::Environment& env, const std::string& channel);
	virtual ~RecordingMode();
	
	virtual bool activate();
	virtual void deactivate();
		
	void startRecording();
	void stopRecording();
	
	void loadConfig();
	bool isActive() const;
	bool isRecording() const;
	std::string recordingToken();
		
	virtual void buildForm(smpl::Form& form, smpl::FormElement& element);
	virtual void parseForm(smpl::Form& form, smpl::FormElement& element);
		
	void onRecordingStopped(void* sender, api::RecordingSession& recorder);
	
	virtual const char* docFile() const;	
	virtual const char* errorMessage() const;
	virtual const char* channelName() const;
	virtual const char* modeName() const { return "Recording Mode"; }

private: 
	int	_segmentDuration;
	bool _synchronizeVideos;
	bool _isActive;
	std::string _error;
	std::string _channel;
	std::string	_recordingToken;
	mutable Mutex _mutex;
};


} } } // namespace scy::anio::spot


#endif // Anionu_Spot_RecordingMode_H



	/*
	//
	/// smpl::IFormProcessor methods
	//bool isConfigurable() const;
	//bool hasParsableFields(smpl::Form& form) const;
	
	//void initialize();
	//void uninitialize();
	bool isConfigurable() const;
	bool hasParsableFields(smpl::Form& form) const;	
	std::string docFile();

	void buildForm(smpl::Form& form, smpl::FormElement& element, bool defaultScope = false);
	void parseForm(smpl::Form& form, smpl::FormElement& element);
	*/
	
	//void onRecordingStarted(void* sender, api::RecorderStream& stream);
	//void onEncoderStateChange(void* sender, av::EncoderState& state, const av::EncoderState& oldState);
	//Signal2<const api::EncoderOptions&, api::RecordingStream*&> RecordingStarted;
	//Signal2<const api::EncoderOptions&, api::RecordingStream*&> RecordingStopped;
	//env().media().RecordingStarted += delegate(this, &RecordingMode::onRecordingStarted);
	//env().media().RecordingStopped += delegate(this, &RecordingMode::onRecordingStopped);