#include "CaptureMode.h"
#include "Anionu/Spot/API/IEnvironment.h"
#include "Anionu/Spot/API/IChannelManager.h"
#include "Anionu/Spot/API/Util.h"
#include "Sourcey/Symple/Form.h"


using namespace std;


namespace scy {
namespace anionu { 
namespace spot {


CaptureMode::CaptureMode(api::IEnvironment& env, const string& channel) : 
	api::IModule(&env), _channel(channel), _isActive(false)
{
	log("Creating");
	loadConfig();
}


CaptureMode::~CaptureMode()
{	
	log("Destroying");
}


bool CaptureMode::activate() 
{
	log("Activating");	
	try 
	{
		// Get the video capture and attach a listener, 
		// or throw an exception.
		av::VideoCapture* video = env()->channels().getChannel(_channel)->videoCapture(true);	
		video->attach(packetDelegate(this, &CaptureMode::onVideoCapture));
		_isActive = true;
	}
	catch (Exception& exc)
	{
		// Set and log the error message.
		_error = exc.message();
		log("Activation failed: " + _error, "error");
		
		// Return false to put the mode in error state.
		return false;
	}

	// Return true if the mode activated properly.
	return true;
}


void CaptureMode::deactivate() 
{
	log("Deactivating");	
	try 
	{
		// Get the video capture and detach the listener,
		// or throw an exception which we log and swallow.
		av::VideoCapture* video = env()->channels().getChannel(_channel)->videoCapture(true);
		video->detach(packetDelegate(this, &CaptureMode::onVideoCapture));
		_isActive = false;
	}
	catch (Exception& exc) 
	{
		// Be sure to swallow all exceptions. Since IMode  
		// is ABI agnostic our implementation should be too.
		log("Deactivation failed: " + exc.message(), "error");
	}
}


void CaptureMode::loadConfig()
{	
	// NOTE: IFormProcessor configuration is only
	// available when using the core API.
#ifdef Anionu_Spot_USING_CORE_API
	Poco::FastMutex::ScopedLock lock(_mutex); 	
	ScopedConfiguration config = getModeConfiguration(this);
	testConfig.intValue = config.getInt("IntValue", 60);
	testConfig.boolValue = config.getBool("BoolValue", false);
	testConfig.stringValue = config.getBool("StringValue", "Hello Sir");

	api::log(this) << "Loaded Config: " << _channel 
		<< "\r\tIntValue: " << testConfig.intValue 
		<< "\r\tBoolValue: " << testConfig.boolValue 
		<< "\r\tStringValue: " << testConfig.stringValue 
		<< endl;
#endif
}


void CaptureMode::onVideoCapture(void*, av::VideoPacket& packet)
{
	// Capture and display the current channel video image.
	av::MatPacket* mpacket = dynamic_cast<av::MatPacket*>(&packet);		
	if (mpacket) {
		cv::Mat& image = *mpacket->mat;

		// You could modify the source image here.
		cv::imshow("Channel Image: " + _channel, image);
		cv::waitKey(1);
	}
}


const char* CaptureMode::errorMessage() const 
{ 
	Poco::FastMutex::ScopedLock lock(_mutex);
	return _error.empty() ? 0 : _error.data();
}


const char* CaptureMode::channelName() const
{
	Poco::FastMutex::ScopedLock lock(_mutex); 
	return _channel.data();
}


bool CaptureMode::isActive() const
{
	Poco::FastMutex::ScopedLock lock(_mutex); 
	return _isActive;
}


const char* CaptureMode::docFile() const
{
	// Return the relative path to the markdown help file.
	return "MediaPlugin/CaptureMode.md";
}


#ifdef Anionu_Spot_USING_CORE_API
//
/// IFormProcessor methods
//
void CaptureMode::buildForm(symple::Form& form, symple::FormElement& element)
{	
	log("Building form: " + form.root().toStyledString());
	symple::FormField field;
	ScopedConfiguration config = getModeConfiguration(this);

	// Determine weather we are building the form at channel or
	// default scope as it will change the way we display the form.
	//
	//   Channel Scope: channels.[channelName].modes.[modeName]
	//   Defualt Scope: modes.[modeName]
	//
	bool defaultScope = element.type() != "form";
	if (defaultScope) {
		element.setHint(
			"This form enables you to configure the default settings for Capture Mode. "
			"Any settings configured here may be overridden on a per channel basis (see Channel Configuration)."
		);
	}
	
	// Some Integer Value
	field = element.addField("number", config.getScopedKey("IntValue", defaultScope), "Some Integer Value");	
	field.setHint("You can say stuff here!");
	field.setValue(config.getInt("IntValue", testConfig.intValue, defaultScope));
	
	// Some Boolean Value
	field = element.addField("boolean", config.getScopedKey("BoolValue", defaultScope), "Some Boolean Value");	
	field.setHint("Well, are you going to check the box or not?");
	field.setValue(config.getBool("BoolValue", testConfig.boolValue, defaultScope));
	
	// Some String Value
	field = element.addField("text", config.getScopedKey("StringValue", defaultScope), "Some String Value");	
	field.setHint("You can type something here, and it won't be forgotten!");
	field.setValue(config.getString("StringValue", testConfig.stringValue, defaultScope));	
}


void CaptureMode::parseForm(symple::Form& form, symple::FormElement& element)
{
	log("Building form: " + form.root().toStyledString());
	symple::FormField field;	
	
	// Some Integer Value
	field = element.getField("Capture Mode.IntValue", true);
	if (field.valid()) {
		int value = field.intValue();
		if (value < 0 ||
			value > 10) {
			field.setError("The integer must be between 0 and 10.");
		}
		else {
			env()->config().setInt(field.id(), value);
		}
	}
	
	// Some Boolean Value
	field = element.getField("Capture Mode.BoolValue", true);
	if (field.valid())
		env()->config().setBool(field.id(), field.boolValue());
	
	// Some String Value
	field = element.getField("Capture Mode.StringValue", true);
	if (field.valid())
		env()->config().setString(field.id(), field.value());

	loadConfig();
}

#endif


} } } // namespace scy::anionu::Spot