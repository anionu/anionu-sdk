#ifndef ANIONU_SPOT_IEnvironment_H
#define ANIONU_SPOT_IEnvironment_H


#include "Sourcey/Signal.h"
#include "Sourcey/Spot/IModule.h"


namespace Sourcey {
	

namespace Media {
class FormatRegistry;
}

namespace Anionu {
struct Event;
}

namespace Spot {


class ISession;
class IConfiguration;
class ISynchronizer;
class IChannelManager;
class IModeRegistry;
class IStreamingManager;
class IStreamingSession;
class IMediaManager;
class IDiagnosticManager;


class IEnvironment 
{
public:
	IEnvironment() {};	
	virtual ~IEnvironment() {};
	
	virtual IConfiguration& appConfig() /*const*/ = 0;
	virtual IConfiguration& config() /*const*/ = 0;
	virtual ISession& session() /*const*/ = 0;
	virtual ISynchronizer& synchronizer() /*const*/ = 0;
	virtual IChannelManager& channels() /*const*/ = 0;
	virtual IModeRegistry& modes() /*const*/ = 0;
	virtual IStreamingManager& streaming() /*const*/ = 0;
	virtual IMediaManager& media() /*const*/ = 0;
	virtual IDiagnosticManager& diagnostics() /*const*/ = 0;
	virtual Logger& logger() /*const*/ = 0;

	virtual std::string version() const = 0;
		/// Returns the version number string of the library.
	
	virtual void notifyEvent(const Anionu::Event& event) = 0;
		/// Notifies the system about an event.
		/// If the event severity is higher than the minimum 
		/// severity defined by the user then the event will
		/// be broadcast over the remote network and saved on
		/// the user account. If the event is not broadcast
		/// then event notifications will not be triggered.
	
	Signal<const Anionu::Event&> OnEvent;
		/// Signals an event. Called internally by notifyEvent()
};


} } // namespace Sourcey::Spot


#endif // ANIONU_SPOT_IEnvironment_H