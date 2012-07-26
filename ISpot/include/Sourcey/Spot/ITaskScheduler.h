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


#ifndef ANIONU_SPOT_TaskScheduler_H
#define ANIONU_SPOT_TaskScheduler_H


#include "Sourcey/Spot/IModule.h"
#include "Sourcey/JSON/ISerializable.h"
//#include "Sourcey/ScheduledTask.h"


namespace Sourcey { 
namespace Spot {


/*


// ---------------------------------------------------------------------
//	
class ITaskScheduler: public IModule
	/// This class manages timed tasks for Spot.
{
public:
	ITaskScheduler(IEnvironment& env);
	virtual ~ITaskScheduler();
	
	virtual const char* className() const { return "ITaskScheduler"; }
};
*/


} } // namespace Sourcey::Spot


#endif // ANIONU_SPOT_TaskScheduler_H



	


//protected:
//	virtual bool valid() = 0;
	
	//virtual void onTimeout() = 0;
		/// Performs task processing when the timer fires.
	

	//Poco::DateTime _time;	
	//void setTime(std::time_t time);
	//void setTime(const Poco::DateTime& time);
	//void setTime(const std::string& time, const std::string& fmt = Poco::DateTimeFormat::ISO8601_FORMAT);
		/// Sets the time the task will be run.
		
	//Poco::DateTime time();
		/// Returns the time the task will be run.
	
	//virtual void run() = 0;

//virtual void run();	
//protected:
//	std::string    _action; 
//	Poco::DateTime _runAt;	

 //public StatefulSignal<ScheduledTaskState>, 
	//std::string description; /// The test description.
	//StringList summary;      /// The test summary, maybe including 
							 /// troubleshooting information on failure.
	
	//virtual void check();
	//virtual void reset();

	//virtual bool complete() const;
	//virtual bool passed() const;
	//virtual bool failed() const;

	//Signal<const std::string&> SummaryUpdated;
		/// Fires when a new text item is added 
		/// to the summary.

	/// The StateChange signal will dispatch
	/// diagnostic test results to delegates.
	       /// The name of the test.
	/*
	virtual bool pass();
	virtual bool fail();

	virtual void addSummary(const std::string& text);
	*/