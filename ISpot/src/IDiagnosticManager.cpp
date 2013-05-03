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
	SummaryUpdated.emit(this, text);
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
		DiagnosticsComplete.emit(this, _items);
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