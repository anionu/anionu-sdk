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


#ifndef Anionu_Spot_API_IEnvironment_H
#define Anionu_Spot_API_IEnvironment_H


#include "Anionu/Spot/API/Config.h"


namespace Scy {
	class Logger;
	class IConfiguration;
	/*
namespace Media {
	class FormatRegistry;}
namespace Symple {
	class Message;
	struct MessageDelegate;}
	class ModeRegistry;
	class IStreamingSession;
	*/
namespace Anionu {		
namespace Spot { 
namespace API { 


/// Forward declare ABI agnostic base interfaces
class IEventManagerBase;
class IMediaManagerBase;
class ISympleClientBase;
class ISynchronizerBase;
class ISessionBase;


class IEnvironmentBase
	/// ABI agnostic API
{
public:
	virtual API::ISynchronizerBase& synchronizerBase() = 0;
	virtual API::ISympleClientBase& clientBase() = 0;
	virtual API::IMediaManagerBase& mediaBase() = 0;
	virtual API::IEventManagerBase& eventsBase() = 0;
	virtual API::ISessionBase& sessionBase() = 0;
	
	virtual const char* logBase(const char* message, const char* level = "trace") const = 0;

	virtual const char* version() const = 0;
		/// Returns the installed Spot package version.
		
protected:
	virtual ~IEnvironmentBase() = 0 {};
};


// ---------------------------------------------------------------------
//
#ifdef ENFORCE_STRICT_ABI_COMPATABILITY
//
// Forward declare API interfaces so
// they can be included as required.
class IStreamingManager;
class IChannelManager;
class IEventManager;
class IMediaManager;
class ISynchronizer;
class ISympleClient;
class ISession;


class IEnvironment: public IEnvironmentBase
	/// The ABI strict API environment.
	/// This class provides references to all Spot's
	/// internal components.
{
public:
	virtual IConfiguration& config() = 0;
	virtual IConfiguration& appConfig() = 0;
	virtual API::IMediaManager& media() = 0;
	virtual API::IEventManager& events() = 0;
	virtual API::IChannelManager& channels() = 0;
	virtual API::IStreamingManager& streaming() = 0;
	virtual API::ISynchronizer& synchronizer() = 0;
	virtual API::ISympleClient& client() = 0;	
	virtual API::ISession& session() = 0;
	virtual Logger& logger() = 0;
		
protected:
	virtual ~IEnvironment() = 0 {};
};


#endif // ENFORCE_STRICT_ABI_COMPATABILITY


} } } } // namespace Anionu::Spot::API


#endif // Anionu_Spot_API_IEnvironment_H