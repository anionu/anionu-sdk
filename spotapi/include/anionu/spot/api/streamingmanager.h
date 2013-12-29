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

#ifndef Anionu_Spot_API_StreamingManager_H
#define Anionu_Spot_API_StreamingManager_H


#include "anionu/spot/api/streamingsession.h"
#include "scy/util/timedmanager.h"


namespace scy { 
namespace anio {
namespace spot {
namespace api { 


struct IceServer 
{
	std::string url;
	std::string username;
	std::string credential;
};


class StreamingManager
{
public:
	virtual api::StreamingSession* createSession(api::StreamingOptions& options) = 0;
		// Creates a streaming session with the given options.
		// A descriptive exception will be thrown on error.
	
	virtual api::StreamingSession* getSession(const std::string& token) const = 0;
		// Returns the api::StreamingSession instance or throws
		// a NotFoundException.
	
	virtual std::vector<IceServer> getIceServers() const = 0;
		// Returns a JSON object containing available STUN and TURN 
		// servers for ICE.
		
	Signal2<api::StreamingSession&, bool&> SetupStreamingSources;
	Signal2<api::StreamingSession&, bool&> SetupStreamingEncoders;
		// Provides plugins with the ability to override the
		// creation of source and encoder objects for the
		// given session. Overriding may be partial or
		// complete. If stream creation is to be completely
		// overridden the boolean parameter must return true,
		// otherwise default stream objects will be created or 
		// skipped based on the existence of their polymorphic
		// base in the session PacketStream.
		//
		// Stream objects should derive from:
		//
		//		PacketEmitter:	source
		//		api::StreamEncoder:	encoder
		//
		// Stream ordering is as follows:
		//
		//		sources:				0 - 4
		//		encoders:				5 +
		//

	Signal3<api::StreamingSession&, PacketStream&, bool&> SetupStreamingConnection;
		// Provides listeners with the ability to override the 
		// initialization of session connection objects.	
		// Methodology is the same as the SetupStreamingSources 
		// signal except that stream objects should derive from:
		//
		//		PacketProcessor:		processor
		//		IPacketizer:			packetizer
		//
		// Stream ordering is as follows:
		//
		//		processors:				5 - 9
		//		packetizers:			10 - 14
		//

protected:
	virtual ~StreamingManager() {};
};


} } } } // namespace scy::anio::spot::api


#endif // Anionu_Spot_API_StreamingManager_H