//
// LibSourcey
// Copyright (C) 2005, Sourcey <http://sourcey.com>
//
// LibSourcey is is distributed under a dual license that allows free, 
// open source use and closed source use under a standard commercial
// license.
//
// Non-Commercial Use:
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
// Commercial Use:
// Please contact mail@sourcey.com
//


#ifndef ANIONU_SPOT_IConfigurable_H
#define ANIONU_SPOT_IConfigurable_H


#include "Sourcey/Base.h"
#include "Sourcey/Symple/Form.h"

#include <iostream>


namespace Sourcey {
namespace Spot {


class IConfigurable
{	
public:
	IConfigurable();	
	virtual ~IConfigurable();
	
	virtual bool isConfigurable() const;
		/// This method is always called before generation methods 
		/// to check hat the implementation provides configuration.
		/// If false is returned buildConfigForm() will never be
		/// called.

	virtual bool hasParsableConfig(Symple::Form& form) const;
		/// Checks if there is any parsable items for this class in
		/// the provided form. If false is returned parseConfigForm()
		/// will not be called.

	virtual void printInformation(std::ostream& ost);
		/// Prints textual help and information pertaining to the
		/// current module. Basic HTML markup may be used.

	virtual void buildConfigForm(Symple::Form& form, Symple::FormElement& element, bool baseScope);
		/// Builds the config form for this module. The baseScope 
		/// variable is only applicable for classes which use
		/// multi-scoped configuration such as IModes.

	virtual void parseConfigForm(Symple::Form& form, Symple::FormElement& element);
		/// Parses the config form for this module. 
		/// Errors should be added to any FormField which is
		/// erroneous.
}; 


} } // namespace Sourcey::Spot


#endif // ANIONU_SPOT_IConfigurable_H