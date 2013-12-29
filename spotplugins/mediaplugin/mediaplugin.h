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

#ifndef Anionu_Spot_MediaPlugin_H
#define Anionu_Spot_MediaPlugin_H


#include "anionu/spot/api/iplugin.h"
#include "anionu/spot/api/imode.h"
#include "anionu/spot/api/iplugin.h"
#include "anionu/spot/api/imodule.h"
#include "anionu/spot/api/streamingmanager.h"
#include "anionu/spot/api/mediamanager.h"
#include "scy/media/avpacketencoder.h"
#include "scy/symple/client.h"


namespace scy {
namespace anio { 
namespace spot {


class MediaPlugin: 
	public api::IPlugin,
	public api::IModule
{
public:
	MediaPlugin(api::Environment& env);
	virtual ~MediaPlugin();

	bool load();
	void unload();
	
	static bool createEncoder(PacketStream& stream, av::EncoderOptions& options);
	void registerCustomMediaFormats();
	
	void onSetupStreamingEncoders(void*, api::StreamingSession& session, bool& handled);
	void onSetupRecordingEncoders(void*, api::RecordingSession& session, bool& handled);

	const char* errorMessage() const;
	
protected:	
	mutable Mutex _mutex;
	std::string	_error;
};


} } } // namespace scy::anio::spot


#endif // Anionu_Spot_MediaPlugin_H