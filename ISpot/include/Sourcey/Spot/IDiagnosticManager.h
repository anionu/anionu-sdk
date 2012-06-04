#ifndef ANIONU_SPOT_IDiagnosticManager_H
#define ANIONU_SPOT_IDiagnosticManager_H


#include "Sourcey/BasicManager.h"
#include "Sourcey/Stateful.h"
#include "Sourcey/Spot/IModule.h"


namespace Sourcey { 
namespace Spot {


struct DiagnosticState: public StateT 
{
	enum Type 
	{
		None = 0,
		Checking,
		Passed,
		Failed
	};

	std::string str(unsigned int id) const 
	{ 
		switch(id) {
		case None:			return "None";
		case Checking:		return "Checking";
		case Passed:		return "Passed";
		case Failed:		return "Failed";
		default:			assert(false);
		}
		return "undefined"; 
	}
};


class Diagnostic: public StatefulSignal<DiagnosticState>, public IModule
{
public:
	Diagnostic(IEnvironment& env);
	virtual ~Diagnostic();

	std::string name;        /// The name of the test.
	std::string description; /// The test description.
	StringList summary;      /// The test summary, maybe including 
							 /// troubleshooting information on failure.
	
	virtual void check();
	virtual void reset();

	virtual bool complete() const;
	virtual bool passed() const;
	virtual bool failed() const;

	Signal<const std::string&> SummaryUpdated;
		/// Fires when a new text item is added 
		/// to the summary.

	/// The StateChange signal will dispatch
	/// diagnostic test results to delegates.
	
	virtual const char* className() const { return "Diagnostic"; }

protected:
	virtual bool pass();
	virtual bool fail();

	virtual void run() = 0;	

	virtual void addSummary(const std::string& text);
};


class AsyncDiagnostic: public Diagnostic, public Poco::Runnable
{
public:
	AsyncDiagnostic(IEnvironment& env) : Diagnostic(env) {};
	virtual ~AsyncDiagnostic() {};

	virtual void check() 
	{
		reset();
		_thread.start(*this);
	};

protected:
	virtual void run() = 0;

	Poco::Thread _thread;
};


class IDiagnosticManager: public BasicManager<std::string, Diagnostic>, public IModule
{
public:
	typedef BasicManager<std::string, Diagnostic>	Manager;
	typedef Manager::Map							Map;
	
public:
	IDiagnosticManager(IEnvironment& env);
	virtual ~IDiagnosticManager();

	bool addDiagnostic(Diagnostic* test);
		// Adds a Diagnostic test instance.

	virtual Diagnostic* getDiagnostic(const std::string& name);
		// Returns the Diagnostic instance or throws
		// a NotFoundException exception.

	virtual void resetAll();

	virtual void checkAll();
		// Runs all managed Diagnostic tests.
		// DiagnosticsComplete will be dispatched on
		// completion.
	
	virtual bool allComplete();

	virtual void onDiagnosticStateChange(void*, DiagnosticState& state, const DiagnosticState&);

	Map& items();

	Signal<const Map&> DiagnosticsComplete;
	
	virtual const char* className() const { return "DiagnosticManager"; }

protected:
	bool freeDiagnostic(const std::string& name);
		
	mutable Poco::FastMutex _mutex;
};


} } // namespace Sourcey::Spot


#endif // ANIONU_SPOT_IDiagnosticManager_H