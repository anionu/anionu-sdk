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

#include "SurveillanceModePlugin.h"
#include "SurveillanceMode.h"
#include "anionu/spot/api/environment.h"


using std::endl;


namespace scy {
namespace anio { 
namespace spot {


SPOT_CORE_PLUGIN(
	SurveillanceModePlugin, 
	"Surveillance Mode Plugin",
	"0.9.4"
)


SurveillanceModePlugin::SurveillanceModePlugin(api::Environment& env) :
	api::IModule(env)
{
	Logger::setInstance(&env.logger());
}


SurveillanceModePlugin::~SurveillanceModePlugin()
{
	Logger::setInstance(NULL, false);
}


bool SurveillanceModePlugin::load() 
{
	DebugL << "Loading" << endl;
	return true;
}


void SurveillanceModePlugin::unload() 
{	
	DebugL << "Unloading" << endl;
}


api::IMode* SurveillanceModePlugin::createModeInstance(const char* modeName, const char* channelName)
{
	assert(strcmp(modeName, "Surveillance Mode") == 0);
	return new SurveillanceMode(env(), channelName);
}


const char** SurveillanceModePlugin::modeNames() const
{
	static const char* modeNames[] = { "Surveillance Mode", NULL };
	return modeNames;
}


} } } // namespace scy::anio::spot
