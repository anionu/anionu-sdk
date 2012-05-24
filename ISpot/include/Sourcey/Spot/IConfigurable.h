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

	virtual void buildConfigForm(Symple::Form& form, Symple::FormElement& element, bool useBase);
		/// Builds the config form for this module. The useBase 
		/// variable is only applicable for classes which use
		/// multi-scoped configuration such as IModes.

	virtual void parseConfigForm(Symple::Form& form, Symple::FormElement& element);
		/// Parses the config form for this module. 
		/// Errors should be added to any FormField which is
		/// erroneous.
}; 


} } // namespace Sourcey::Spot


#endif // ANIONU_SPOT_IConfigurable_H