#include "Sourcey/Spot/ISynchronizer.h"
#include "Sourcey/CryptoProvider.h"

#include <sstream>


using namespace std; 
using namespace Poco;


namespace Sourcey {
namespace Spot {


// ---------------------------------------------------------------------
//
// Synchronization Queue Job
//
// ---------------------------------------------------------------------
Job::Job() :
	id(CryptoProvider::generateRandomKey(16)), 
	priority(0), 
	state("Ready"), 
	time(Poco::DateTimeFormatter::format(Poco::Timestamp(), "%Y-%m-%d %H:%M:%S"))
{
}


Job::Job(const string& type,
		 const string& path,
		 int priority,		 
		 const string& time) :
	id(CryptoProvider::generateRandomKey(16)), 
	type(type), 
	path(path), 
	priority(priority), 
	state("Ready"), 
	time(time)
{
}


/*
Job::Job(const Job& r) : 
	id(r.id), 
	parent(r.parent), 
	type(r.type), 
	path(r.path), 
	state(r.state), 
	time(r.time),
	priority(r.priority), 
{
}
*/


Job* Job::clone() const 
{
	return new Job(*this);
}


bool Job::valid() const
{
	return !path.empty() 
		&& (type == "Video"
		||  type == "VideoSpectrogram"
		||  type == "Audio"
		||  type == "Image"
		||  type == "Archive"
		||  type == "Log"
		||  type == "Text");
}


string Job::toString() const
{
	ostringstream ost;
	ost << "Job["
		<< id << ":"
		<< parent << ":"
		<< type << ":"
		<< path << ":"
		<< state << ":"
		<< priority << ":"
		<< time << "]";
	return ost.str();
}


// ---------------------------------------------------------------------
//
// Synchronization Queue
//
// ---------------------------------------------------------------------
ISynchronizer::ISynchronizer(IEnvironment& env) :
	IModule(env)
{	
}


} } // namespace Sourcey::Spot



	
/*
		 const string& state,
		 const string& message,
Job::Job(int priority,
		 const string& type,
		 const string& file,
		 const string& state,
		 const string& message,
		 const string& time) :
	type(type), 
	file(file), 
	state(state), 
	message(message), 
	time(time)
{
}
*/
	
/*
ISynchronizer::~ISynchronizer() 
{
	Log("debug") << "[ISynchronizer] ~ISynchronizer()" << endl;
}


void ISynchronizer::start()
{
	Log("debug") << "[ISynchronizer] Starting" << endl;
	//database.initialize();
	//database.parseAndEnqueueJobs();
	_runner.start(this);
}


void ISynchronizer::stop()
{
	Log("debug") << "[ISynchronizer] Stopping" << endl;
	//_runner.stop(this);
	//Mutex::ScopedLock lock(_mutex);
	//for (JobQueue::iterator it = _activeQueue.begin(); it != _activeQueue.end(); ++it) {
	//}	
	reinterpret_cast<Task*>(this)->destroy();
	Log("debug") << "[ISynchronizer] Stopping: OK" << endl;
}


int ISynchronizer::push(const Job& job)
{
	Log("debug") << "[ISynchronizer] Pushing: " << job.toString() << endl;
	Mutex::ScopedLock lock(_mutex);

	_queue.push_back(job);
	Job& j = _queue.back();
	j.id = database.add(job);
	return j.id;
}


void ISynchronizer::run()
{
}


ISynchronizer& ISynchronizer::operator >> (const Job& job) 
{
	push(job);
	return *this;
}
*/
