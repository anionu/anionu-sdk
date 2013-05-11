#include "Anionu/Event.h"
#include "assert.h"


using namespace std;


namespace Scy { 
namespace Anionu {


Event::Event(const string& name, const string& message, time_t time) :
	name(name), message(message), severity(Event::Default), realm(Event::SpotLocal), time(time) 
{
}


Event::Event(const string& name, const string& message, Severity severity, Realm realm, time_t time) :
	name(name), message(message), severity(severity), realm(realm), time(time) 
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

		
string Event::realmStr() const 
{
	return realmToStr(realm);
}


string Event::realmToStr(Realm id) 
{ 	
	switch (id) {
	case SpotLocal:		return "Spot Local";
	case SpotRemote:	return "Spot Remote";
	case Dashboard:		return "Dashboard";
	}
	assert(0 && "unknown realm");
	return "Spot Local";
}


Event::Realm Event::strToRealm(const string& id) 
{ 	
	if (id == "Spot Local")
		return SpotLocal;
	if (id == "Spot Remote")
		return SpotRemote;
	else if (id == "Dashboard")
		return Dashboard;
	assert(0 && "unknown realm");
	return SpotLocal;
}


} } // namespace Scy::Anionu