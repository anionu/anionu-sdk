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


#ifndef ANIONU_SPOT_ISynchronizer_H
#define ANIONU_SPOT_ISynchronizer_H


#include "Sourcey/Spot/IModule.h"
#include "Sourcey/JSON/JSON.h"
#include "Sourcey/JSON/ISerializable.h"
#include "Poco/DateTimeFormatter.h"

#include <string>
#include <deque>


namespace Sourcey {
namespace Spot {


/*
struct SynchronizerTaskStateChange: public State
{
	enum Type
	{
		Ready,
		Active,
		Failed
	};

	std::string str(unsigned int id) const 
	{ 	
		switch(id) {
		case Ready: return "Ready";
		case Active: return "Active";
		case Failed: return "Failed";
		default: assert(false);
		}
		return "undefined";
	};
};
*/

	
struct SynchronizerTask: public JSON::ISerializable
	/// Synchronizer task structure.
{
	/*
	virtual std::string id() const = 0;		// Unique ID
	virtual std::string parent() const = 0;	// Parent ID (optional)
	virtual std::string	type() const = 0;	// Video, Audio, Image, Archive
	virtual std::string	file() const = 0;	// The asset file system path
	virtual std::string	state() const = 0;	// Pending, Ready, Active, Complete, Cancelled, Failed
	virtual std::string	time() const = 0;	// The asset timestamp
	virtual int priority() const = 0;		// Value between 0 - 100

	virtual JSON::Value& params() = 0;		// Optional API request parameters
	virtual JSON::Value& data() = 0;		// API response data
	
	virtual std::string	completedOn() = 0;	// The datetime the task completed (Complete state)
	virtual std::string	error() = 0;		// Error message set when (Failed state)
	*/

	std::string id;			// Unique ID
	std::string parent;		// Parent ID (optional)
	std::string	name;		// The task name (the file name is not set)
	std::string	type;		// Video, Audio, Image, Archive
	std::string	file;		// The asset file path
	std::string	state;		// Pending, Ready, Active, Complete, Cancelled, Failed
	std::string	time;		// The asset timestamp
	int priority;			// Value between 0 - 100
	int progress;			// Value between 0 - 100
	size_t size;				// The asset file size

	JSON::Value params;		// Optional API request parameters
	JSON::Value	data;		// API response data
	
	std::string	completedOn; // The datetime the task completed (Complete state)
	std::string	error;		// Error message set when (Failed state)

	SynchronizerTask();
	SynchronizerTask(
		const std::string& type,
		const std::string& file);

	virtual SynchronizerTask* clone() const;

	virtual bool valid() const;
	virtual std::string toString() const;

	void serialize(JSON::Value& root);
	void deserialize(JSON::Value& root);

	virtual bool operator == (const SynchronizerTask& r) 
	{		
		return id == r.id
			&& type == r.type
			&& file == r.file
			&& priority == r.priority
			&& time == r.time;
	}

	bool operator < (const SynchronizerTask& r) const 
	{ 
		return priority < r.priority; 
	}
};


class ISynchronizer: public IModule
{
public:
	ISynchronizer(IEnvironment& env);
	virtual ~ISynchronizer() {};

	virtual void push(const SynchronizerTask& task) = 0;
		/// Pushes a task onto the synchronization queue.

	virtual ISynchronizer& operator >> (const SynchronizerTask& task) = 0;
		/// Stream operator alias for push().	
	
	Signal<SynchronizerTask&> TaskAdded;
	Signal<SynchronizerTask&> TaskRemoved;
	Signal<SynchronizerTask&> TaskProgress;
	Signal<SynchronizerTask&> TaskStateChange;

	virtual const char* className() const { return "Synchronizer"; }
};


} } // namespace Sourcey::Spot


#endif // ANIONU_SPOT_ISynchronizer_H




	//virtual std::string name() const;
		/// Returns the file name part of the file path.