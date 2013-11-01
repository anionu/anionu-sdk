#ifndef Anionu_Spot_CaptureMode_H
#define Anionu_Spot_CaptureMode_H


#include "Anionu/Spot/API/IMode.h"
#include "Anionu/Spot/API/IModule.h"
#include "Anionu/Spot/API/Channel.h"
#include "Anionu/Spot/API/MediaManager.h"
#include "Anionu/Spot/API/IFormProcessor.h"
#include "Sourcey/Media/VideoCapture.h"


namespace scy {
namespace anio { 
namespace spot {


class CaptureMode: 
	/// Capture Mode is a very simple mode implementation that displays 
	/// a local video cature window to the user when activated.
	///
	public api::IMode, 
		/// Inherit from the IMode interface.
		/// This interface is ABI agnostic.
		///
#ifdef Anionu_Spot_USING_CORE_API
		///
	public api::IModule, 
		/// Inherit from IModule if we are using the core API.
		///
	public api::IFormProcessor
		/// IFormProcessor enables us to process forms for remote 
		/// configuration from the Anionu dashboard.
		///
#else
	public api::IModuleBase, 
		/// Inherit from IModuleBase if we are using the base API.
		///
#endif
{
public:
	CaptureMode(api::Environment& env, const std::string& channel);
	virtual ~CaptureMode();
	
	bool activate();
	void deactivate();
	
	void loadConfig();
	bool isActive() const;
	
	const char* errorMessage() const;
	const char* channelName() const;
	const char* docFile() const;
	const char* modeName() const { return "Capture Mode"; }
	const char* className() const { return "CaptureMode"; }

	void onVideoCapture(void*, av::VideoPacket& packet);
		
#ifdef Anionu_Spot_USING_CORE_API
	//
	/// IFormProcessor
	void buildForm(smpl::Form& form, smpl::FormElement& element);
	void parseForm(smpl::Form& form, smpl::FormElement& element);
	
	struct ConfigTest
	{
		int	intValue;
		bool boolValue;
		std::string stringValue;
	} testConfig;
#endif
	
private: 
	bool _isActive;
	std::string _error;
	std::string _channel;
	av::VideoCapture* _video;
	mutable Mutex _mutex;
};


} } } // namespace scy::anio::spot


#endif // Anionu_Spot_CaptureMode_H