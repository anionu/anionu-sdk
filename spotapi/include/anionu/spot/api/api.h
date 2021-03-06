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


#ifndef Anionu_Spot_API_Config_H
#define Anionu_Spot_API_Config_H

	
#define Anionu_Spot_USING_CORE_API
	// This specifies that we are creating plugins using the ABI strict core interface.
	// When using the core API, your compiler and dependencies must match the compiler
	// used to create the original Spot binary. If the version and dependency check 
	// fail the build will fail with an error. See Compatibility.txt for more information.


#ifdef Anionu_Spot_USING_CORE_API	
	//
	// Enforce strict ABI compatible build environment 
	//
	// VS 2003 (VC7.1): 1310
	// VS 2005 (VC8): 1400
	// VS 2008 (VC9): 1500
	// VS 2010 (VC10): 1600
	// VS 2012 (VC11): 1700
	// VS 2013 (VC12): 1800
	#if _WIN32
		#if _MSC_VER != 1700
			#error "Must use MS Visual Studio 2012 to compile binary \
				compatible plugins using the current API version."
		#endif
	#endif

	#include "scy/base.h"
	#if SCY_MAJOR_VERSION != 0 || SCY_MINOR_VERSION != 9 || SCY_PATCH_VERSION != 3
		#error "Must use LibSourcey 0.9.3 to compile binary \
				compatible plugins using the current API version."
	#endif

	// TODO: Enforce third party dependency library versions
#endif


namespace scy {
namespace anio {
namespace spot { 
namespace api { 

	
// Current Spot SDK version number.
// This version is bumped whenever any ABI breaking are
// introduced either by changes to the API or dependencies.
//
static const char* SDKVersion = "0.6.2";
static const int SDKVersionNumber = 0x000601;


// Date Format (ISO8601)
// The Anionu API always uses ISO8601 for consistency.
//
//static const char* DateFormat = "%Y-%m-%dT%H:%M:%SZ";
//static const char* DateFormatISO8601("%Y-%m-%dT%H:%M:%S%z");


} } } } // namespace scy::anio::spot::api


#endif // Anionu_Spot_API_Config_H