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


#ifndef ANIONU_SPOT_API_IStreamingManager_H
#define ANIONU_SPOT_API_IStreamingManager_H


#include "Anionu/Spot/API/IStreamingSession.h"
#include "Sourcey/Util/TimedManager.h"


namespace Scy { 
namespace Anionu {
namespace Spot {
namespace API { 


class IStreamingManager
{
public:
	virtual API::IStreamingSession* createSession(API::StreamingOptions& options) = 0;
		/// Creates a streaming session with the given options.
		/// A descriptive exception will be thrown on error.
	
	virtual API::IStreamingSession* getSession(const std::string& token) const = 0;
		/// Returns the API::IStreamingSession instance or throws
		/// a NotFoundException.
		
	Signal2<API::IStreamingSession&, bool&> SetupStreamingSession;
		/// Provides plugins with the ability to override the
		/// creation of source and encoder objects for the
		/// given session. Overriding may be partial or
		/// complete. If stream creation is to be completely
		/// overridden the boolean parameter must return true,
		/// otherwise default stream objects will be created or 
		/// skipped based on the existence of their polymorphic
		/// base in the session PacketStream.
		///
		/// Stream objects should derive from:
		///
		///		PacketEmitter:	source
		///		IPacketEncoder:	encoder
		///
		/// Stream ordering is as follows:
		///
		///		sources:				0 - 4
		///		encoders:				5 +
		///

	Signal3<API::IStreamingSession&, API::ConnectionStream&, bool&> SetupStreamingConnection;
		/// Provides listeners with the ability to override the 
		/// initialization of session connection objects.
	
		/// Methodology is the same
		/// as the SetupStreamingSession signal same except
		/// that stream objects should derive from:
		///
		///		IPacketProcessor:		processor
		///		IPacketizer:			packetizer
		///
		/// Stream ordering is as follows:
		///
		///		processors:				5 - 9
		///		packetizers:			10 - 14
		///

protected:
	virtual ~IStreamingManager() = 0 {};
};


} } } } // namespace Scy::Anionu::Spot::API


#endif // ANIONU_SPOT_API_IStreamingManager_H