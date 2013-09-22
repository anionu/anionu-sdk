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


#ifndef Anionu_Spot_API_Channel_H
#define Anionu_Spot_API_Channel_H


#include "Sourcey/Base.h"
#include "Sourcey/Signal.h"


namespace scy {
namespace av {
	struct Device;
	class VideoCapture;
	class AudioCapture;}
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
	virtual av::VideoCapture* videoCapture(bool whiny = false) const = 0;
	virtual av::AudioCapture* audioCapture(int channels = 2, int sampleRate = 44100, bool whiny = false) const = 0;
	
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
		
protected:
	virtual ~Channel() = 0;
};


typedef std::vector<Channel*> ChannelList;


} } } } // namespace scy::anio::spot::api


#endif // Anionu_Spot_API_Channel_H