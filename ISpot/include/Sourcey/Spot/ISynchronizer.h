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
#include "Poco/DateTimeFormatter.h"

#include <string>
#include <deque>


namespace Sourcey {
namespace Spot {


/*
struct JobState: public StateT
{
	enum Type
	{
		Ready,
		Running,
		Failed
	};

	std::string str(unsigned int id) const 
	{ 	
		switch(id) {
		case Ready: return "Ready";
		case Running: return "Running";
		case Failed: return "Failed";
		default: assert(false);
		}
		return "undefined";
	};
};
*/


struct Job
{
	std::string	id;		// Unique ID
	std::string	parent;	// Parent ID (optional)
	std::string	type;	// Video, Audio, Image, Archive
	std::string	path;	// The asset file system path
	std::string	state;	// Pending, Ready, Running, Complete, Failed
	std::string	time;	// The asset timestamp
	int priority;		// Value between 0 - 100

	JSON::Value params; // Optional API request parameters
	JSON::Value	data;	// API response data
	
	std::string	error;	// Set when Failed

	Job();
	Job(const std::string& type,
		const std::string& file,
		int priority = 0,
		const std::string& time = 
			Poco::DateTimeFormatter::format(
			Poco::Timestamp(), "%Y-%m-%d %H:%M:%S"));
	//Job(const Job& r);
	virtual ~Job() {};	

	virtual Job* clone() const;

	virtual bool valid() const;
	virtual std::string toString() const;

	virtual bool operator == (const Job& r) 
	{		
		return id == r.id
			&& type == r.type
			&& path == r.path
			&& priority == r.priority
			&& time == r.time;
	}

	bool operator < (const Job& r) const 
	{ 
		return priority < r.priority; 
	}
};


class ISynchronizer: public IModule
{
public:
	ISynchronizer(IEnvironment& env);
	virtual ~ISynchronizer() {};

	virtual void push(const Job& job) = 0;
		// Pushes a Job onto the synchronization queue;

	virtual ISynchronizer& operator >> (const Job& job) = 0;
		// Pushes a job onto the synchronization queue.	

	Signal<const Job&> SyncComplete;
	
	virtual const char* className() const { return "Synchronizer"; }
};


} } // namespace Sourcey::Spot


#endif // ANIONU_SPOT_ISynchronizer_H