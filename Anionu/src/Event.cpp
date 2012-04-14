#include "Sourcey/Anionu/Event.h"
#include "assert.h"

using namespace std;


namespace Sourcey {
namespace Anionu {


Event::Event(Severity severity, const string& name, const string& message, time_t time) :
	severity(severity), name(name), message(message), time(time) 
{
}


std::string Event::formatTime(const string& fmt, int timeZoneDifferential) const
{
	return Poco::DateTimeFormatter::format(
		Poco::Timestamp::fromEpochTime(time),
			fmt, timeZoneDifferential);
}

		
string Event::severityStr() const 
{
	return severityToStr(severity);
}


string Event::severityToStr(Severity id) 
{ 	
	switch (id) {
	case None:		return "None";
	case Low:		return "Low";
	case Medium:	return "Medium";
	case High:		return "High";
	}
	assert(0 && "unknown severity");
	return "None";
}


Event::Severity Event::strToSeverity(const std::string& id) 
{ 	
	if (id == "None")
		return None;
	else if (id == "Low")
		return Low;
	else if (id == "Medium")
		return Medium;
	else if (id == "High")
		return High;
	assert(0 && "unknown severity");
	return None;
}


} } // namespace Sourcey::Anionu