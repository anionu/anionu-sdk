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


#include "Sourcey/Spot/ISynchronizer.h"
#include "Sourcey/CryptoProvider.h"

#include <sstream>


using namespace std; 
using namespace Poco;


namespace Sourcey {
namespace Spot {
	
	
// ---------------------------------------------------------------------
//
// Synchronizer
//
// ---------------------------------------------------------------------
ISynchronizer::ISynchronizer(IEnvironment& env) :
	IModule(env)
{	
}
	

// ---------------------------------------------------------------------
//
// Synchronization Task
//
// ---------------------------------------------------------------------
SynchronizerTask::SynchronizerTask() :
	id(CryptoProvider::generateRandomKey(8)), 
	priority(0), 
	progress(0),
	state("Ready"), 
	time(Poco::DateTimeFormatter::format(Poco::Timestamp(), "%Y-%m-%d %H:%M:%S"))
{
}


SynchronizerTask::SynchronizerTask(const string& type,
		 const string& file,
		 int priority,		 
		 const string& time) :
	id(CryptoProvider::generateRandomKey(8)), 
	type(type), 
	file(file), 
	priority(priority), 
	progress(0),
	state("Ready"), 
	time(time)
{
}


SynchronizerTask* SynchronizerTask::clone() const 
{
	return new SynchronizerTask(*this);
}


void SynchronizerTask::serialize(JSON::Value& root)
{
	root["id"] = id;
	root["parent"] = parent;
	root["type"] = type;
	root["file"] = file;
	root["state"] = state;
	root["time"] = time;
	root["priority"] = priority;
	//root["progress"] = progress;
	root["params"] = params;
	root["data"] = data;
	
	root["completedOn"] = completedOn;
}


void SynchronizerTask::deserialize(JSON::Value& root)
{		
	id = root["id"].asString();
	parent = root["parent"].asString();
	type = root["type"].asString();
	file = root["file"].asString();
	state = root["state"].asString();
	time = root["time"].asString();	
	priority = root["priority"].asInt();
	//progress = root["progress"].asInt();
	params = root["params"]; //.asString();	
	data = root["data"]; //.asString();	
	completedOn = root["completedOn"].asString();	
}


bool SynchronizerTask::valid() const
{
	return !file.empty() 
		&& (type == "Video"
		||  type == "VideoSpectrogram"
		||  type == "Audio"
		||  type == "Image"
		||  type == "Archive"
		||  type == "Log"
		||  type == "Text");
}


string SynchronizerTask::toString() const
{
	ostringstream ost;
	ost << "SynchronizerTask["
		<< id << ":"
		<< parent << ":"
		<< type << ":"
		<< file << ":"
		<< state << ":"
		<< priority << ":"
		<< time << "]";
	return ost.str();
}


} } // namespace Sourcey::Spot