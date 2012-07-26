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


#include "Sourcey/Spot/ITaskScheduler.h"

#include "Poco/DateTimeParser.h"
#include "Poco/Format.h"


using namespace std; 
using namespace Poco; 


namespace Sourcey {
namespace Spot {


/*
// ---------------------------------------------------------------------
//	
ITaskScheduler::ITaskScheduler(IEnvironment& env) :
	IModule(env)
{	
}


ITaskScheduler::~ITaskScheduler() 
{
}
*/


} } // namespace Sourcey::Spot




	


/*
	//,
	//TimerTask(reinterpret_cast<Runner&>(env))

void IScheduledTask::setTime(const DateTime& time)
{
	{
		FastMutex::ScopedLock lock(_mutex);
		_time = time;
	}

	// Determine time differential and update task run time
	
    DateTime now;
    Timespan s = time - now;
	
	//milliseconds();
}


void IScheduledTask::setTime(time_t time) 
{
	DateTime runAt(Timestamp::fromEpochTime(time));
	setTime(runAt);
}


void IScheduledTask::setTime(const string& time, const std::string& fmt) 
{
	int tzd;
	DateTime runAt(DateTimeParser::parse(fmt, time, tzd));
	setTime(runAt);
}
*/

		/*
bool IScheduledTask::run()
{
	bool doTimeout = false;
	bool doDestroy = false;
	{
		Poco::FastMutex::ScopedLock lock(_mutex);
		if (_scheduleAt.expired()) {
			doTimeout = true;
			if (_interval > 0) {
				_scheduleAt.setDelay(_interval);
				_scheduleAt.reset();
			}
			else doDestroy = true;
		}
	}
	if (doTimeout)
		onTimeout();
	if (doDestroy)
		destroy();
}
		*/


	
	//FastMutex::ScopedLock lock(_mutex);
	//{
		//FastMutex::ScopedLock lock(_mutex);
		//root["action"] = action;
		//root["channel"] = channel;
		//root["mode"] = mode;
	//}
	//ScheduledTask::serialize(root);
	//assert(valid());

	//{
		//FastMutex::ScopedLock lock(_mutex);
		//action = root["action"].asString();
		//channel = root["channel"].asString();
		//mode = root["mode"].asString();
	//}
	//ScheduledTask::deserialize(root);
	//assert(valid());

	//_time = DateTime(Timestamp::fromEpochTime(time))
	//(*this)["time"] = (double)time;
	//DateTimeFormatter::format(
	//	Timestamp::fromEpochTime(time), 
	//	DateTimeFormat::ISO8601_FORMAT);

 //(Timestamp::fromEpochTime(time));
	// no checking done
	//(*this)["time"] = time;



/*
void IScheduledTask::run()
{
	TimerTask::run();
}
*/


/*
void ScheduledTask::reset() 
{ 
	summary.clear();
	setState(this, ScheduledTaskState::None); 
}


void ScheduledTask::check() 
{ 
	reset(); 
	run(); 
}


void ScheduledTask::addSummary(const std::string& text)
{
	summary.push_back(text);
	SummaryUpdated.dispatch(this, text);
}

	
bool ScheduledTask::pass() 
{ 
	return setState(this, ScheduledTaskState::Passed); 
}

	
bool ScheduledTask::fail() 
{ 
	return setState(this, ScheduledTaskState::Failed); 
}

	
bool ScheduledTask::complete() const 
{ 
	return stateEquals(ScheduledTaskState::Passed)
		|| stateEquals(ScheduledTaskState::Failed); 
}

	
bool ScheduledTask::passed() const 
{ 
	return stateEquals(ScheduledTaskState::Passed)
		|| stateEquals(ScheduledTaskState::Failed); 
}

	
bool ScheduledTask::failed() const 
{ 
	return stateEquals(ScheduledTaskState::Failed); 
}
*/



/*
bool TaskScheduler::addChannel(IChannel* channel) 
{
	assert(channel);
	assert(!channel->name().empty());
	
	log() << "Adding Channel: " << channel->name() << endl;	
	return Manager::add(channel->name(), channel);
}


bool TaskScheduler::freeChannel(const string& name) 
{
	assert(!name.empty());

	log() << "Deleting Channel: " << name << endl;		
	return Manager::free(name);
}


IChannel* TaskScheduler::getChannel(const string& name) 
{
	return Manager::get(name, true);
}


IChannel* TaskScheduler::getDafaultChannel()
{
	if (!_items.empty()) {
		IChannelMap::const_iterator it = _items.begin();
		return it->second;
	}

	return NULL;
}
*/