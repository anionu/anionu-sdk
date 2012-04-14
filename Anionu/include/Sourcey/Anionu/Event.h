#ifndef ANIONU_Event_H
#define ANIONU_Event_H


#include "Sourcey/Base.h"

#include "Poco/DateTimeFormatter.h"
#include "Poco/DateTimeFormat.h"


namespace Sourcey {
namespace Anionu {


struct Event
	/// This class defines a runtime event that takes place 
	/// on the Anionu system.
	/// NOTE: Anionu notifications will be triggered based on
	/// the event name (see Custom Events on the dashboard).
{	
	enum Severity
	{
		None = 0,
		Low,
		Medium,
		High
	};

	Event(Severity severity = None, 
		const std::string& name = "", 
		const std::string& message = "", 
		time_t time = ::time(0));
	
	std::string name;
	std::string message;
	Severity severity;
	time_t time;
		
	std::string formatTime(
		const std::string& fmt = Poco::DateTimeFormat::ISO8601_FORMAT,
		int timeZoneDifferential = Poco::DateTimeFormatter::UTC) const;

	std::string severityStr() const;
	static std::string severityToStr(Severity id);
	static Severity strToSeverity(const std::string& id);
}; 


} } // namespace Sourcey::Anionu


#endif // ANIONU_Event_H