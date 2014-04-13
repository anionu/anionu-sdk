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

#ifndef Anionu_Spot_CaptureMode_H
#define Anionu_Spot_CaptureMode_H


#include "anionu/spot/api/imode.h"
#include "anionu/spot/api/imodule.h"
#include "anionu/spot/api/channel.h"
#include "anionu/spot/api/mediamanager.h"
#include "anionu/spot/api/iformprocessor.h"
#include "scy/media/videocapture.h"


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
	const char* modeName() const { return "Capture Mode"; }

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
	av::VideoCapture::Ptr _video;
};


} } } // namespace scy::anio::spot


#endif // Anionu_Spot_CaptureMode_H