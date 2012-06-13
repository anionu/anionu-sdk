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


#include "Sourcey/Spot/IChannel.h"
#include "Sourcey/Media/MediaFactory.h"


using namespace std; 
using namespace Poco; 
using namespace Sourcey::Media; 


namespace Sourcey {
namespace Spot {

	
IChannel::IChannel(IEnvironment& env, const string& name, const Media::Device& video, const Media::Device& audio) :
	IModule(env),
	_name(name),
	_videoDevice(video),
	_audioDevice(audio)
{	
	assert(!_name.empty());
}

	
IChannel::IChannel(IEnvironment& env, const string& name) :
	IModule(env),
	_name(name)
{	
	assert(!_name.empty());
}


IChannel::~IChannel() 
{
}


string IChannel::name() const   
{ 
	FastMutex::ScopedLock lock(_mutex);

	return _name; 
}


string IChannel::videoInputFile() const   
{ 
	FastMutex::ScopedLock lock(_mutex);

	return _videoInputFile; 
}


Device IChannel::videoDevice() const 
{ 
	FastMutex::ScopedLock lock(_mutex);

	return _videoDevice; 
}


Device IChannel::audioDevice() const 
{ 
	FastMutex::ScopedLock lock(_mutex);

	return _audioDevice; 
}


bool IChannel::valid() const
{
	FastMutex::ScopedLock lock(_mutex);

	return !_name.empty() 
		&& _videoDevice.id >= 0;
}


VideoCapture* IChannel::videoCapture(bool whiny) const 
{
	FastMutex::ScopedLock lock(_mutex);

	VideoCapture* capture = !_videoInputFile.empty() ?
		MediaFactory::instance()->video.getCapture(_videoInputFile) : 
		_videoDevice.id >= 0 ? 
			MediaFactory::instance()->video.getCapture(_videoDevice.id) : NULL;
		
	if (whiny && capture == NULL) 
		throw NotFoundException(_name + ": No video device");

	return capture;
}


AudioCapture* IChannel::audioCapture(int channels, int sampleRate, bool whiny) const
{
	FastMutex::ScopedLock lock(_mutex);

	AudioCapture* capture = _audioDevice.id >= 0 ? 
		MediaFactory::instance()->audio.getCapture(_audioDevice.id, channels, sampleRate) : NULL;
		
	if (whiny && capture == NULL) 
		throw NotFoundException(_name + ": No audio device");

	return capture;
}


void IChannel::print(ostream& ost) const 
{
	FastMutex::ScopedLock lock(_mutex);

	ost << "Channel["
		<< "\n\tName:" << _name
		<< "\n\tVideo:" << _videoDevice.name
		<< "\n\tAudio:" << _audioDevice.name
		<< "\n\tVideo ID:" << _videoDevice.id
		<< "\n\tAudio ID:" << _audioDevice.id
		<< "]";
}


} } // namespace Sourcey::Spot