//
// Anionu SDK
// Copyright (C) 2011, Anionu <http://anionu.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#include "capturemode.h"
#include "anionu/spot/api/environment.h"
#include "anionu/spot/api/channelmanager.h"
#include "anionu/spot/api/util.h"
#include "scy/symple/form.h"


using std::endl;


namespace scy {
namespace anio { 
namespace spot {


CaptureMode::CaptureMode(api::Environment& env, const std::string& channel) : 
	api::IModule(env), _channel(channel), _isActive(false), _video(nullptr)
{
	DebugL << "Creating" << endl;

	loadConfig();
}


CaptureMode::~CaptureMode()
{	
	DebugL << "Destroying" << endl;
}


bool CaptureMode::activate() 
{
	DebugL << "Activating" << endl;	

	try {
		// Get the video capture and attach a listener, 
		// or throw an exception.
		assert(!_video);
		_video = env().channels().getChannel(_channel)->videoCapture(true);	
		_video->emitter.attach(packetDelegate(this, &CaptureMode::onVideoCapture));
		_isActive = true;
	}
	catch (std::exception& exc) {
		// Set and log the error message.
		_error.assign(exc.what());
		ErrorL << "Activation failed: " << exc.what() << endl;
		
		// Return false to put the mode in error state.
		return false;
	}

	// Return true if the mode activated properly.
	return true;
}


void CaptureMode::deactivate() 
{
	DebugL << "Deactivating" << endl;	

	try {
		// Get the video capture and detach the listener,
		// or throw an exception which we log and swallow.
		assert(_video);
		if (_video)
			_video->emitter.detach(packetDelegate(this, &CaptureMode::onVideoCapture));
		_isActive = false;
	}
	catch (std::exception& exc) {
		// Be sure to swallow all exceptions. Since IMode  
		// is ABI agnostic our implementation should be too.
		ErrorL << "Deactivation failed: " << exc.what() << endl;
	}
}


void CaptureMode::loadConfig()
{	
	// NOTE: IFormProcessor configuration is only
	// available when using the core API.
#ifdef Anionu_Spot_USING_CORE_API
	ScopedConfiguration config = getModeConfiguration(this);
	testConfig.intValue = config.getInt("IntValue", 60);
	testConfig.boolValue = config.getBool("BoolValue", false);
	testConfig.stringValue = config.getBool("StringValue", "Hello Sir");

	DebugL << "Loaded config: " << _channel 
		<< "\r\tIntValue: " << testConfig.intValue 
		<< "\r\tBoolValue: " << testConfig.boolValue 
		<< "\r\tStringValue: " << testConfig.stringValue 
		<< endl;
#endif
}


void CaptureMode::onVideoCapture(void*, av::VideoPacket& packet)
{
	// Capture and display the current channel video image.
	const auto& mpacket = dynamic_cast<av::MatrixPacket&>(packet);
	cv::Mat& image = *mpacket.mat;

	// You could modify the source image here.
	cv::imshow("Channel Image: " + _channel, image);
	cv::waitKey(1);
}


const char* CaptureMode::errorMessage() const 
{ 
	return _error.empty() ? 0 : _error.c_str();
}


const char* CaptureMode::channelName() const
{
	return _channel.c_str();
}


bool CaptureMode::isActive() const
{
	return _isActive;
}


#ifdef Anionu_Spot_USING_CORE_API
//
/// IFormProcessor methods
//
void CaptureMode::buildForm(smpl::Form& form, smpl::FormElement& element)
{	
	DebugL << "Building form: " << form.root().toStyledString() << endl;
	smpl::FormField field;
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


void CaptureMode::parseForm(smpl::Form& form, smpl::FormElement& element)
{
	//DebugL << "Building form: " << form.root().toStyledString() << endl;

	smpl::FormField field;	
	
	// Some Integer Value
	field = element.getField("Capture Mode.IntValue", true);
	if (field.valid()) {
		int value = field.intValue();
		if (value < 0 ||
			value > 10) {
			field.setError("The integer must be between 0 and 10.");
		}
		else {
			env().config().setInt(field.id(), value);
		}
	}
	
	// Some Boolean Value
	field = element.getField("Capture Mode.BoolValue", true);
	if (field.valid())
		env().config().setBool(field.id(), field.boolValue());
	
	// Some String Value
	field = element.getField("Capture Mode.StringValue", true);
	if (field.valid())
		env().config().setString(field.id(), field.value());

	loadConfig();
}

#endif


} } } // namespace scy::anio::spot