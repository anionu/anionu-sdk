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


#ifndef Anionu_Spot_API_StreamingManager_H
#define Anionu_Spot_API_StreamingManager_H


#include "Anionu/Spot/API/StreamingSession.h"
#include "Sourcey/Util/TimedManager.h"


namespace scy { 
namespace anio {
namespace spot {
namespace api { 


class StreamingManager
{
public:
	virtual api::StreamingSession* createSession(api::StreamingOptions& options) = 0;
		// Creates a streaming session with the given options.
		// A descriptive exception will be thrown on error.
	
	virtual api::StreamingSession* getSession(const std::string& token) const = 0;
		// Returns the api::StreamingSession instance or throws
		// a NotFoundException.
		
	Signal2<api::StreamingSession&, bool&> InitStreamingSession;
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

	Signal3<api::StreamingSession&, PacketStream&, bool&> InitStreamingConnection;
		// Provides listeners with the ability to override the 
		// initialization of session connection objects.	
		// Methodology is the same as the InitStreamingSession 
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
	virtual ~StreamingManager() = 0 {};
};


} } } } // namespace scy::anio::spot::api


#endif // Anionu_Spot_API_StreamingManager_H