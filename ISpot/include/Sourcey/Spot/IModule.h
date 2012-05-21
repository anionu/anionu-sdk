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
	IModule(IEnvironment& env);
	virtual ~IModule();
	
	//virtual void initialize() = 0;
	//virtual void uninitialize() = 0;

	IEnvironment& env();

	LogStream log(const char* level = "debug") const;
	
protected:
	mutable Poco::FastMutex _mutex;
	IEnvironment& _env;
};


} } // namespace Sourcey::Spot


#endif // ANIONU_SPOT_IModule_H