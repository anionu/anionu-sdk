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


#include "Sourcey/Spot/IStreamingManager.h"
#include "Sourcey/Spot/IModule.h"


using namespace std;
using namespace Poco;


namespace Sourcey {
namespace Spot {

	
IStreamingManager::IStreamingManager(IEnvironment& env) :
	IModule(env)
{
	log() << "Creating" << endl;
}


IStreamingManager::~IStreamingManager() 
{
	log() << "Destroying" << endl;

	// Gracefully terminate active sessions
	Map sessions = items();
	Map::iterator it = sessions.begin();
	Map::iterator it2;
	while (it != sessions.end()) {
		it2 = it++;		
		log() << "Terminating: " << (*it2).second->token() << endl;
		static_cast<IStreamingSession*>((*it2).second)->terminate();
		sessions.erase(it2);
	}
}


void IStreamingManager::addSession(IStreamingSession* session)
{
	log() << "Adding Session: " << session->token() << endl;
	Manager::add(session->token(), session);
}


IStreamingSession* IStreamingManager::getSession(const string& token)
{
	return Manager::get(token, true);
}


IStreamingSession* IStreamingManager::removeSession(const string& token) 
{
	log() << "Removing Session: " << token << endl;	
	return Manager::remove(token);
}

	
bool IStreamingManager::hasSession(const string& token) const
{
	return Manager::exists(token);
}

	
void IStreamingManager::onItemTimeout(TimerCallback<IStreamingManager::Manager>& timer)
{
	log("debug") << "Item Timeout" << endl;
	
	// FIXME: Crashing here occasionally.
	// This will result is freeing of the session object via callback.
	static_cast<IStreamingSession*>(timer.opaque())->terminate();

	//Manager::onItemTimeout(timer);
}


} } // namespace Sourcey::Spot






	/*
		// Waiting media sessions will have [timeout]
		// duration to reconnect before the session expires.
		//startTimer();
		//stop();
		// Reset and start the internal packet stream 
		// adapters and stop the timers.
		//reset();
		//start();
		//stopTimer();
		//startTimer();
		//stop();
		//stopTimer();
		//stop();
	if (state.id() == StreamingState::Terminating) {
	}
	*/

	/*
		//(*it2).second->StateChange -= delegate(this, &SessionManager::onJingleSessionStateChange);
		//(*it2).second->terminate();
		//delete (*it2).second;
	Map sessions = items();
	Map::iterator it = sessions.begin();
	while (it != sessions.end()) {
		log() << "Terminating: " << it->second << endl;
		static_cast<IStreamingSession*>(it->second)->terminate();
		it = sessions.erase(it);
	}
	*/

	// NOTE: Throw an exception if a stream with 
	// matching token already exists.
	//session->stream().StateChange += delegate(this, &IStreamingManager::onStreamStateChange, -1);
	//session->StateChange += delegate(this, &IStreamingManager::onSessionStateChange);
	//return Manager::add(session->token(), session);, -1, false)
	//	return true;
	//}
	//if () {	
	//return false;

/*
void IStreamingManager::registerSession(IStreamingSession* session) 
{
	assert(session);
	
	LogDebug() << "[IStreamingManager] Registering Media ISession: " << session->token() << endl;
	
	FastMutex::ScopedLock lock(_mutex);
	_sessions[session->token()] = session;
}


IStreamingSession* IStreamingManager::getSession(const string& token) 
{
	FastMutex::ScopedLock lock(_mutex);
	IStreamingSessionMap::iterator it = _sessions.find(token);
	if (it != _sessions.end())
		return it->second; 
	return NULL;
}


bool IStreamingManager::terminateSession(const string& token) 
{
	LogDebug() << "[IStreamingManager] Terminating Media ISession: " << token << endl;	

	IStreamingSession* session = 0;
	{
		FastMutex::ScopedLock lock(_mutex);
		IStreamingSessionMap::iterator it = _sessions.find(token);
		assert(it != _sessions.end());
		if (it != _sessions.end()) {
			session = it->second;
		}
	}
	if (session) {
		session->terminate();
		return true;
	}
	return false;
}


bool IStreamingManager::removeSession(const string& token) 
{
	LogDebug() << "[IStreamingManager] Removing Media ISession: " << token << endl;	

	FastMutex::ScopedLock lock(_mutex);
	IStreamingSessionMap::iterator it = _sessions.find(token);
	assert(it != _sessions.end());
	if (it != _sessions.end()) {
		_sessions.erase(it);
		return true;
	}
	return false;
}
*/