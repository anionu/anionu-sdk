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


#ifndef Anionu_Spot_API_Channel_H
#define Anionu_Spot_API_Channel_H


#include "scy/signal.h"
#include "scy/media/videocapture.h"
#include "scy/media/audiocapture.h"


namespace scy {
namespace av {
	struct Device; }
namespace anio {
namespace spot { 
namespace api { 


class Channel
{
public:
	virtual std::string name() const = 0;
	virtual std::string videoInputFile() const = 0;
	virtual av::Device videoDevice() const = 0;
	virtual av::Device audioDevice() const = 0;
	virtual av::VideoCapture::Ptr videoCapture(bool whiny = false) const = 0;
	virtual av::AudioCapture::Ptr audioCapture(int channels = 2, int sampleRate = 44100, bool whiny = false) const = 0;
	
	virtual void startRecording() = 0;
	virtual void stopRecording() = 0;
	virtual bool isRecording() const = 0;
		
	virtual void activateMode(const std::string& name) = 0;
	virtual void deactivateMode(const std::string& name) = 0;
	virtual bool isModeActive(const std::string& name) const = 0;

	virtual bool valid() const = 0;
		// The channel is considered valid when both
		// a name and video device have been set.
	
	Signal<std::string&> ChannelChanged;
		// Signals change to the internal channel data, namely
		// devices and name. In the case of a name change the
		// signal will broadcast the old name, and the new name
		// will be accessible via the name() method.
		// The Signal sender is the Channel instance.
		
protected:
	virtual ~Channel() {};
};


typedef std::vector<Channel*> ChannelVec;


} } } } // namespace scy::anio::spot::api


#endif // Anionu_Spot_API_Channel_H