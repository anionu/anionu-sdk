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

#ifndef Anionu_Spot_API_IEventManager_H
#define Anionu_Spot_API_IEventManager_H


#include "anionu/spot/api/api.h"


#ifdef Anionu_Spot_USING_CORE_API
#include "scy/json/json.h"
#endif


namespace scy {
namespace smpl {
	class Message;
	class Command;
	class Presence;
	class Event; }
namespace anio { 
namespace spot { 
namespace api { 
	
	
class IMessageProcessorBase
	/// The message processor provides an ABI agnostic
	/// interface for processing all incoming, outgoing
	/// and local messages passing through the Spot client.
{
public:		
	virtual bool onMessage(const char* /* message */) { return false; };
		// Handle incoming messages.
		// Return true if the message has been responded to.

	virtual bool onCommand(const char* /* command */) { return false; };
		// Handle incoming commands.
		// Return true if the command has been handled.

	virtual void onPresence(const char* /* presence */) {};
	virtual void onEvent(const char* /* event */) {};
};


#ifdef Anionu_Spot_USING_CORE_API


class IMessageProcessor: public IMessageProcessorBase
{
public:		
	virtual bool onMessage(const smpl::Message& /* message */) { return false; };
	virtual bool onCommand(const smpl::Command& /* command */) { return false; };
	virtual void onPresence(const smpl::Presence& /* presence */) {};
	virtual void onEvent(const smpl::Event& /* event */) {};

	virtual void generateCommands(json::Value& /* root */) {};
};


//
// Command Definition
//


typedef json::Value CommandDefinition;
	/// CommandDefinition defines an arbitrary command which run 
	/// on this Spot client by peers with sufficient permission.
	/// It is used by remote client interfaces to build custom menus.
	

inline CommandDefinition makeCommandDefinition(const std::string& node, 
                                               const std::string& name, 
							                   const std::string& desc, 
							                   bool enabled = true)
{
	CommandDefinition c;
	c["node"] = node;
	c["name"] = name;
	c["desc"] = desc;
	c["enabled"] = enabled;
	return c;
}


#endif // Anionu_Spot_USING_CORE_API


} } } } // namespace scy::anio::spot::api


#endif // Anionu_Spot_API_IEventManager_H