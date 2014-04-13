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

#ifndef Anionu_Spot_API_Synchronizer_H
#define Anionu_Spot_API_Synchronizer_H


#include "anionu/spot/api/api.h"

#ifdef Anionu_Spot_USING_CORE_API
#include "scy/signal.h"
#include "scy/json/json.h"
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
	virtual bool sync(const char* json) = 0; // const char* type, const char* path, const char* name, int priority = 0
		// Synchronizes a file asset with your online sotrage.
		// The input JSON is a serialized SynchronizerTask string.
		// See SynchronizerTask below for the JSON description.
		// Full error messages can be found in log file.
		// Returns false on error. 
		
protected:
	virtual ~SynchronizerBase() {};
};


// ---------------------------------------------------------------------
//
#ifdef Anionu_Spot_USING_CORE_API


struct SynchronizerTask;


class Synchronizer: public SynchronizerBase
{
public:
#if 0
	virtual void sync(
			const std::string& type,				/// Asset type [Video, Audio, Image, Archive, Text]
			const std::string& path,				/// Asset file path
			const std::string& name,				/// Asset display name
			const std::string& description,			/// Asset description
			const std::string& mime = "application/octet-stream", /// Asset description
			int priority = 0						/// Task priority [0 - 100]
		) = 0;
#endif
	virtual void sync(const SynchronizerTask&) = 0;
		// Pushes a task onto the synchronization queue.		
		// Throws an exception on error.
	
	Signal<SynchronizerTask&> TaskAdded;
	Signal<SynchronizerTask&> TaskRemoved;
	Signal<SynchronizerTask&> TaskProgress;
	Signal<SynchronizerTask&> TaskStateChange;
	
protected:
	virtual ~Synchronizer() {};
};


//
// Synchronizer Task
//		


struct SynchronizerTask 
{	
	SynchronizerTask(
		const std::string& path = "",
		const std::string& type = "",
		const std::string& mime = "",
		const std::string& name = "",
		const std::string& description = "");

	SynchronizerTask(const json::Value& json);

	// Metadata values
	virtual std::string	type() const /*= 0*/;			// Video, Audio, Image, Archive, Text
	virtual std::string	mime() const /*= 0*/;			// Asset mime type ie. "application/octet-stream"
	virtual std::string	name() const /*= 0*/;			// Asset name
	virtual std::string	description() const /*= 0*/;	// Asset description
		
	// Filesystem values
	virtual std::string	path() const /*= 0*/;			// Asset file path	
	virtual int size() const /*= 0*/;					// Asset file size

	// Synchronizer values
	virtual std::string id() const /*= 0*/;				// Task ID
	virtual std::string	state() const /*= 0*/;			// Pending, Ready, Active, Complete, Paused, Failed
	virtual std::string	error() const /*= 0*/;			// Task error message (Failed state)
	virtual std::string	createdAt() const /*= 0*/;		// Task creation timestamp
	virtual std::string	completedAt() const /*= 0*/;	// Task completion timestamp (Complete state)
	virtual std::string parent() const /*= 0*/;			// Parent Task ID (optional)
	virtual int priority() const /*= 0*/;				// Task importance [0 - 100]
	virtual int progress() const /*= 0*/;				// Upload progress [0 - 100]

	json::Value json;
};


inline bool isValidSynchronizerTaskType(const std::string& type) 
{
	return (type == "Video"
		||  type == "Audio"
		||  type == "Image"
		||  type == "Archive"
		||  type == "Text");
}


inline bool isValidSynchronizerTaskState(const std::string& state) 
{
	return (state == "None"	
		||  state == "Pending"	
		||  state == "Ready"
		||  state == "Active"
		||  state == "Complete"
		||  state == "Paused"
		||  state == "Failed");
}


inline bool isValidSynchronizerTask(const SynchronizerTask& task)
{
	return !task.path().empty()
		&& isValidSynchronizerTaskType(task.type())
		&& isValidSynchronizerTaskState(task.state());
}


inline SynchronizerTask::SynchronizerTask(
		const std::string& path,
		const std::string& type,
		const std::string& mime,
		const std::string& name,
		const std::string& description)
{	
	json["state"] = "None";
	json["path"] = path;
	json["type"] = type;
	json["mime"] = mime;
	json["name"] = name;
	json["description"] = description;
}


inline SynchronizerTask::SynchronizerTask(const json::Value& json) :
	json(json)
{
}


inline std::string SynchronizerTask::id() const
{
	return json.get("id", "").asString();
}


inline std::string SynchronizerTask::type() const
{
	return json.get("type", "").asString();
}


inline std::string SynchronizerTask::mime() const
{
	return json.get("mime", "").asString();
}


inline std::string SynchronizerTask::name() const
{
	return json.get("name", "").asString();
}


inline std::string SynchronizerTask::description() const
{
	return json.get("description", "").asString();
}


inline std::string SynchronizerTask::path() const
{
	return json.get("path", "").asString();
}


inline std::string SynchronizerTask::parent() const
{
	return json.get("parent", "").asString();
}



inline std::string SynchronizerTask::state() const
{
	return json.get("state", "").asString();
}


inline std::string SynchronizerTask::error() const
{
	return json.get("error", "").asString();
}


inline std::string SynchronizerTask::createdAt() const
{
	return json.get("createdAt", "").asString();
}


inline std::string SynchronizerTask::completedAt() const
{
	return json.get("completedAt", "").asString();
}


inline int SynchronizerTask::priority() const
{
	return json.get("priority", 0).asInt();
}


inline int SynchronizerTask::progress() const
{
	return json.get("progress", 0).asInt();
}


inline int SynchronizerTask::size() const
{
	return json.get("size", 0).asInt();
}


#endif // Anionu_Spot_USING_CORE_API


} } } } // namespace scy::anio::spot::api


#endif // Anionu_Spot_API_Synchronizer_H