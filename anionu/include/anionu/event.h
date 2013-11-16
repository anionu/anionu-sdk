#ifndef Anionu_Event_H
#define Anionu_Event_H


#include "anionu/config.h"
#include <ctime>
#include <string>


namespace scy { 
namespace anio {


struct Event
	/// This class describes an Anionu surveillance event.
	///
	/// NOTE: Anionu notifications will be triggered based on
	/// the event name (see 'Custom Events' on the dashboard).
{	
	enum Severity
	{
		Default = 0,	// Default threat level for current event type (specified via Dashboard)
		Safe,			// Safe threat level
		Low,			// Low threat level
		Medium,			// Medium threat level
		High			// High threat level
	};

	enum Origin
	{
		Spot = 0,		// Triggered from a Spot client
		Dashboard,		// Triggered from the Anionu dashboard (incoming events)
		Unknown,		// Unknown origin or third party API consumer
	};

	Event(const std::string& name = "", const std::string& message = "", 	
		Severity severity = Default, Origin origin = Spot);
	
	std::string name;
	std::string message;
	std::string origin;
	std::time_t time;
	Severity severity;
	//Origin origin;

	std::string severityStr() const;
	static std::string severityToStr(Severity id);
	static Severity strToSeverity(const std::string& id);	

	//std::string originStr() const;
	static std::string originToStr(Origin id);
	static Origin strToOrigin(const std::string& id);
}; 


} } // namespace scy::anio


#endif // Anionu_Event_H