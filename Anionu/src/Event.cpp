#include "Sourcey/Anionu/Event.h"
#include "assert.h"

using namespace std;


namespace Sourcey {
namespace Anionu {


Event::Event(const string& name, const string& message, time_t time) :
	severity(Severity::Default), name(name), message(message), time(time) 
{
}


Event::Event(Severity severity, const string& name, const string& message, time_t time) :
	severity(severity), name(name), message(message), time(time) 
{
}


string Event::formatTime(const string& fmt, int timeZoneDifferential) const
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
	case Default:	return "Default";
	case Safe:		return "Safe";
	case Low:		return "Low";
	case Medium:	return "Medium";
	case High:		return "High";
	}
	assert(0 && "unknown severity");
	return "Safe";
}


Event::Severity Event::strToSeverity(const string& id) 
{ 	
	if (id == "Default")
		return Default;
	if (id == "Safe")
		return Safe;
	else if (id == "Low")
		return Low;
	else if (id == "Medium")
		return Medium;
	else if (id == "High")
		return High;
	assert(0 && "unknown severity");
	return Default;
}


} } // namespace Sourcey::Anionu