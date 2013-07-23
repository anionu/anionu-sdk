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


#ifndef Anionu_Spot_API_Util_H
#define Anionu_Spot_API_Util_H


#include "Anionu/Spot/API/Config.h"
#include "Anionu/Spot/API/IMode.h"
#include "Anionu/Spot/API/IModule.h"
#include "Sourcey/IConfiguration.h"
#include "Sourcey/Util/ScopedConfiguration.h"
#include "Poco/Format.h"


namespace scy {
namespace anio {
namespace spot { 
namespace api { 


inline ScopedConfiguration getModeConfiguration(IMode* mode) 
{
	api::IModule* module = dynamic_cast<api::IModule*>(mode);
	if (!module)
		throw Poco::Exception("Mode must implement IModule");

	return ScopedConfiguration(module->env()->config(), 
			//
			/// Channel Scope: channels.[name].modes.[name].[value]
			Poco::format("channels.%s.modes.%s.", 
				std::string(mode->channelName()), 
				std::string(mode->modeName())),
			//
			/// Default Scope: modes.[name].[value]
			Poco::format("modes.%s.", 
				std::string(mode->modeName())));
}

	
LogStream log(api::IModule* module, const char* level = "debug")
	/// This method created a log stream which sends  
	/// messages the Spot application logger.
{ 
	if (module->env() == NULL) 
		return LogStream();
	return module->env()->logger().send(level, module->className(), module); 
}


} } } } // namespace anio::spot::api


#endif // Anionu_Spot_API_Util_H




/*

//ifdef Anionu_Spot_USING_CORE_API
//#include "Sourcey/Signal.h"
//#endif
class ModeConfiguration: public ScopedConfiguration
{
public:
	ModeConfiguration(IMode* mode);

	//, const std::string& currentScope, const std::string& defaultScope

}
*/