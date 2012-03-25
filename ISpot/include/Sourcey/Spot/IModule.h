#ifndef ANIONU_SPOT_IModule_H
#define ANIONU_SPOT_IModule_H


#include "Sourcey/Logger.h"


namespace Sourcey { 
namespace Spot {


class IEnvironment;


class IModule: public ILoggable
	/// This class is the unified interface from which all
	/// Spot classes inherit from. It ensures all classes
	/// have an IEnvironment reference and Logger access.
{
public:
	IModule(IEnvironment& env); // : _env(env) {};
	virtual ~IModule(); // {};

	IEnvironment& env() { return _env; }
	LogStream log(const char* level = "debug") const; // { return _env.send(this, level); }		
	
protected:
	IEnvironment& _env;
};


} } // namespace Sourcey::Spot


#endif // ANIONU_SPOT_IModule_H