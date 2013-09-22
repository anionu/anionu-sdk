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


#ifndef Anionu_Spot_API_Synchronizer_H
#define Anionu_Spot_API_Synchronizer_H


#include "Anionu/Spot/API/Config.h"

#ifdef Anionu_Spot_USING_CORE_API
#include "Sourcey/Signal.h"
#endif


namespace scy {
namespace anio {
namespace spot { 
namespace api { 


// ---------------------------------------------------------------------
//
class SynchronizerBase
	/// ABI agnostic API
{
public:
	virtual bool sync(const char* type, const char* file, int priority = 0) = 0;
		// Synchronizes the given file with online storage using
		// the Anionu REST API. Returns on HTTP request error. 
		// Full error message in log file.
		
protected:
	virtual ~SynchronizerBase() = 0;
};


// ---------------------------------------------------------------------
//
#ifdef Anionu_Spot_USING_CORE_API


struct SynchronizerTask
{
	virtual std::string id() const = 0;			// Task ID
	virtual std::string	type() const = 0;		// Video, Audio, Image, Archive, Text
	virtual std::string	name() const = 0;		// Asset file name
	virtual std::string	file() const = 0;		// Asset file path	
	virtual int size() const = 0;				// Asset file size
	
	virtual std::string	state() const = 0;		// Pending, Ready, Active, Complete, Paused, Failed
	virtual std::string	error() const = 0;		// Task error message (Failed state)
	virtual std::string	createdAt() const = 0;	/// Asset creation timestamp
	virtual std::string	completedAt() const = 0; /// Task completion timestamp (Complete state)
	virtual int priority() const = 0;			// Task importance [0 - 100]
	virtual int progress() const = 0;			// Upload progress [0 - 100]

	virtual bool valid() const = 0;
		// Returns true if the file and type are set.
};


class Synchronizer: public SynchronizerBase
{
public:
	virtual void sync(
			const std::string& type, /// Asset type [Video, Audio, Image, Archive, Text]
			const std::string& file, /// Asset file path
			int priority = 0		 /// Task priority [0 - 100]
		) = 0;
		// Pushes a task onto the synchronization queue.
		// Throws an exception on error.
	
	Signal<SynchronizerTask&> TaskAdded;
	Signal<SynchronizerTask&> TaskRemoved;
	Signal<SynchronizerTask&> TaskProgress;
	Signal<SynchronizerTask&> TaskStateChange;
	
protected:
	virtual ~Synchronizer() = 0;
};


#endif // Anionu_Spot_USING_CORE_API


} } } } // namespace scy::anio::spot::api


#endif // Anionu_Spot_API_Synchronizer_H