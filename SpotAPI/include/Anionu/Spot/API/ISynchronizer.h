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


#ifndef Anionu_Spot_API_ISynchronizer_H
#define Anionu_Spot_API_ISynchronizer_H


#include "Anionu/Spot/API/Config.h"

#ifdef ENFORCE_STRICT_ABI_COMPATABILITY
#include "Sourcey/Signal.h"
#endif


namespace Scy {
namespace Anionu {
namespace Spot { 
namespace API { 


// ---------------------------------------------------------------------
//
class ISynchronizerBase
	/// ABI agnostic API
{
public:
	virtual bool sync(const char* file, const char* type = "Video", int priority = 0) = 0;
		/// Synchronizes the given file with online storage using
		/// the Anionu REST API. Returns on HTTP request error. 
		/// Full error message in log file.
		
protected:
	virtual ~ISynchronizerBase() = 0 {};
};


// ---------------------------------------------------------------------
//
#ifdef ENFORCE_STRICT_ABI_COMPATABILITY


struct ISynchronizerTask
	/// ISynchronizer task structure.
{
	virtual std::string id() const = 0;			/// Task ID
	virtual std::string	type() const = 0;		/// Video, Audio, Image, Archive, Text
	virtual std::string	name() const = 0;		/// Asset file name
	virtual std::string	file() const = 0;		/// Asset file path
	virtual std::string	state() const = 0;		/// Pending, Ready, Active, Complete, Paused, Failed
	virtual std::string	error() const = 0;		/// Task error message (Failed state)
	virtual std::string	createdAt() const = 0;	/// Asset creation timestamp
	virtual std::string	completedAt() const = 0; /// Task completion timestamp (Complete state)
	virtual int priority() const = 0;			/// Task importance [0 - 100]
	virtual int progress() const = 0;			/// Upload progress [0 - 100]

	virtual bool valid() const = 0;
		/// Returns true if the file and type are set.
};


class ISynchronizer: public ISynchronizerBase
{
public:
	virtual void sync(
		const std::string& file, /// Asset file path
		const std::string& type, /// Asset type [Video, Audio, Image, Archive, Text]
		int priority = 0		 /// Task priority [0 - 100]
		) = 0;
		/// Pushes a task onto the synchronization queue.
		/// Throws an exception on error.
	
	Signal<ISynchronizerTask&> TaskAdded;
	Signal<ISynchronizerTask&> TaskRemoved;
	Signal<ISynchronizerTask&> TaskProgress;
	Signal<ISynchronizerTask&> TaskStateChange;
	
protected:
	virtual ~ISynchronizer() = 0 {};
};


#endif // ENFORCE_STRICT_ABI_COMPATABILITY


} } } } // namespace Anionu::Spot::API


#endif // Anionu_Spot_API_ISynchronizer_H