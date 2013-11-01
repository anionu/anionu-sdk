#include "Anionu/Event.h"
#include "assert.h"


using namespace std;


namespace scy { 
namespace anio {


Event::Event(const std::string& name, const std::string& message, Severity severity, Origin origin) :
	name(name), message(message), severity(severity), origin(originToStr(origin)), time(std::time(0)) 
{
}

		
std::string Event::severityStr() const 
{
	return severityToStr(severity);
}


std::string Event::severityToStr(Severity id) 
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


Event::Severity Event::strToSeverity(const std::string& id) 
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

		
//std::string Event::originStr() const 
//{
//	return originToStr(origin);
//}


std::string Event::originToStr(Origin id) 
{ 	
	switch (id) {
	case Spot:			return "Spot";
	//case SpotRemote:	return "Spot Remote";
	case Dashboard:		return "Dashboard";
	case Unknown:		return "Unknown";
	}
	assert(0 && "unknown origin");
	return "Spot Local";
}


Event::Origin Event::strToOrigin(const std::string& id) 
{ 	
	if (id == "Spot")
		return Spot;
	//if (id == "Spot Remote")
	//	return SpotRemote;
	else if (id == "Dashboard")
		return Dashboard;
	else if (id == "Unknown")
		return Unknown;
	assert(0 && "unknown origin");
	return Unknown;
}


} } // namespace scy::anio