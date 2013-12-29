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


#ifndef Anionu_Spot_API_IFormProcessor_H
#define Anionu_Spot_API_IFormProcessor_H


#include "anionu/spot/api/api.h"


#ifdef Anionu_Spot_USING_CORE_API

namespace scy {
namespace smpl {
	class Form;
	class FormElement;}
namespace anio {
namespace spot { 
namespace api { 


class IFormProcessor
{
public:			
	virtual void buildForm(smpl::Form& /* form */, smpl::FormElement& /* element */) {};
		// Builds the configuration form for this module.

	virtual void parseForm(smpl::Form& /* form */, smpl::FormElement& /* element */) {};
		// Parses the configuration form for this module.
		// Errors can be added to any invalid FormFields to
		// resubmit the form to the client for correction.
};
	

} } } } // namespace scy::anio::spot::api


#endif /// Anionu_Spot_USING_CORE_API
#endif /// Anionu_Spot_API_IFormProcessor_H



	


/*
	virtual bool hasParsableFields(smpl::Form& form) const { return false; }
		// Checks if there is any parsable fields for the given form.
		// If no items exist then parseForm() will not be called.

	virtual bool isConfigurable() const { return false; }
		// This method is called to determine weather this module
		// has any configurable methods. If false is returned then 
		// buildForm() will never be called.	

	virtual void sendMessage(const smpl::CMessage& message, bool respond = false) = 0;
		// Sends a Symple message to a remote peer.
		// If the "respond" flag is true, the 'to' and 'from' fields
		// will be swapped, so the message is returned to sender.
		// An exception will be thrown if the Symple client is offline,
		// or the session is invalid.
	
#include "scy/logger.h"
#include "anionu/spot/api/environment.h"
#include "Poco/ClassLibrary.h"
#include "Poco/Mutex.h"
*/
	/*
	virtual void attachListener(const smpl::MessageDelegate& delegate) = 0;
		// Attach a delegate to listen for Symple presence, messages,
		// and/or commands from remote peers.
		// If the message is responded to inside this callback,
		// a StopPropagation exception should be thrown to break
		// out of the callback scope.

	virtual void detachListener(const smpl::MessageDelegate& delegate) = 0;
		// Detach any previously attached message delegates.
		*/