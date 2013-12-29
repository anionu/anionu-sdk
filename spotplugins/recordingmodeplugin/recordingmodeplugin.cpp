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

#include "recordingmodeplugin.h"
#include "recordingmode.h"
#include "anionu/spot/api/environment.h"


using std::endl;


namespace scy {
namespace anio { 
namespace spot {


SPOT_CORE_PLUGIN(
	RecordingModePlugin, 
	"Recording Mode Plugin", 
	"0.9.2"
)


RecordingModePlugin::RecordingModePlugin(api::Environment& env) :
	api::IModule(env)
{
	// Set the default logger instance, otherwise
	// a new logger singleton instance will be
	// created for the plugin process.
	Logger::setInstance(&env.logger());
}


RecordingModePlugin::~RecordingModePlugin()
{
	Logger::setInstance(NULL, false);
}


bool RecordingModePlugin::load() 
{
	DebugL << "Loading" << endl;
	return true;
}


void RecordingModePlugin::unload() 
{	
	DebugL << "Unloading" << endl;
}


api::IMode* RecordingModePlugin::createModeInstance(const char* modeName, const char* channelName)
{
	assert(strcmp(modeName, "Recording Mode") == 0);
	return new RecordingMode(env(), channelName);
}


const char** RecordingModePlugin::modeNames() const
{
	static const char* modeNames[] = { "Recording Mode", NULL };
	return modeNames;
}


} } } // namespace scy::anio::spot