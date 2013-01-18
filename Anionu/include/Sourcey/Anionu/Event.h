#ifndef ANIONU_Event_H
#define ANIONU_Event_H


#include "Sourcey/Base.h"

#include "Poco/DateTimeFormatter.h"
#include "Poco/DateTimeFormat.h"


namespace Sourcey {
namespace Anionu {


struct Event
	/// This class defines a surveillance event that takes place 
	/// on the Anionu system.
	/// NOTE: Anionu notifications will be triggered based on
	/// the event name (see Special Events on the dashboard).
{	
	enum Severity
	{
		Default = 0,	// Default threat level for current event type (specified via Dashboard)
		Safe,			// Safe threat level
		Low,			// Low threat level
		Medium,			// Medium threat level
		High			// High threat level
	};

	enum Realm
	{
		SpotLocal = 0,	// Triggered by a command from the local interface
		SpotRemote,		// Triggered by a command from a remote peer
		Dashboard		// Triggered from the Anionu dashboard
	};

	Event(const std::string& name, 
		const std::string& message, 
		time_t time = ::time(0));

	Event(const std::string& name = "", 
		const std::string& message = "", 	
		Realm realm = SpotLocal,
		Severity severity = Default,
		time_t time = ::time(0));
	
	std::string name;
	std::string message;
	Severity severity;
	Realm realm;
	time_t time;
		
	std::string formatTime(
		const std::string& fmt = Poco::DateTimeFormat::ISO8601_FORMAT,
		int timeZoneDifferential = Poco::DateTimeFormatter::UTC) const;

	std::string severityStr() const;
	static std::string severityToStr(Severity id);
	static Severity strToSeverity(const std::string& id);	

	std::string realmStr() const;
	static std::string realmToStr(Realm id);
	static Realm strToRealm(const std::string& id);
}; 


} } // namespace Sourcey::Anionu


#endif // ANIONU_Event_H