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


#ifndef Anionu_Spot_API_Config_H
#define Anionu_Spot_API_Config_H

	
#define ENFORCE_STRICT_ABI_COMPATABILITY
	/// This is to specify that we are creating an ABI compatible plugin.
	/// The current compiler must match the compiler used to create the original Spot binary.
	/// If the version and dependency check fail the build will fail with an error.


// TODO: Check dependency and compiler versions
#ifndef ENFORCE_STRICT_ABI_COMPATABILITY
#error "Please easure strict ABI compatability and define \
		ENFORCE_STRICT_ABI_COMPATABILITY before including the Spot C++ API"
#endif


namespace Scy {
namespace Anionu {
namespace Spot { 
namespace API { 

	
/// Current Spot SDK version number.
/// This version is be bumped whenever the API or
/// dependencies change, breaking binary compatability.
/// Spot plugins must be built with the same compiler,
/// dependencies, and SDK version as the target Spot client.
/// See Compatability.txt for more information.
///
static const char* SDKVersion = "0.5.1";


/// Date Format
///
static const char* DateFormat = "%Y-%m-%d %H:%M:%S %Z";


} } } } // namespace Sc  y::Anionu::Spot::API


#endif // Anionu_Spot_API_Config_H