#include "Sourcey/Spot/IDiagnosticManager.h"


using namespace std; 


namespace Sourcey {
namespace Spot {


Diagnostic::Diagnostic(IEnvironment& env) : 
	  IModule(env) 
{
}


Diagnostic::~Diagnostic()
{
}
	

void Diagnostic::reset() 
{ 
	summary.clear();
	setState(this, DiagnosticState::None); 
}


void Diagnostic::check() 
{ 
	reset(); 
	run(); 
}


void Diagnostic::addSummary(const std::string& text)
{
	summary.push_back(text);
	SummaryUpdated.dispatch(this, text);
}

	
bool Diagnostic::pass() 
{ 
	return setState(this, DiagnosticState::Passed); 
}

	
bool Diagnostic::fail() 
{ 
	return setState(this, DiagnosticState::Failed); 
}

	
bool Diagnostic::complete() const 
{ 
	return stateEquals(DiagnosticState::Passed)
		|| stateEquals(DiagnosticState::Failed); 
}

	
bool Diagnostic::passed() const 
{ 
	return stateEquals(DiagnosticState::Passed)
		|| stateEquals(DiagnosticState::Failed); 
}

	
bool Diagnostic::failed() const 
{ 
	return stateEquals(DiagnosticState::Failed); 
}


// ---------------------------------------------------------------------
//
IDiagnosticManager::IDiagnosticManager(IEnvironment& env) :
	IModule(env)
{	
}


IDiagnosticManager::~IDiagnosticManager() 
{
}


void IDiagnosticManager::resetAll()
{
	Map tests = items();
	for (Map::const_iterator it = tests.begin(); it != tests.end(); ++it) {	
		it->second->reset();
	}
}


void IDiagnosticManager::checkAll()
{
	Map tests = items();
	for (Map::const_iterator it = tests.begin(); it != tests.end(); ++it) {	
		it->second->check();
	}
}
	

bool IDiagnosticManager::allComplete()
{
	Map tests = items();
	for (Map::const_iterator it = tests.begin(); it != tests.end(); ++it) {	
		if (!it->second->complete())
			return false;
	}
	return true;
}


void IDiagnosticManager::onDiagnosticStateChange(void* sender, DiagnosticState& state, const DiagnosticState&)
{
	Diagnostic* test = reinterpret_cast<Diagnostic*>(sender);
	log() << "Diagnostic State Change: " << test->name << ": " << state.toString() << endl;

	if (test->complete() && allComplete())
		DiagnosticsComplete.dispatch(this, _items);
}


bool IDiagnosticManager::addDiagnostic(Diagnostic* test) 
{
	assert(test);
	assert(!test->name.empty());
	
	log() << "Adding Diagnostic: " << test->name << endl;	
	test->StateChange += delegate(this, &IDiagnosticManager::onDiagnosticStateChange);
	return Manager::add(test->name, test);
}


bool IDiagnosticManager::freeDiagnostic(const string& name) 
{
	assert(!name.empty());

	log() << "Removing Diagnostic: " << name << endl;	
	Diagnostic* test = Manager::remove(name);
	if (test) {
		test->StateChange -= delegate(this, &IDiagnosticManager::onDiagnosticStateChange);
		return true;
	}
	return false;
}


Diagnostic* IDiagnosticManager::getDiagnostic(const string& name) 
{
	return Manager::get(name);
}


IDiagnosticManager::Map& IDiagnosticManager::items() 
{ 
	Poco::FastMutex::ScopedLock lock(_mutex); 
	return _items;
}

} } // namespace Sourcey::Spot