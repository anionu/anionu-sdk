//
// LibSourcey
// Copyright (C) 2005, Sourcey <http://sourcey.com>
//
// LibSourcey is is distributed under a dual license that allows free, 
// open source use and closed source use under a standard commercial
// license.
//
// Non-Commercial Use:
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
// Commercial Use:
// Please contact mail@sourcey.com
//


#ifndef Anionu_Spot_API_IChannel_H
#define Anionu_Spot_API_IChannel_H


#include "Sourcey/Base.h"
#include "Sourcey/Signal.h"


namespace Scy {
namespace Media {
	struct Device;
	class VideoCapture;
	class AudioCapture;}
namespace Anionu {
namespace Spot { 
namespace API { 


class IChannel
{
public:
	virtual std::string name() const = 0;
	virtual std::string videoInputFile() const = 0;
	virtual Media::Device videoDevice() const = 0;
	virtual Media::Device audioDevice() const = 0;
	virtual Media::VideoCapture* videoCapture(bool whiny = false) const = 0;
	virtual Media::AudioCapture* audioCapture(int channels = 2, int sampleRate = 44100, bool whiny = false) const = 0;
	
	virtual void startRecording() = 0;
	virtual void stopRecording() = 0;
	virtual bool isRecording() const = 0;
		
	virtual void activateMode(const std::string& name) = 0;
	virtual void deactivateMode(const std::string& name) = 0;
	virtual bool isModeActive(const std::string& name) const = 0;

	virtual bool valid() const = 0;
		/// The channel is considered valid when both
		/// a name and video device have been set.
	
	Signal<std::string&> ChannelChanged;
		/// Signals change to the internal channel data, namely
		/// devices and name. In the case of a name change the
		/// signal will broadcast the old name, and the new name
		/// will be accessible via the name() method.
		
protected:
	virtual ~IChannel() = 0 {};
};


typedef std::vector<IChannel*> IChannelList;


} } } } // namespace Scy::Anionu::Spot::API


#endif // Anionu_Spot_API_IChannel_H