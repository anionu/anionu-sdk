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


#include "Anionu/APIClient.h"
#include "Sourcey/Logger.h"

#include "Poco/Net/HTTPClientSession.h"
#include "Poco/Net/HTTPBasicCredentials.h"
#include "Poco/StreamCopier.h"
#include "Poco/Format.h"

#include <assert.h>


using namespace std;
using namespace Poco;
using namespace Poco::Net;
using namespace Scy;


namespace Scy {
namespace Anionu {


APIClient::APIClient() :
	_methods(*this)
{
	log("trace") << "Creating" << endl;
	_methods.load();
}


APIClient::~APIClient()
{
	log("trace") << "Destroying" << endl;
	cancelTransactions();
}


void APIClient::setCredentials(const string& username, const string& password, const string& endpoint) 
{ 
	FastMutex::ScopedLock lock(_mutex);
	_credentials.username = username;
	_credentials.password = password;
	_credentials.endpoint = endpoint;
}


void APIClient::cancelTransactions()
{
	log("trace") << "Canceling Transactions" << endl;	
	FastMutex::ScopedLock lock(_mutex);
	for (vector<APITransaction*>::const_iterator it = _transactions.begin(); it != _transactions.end(); ++it) {	
		(*it)->Complete -= delegate(this, &APIClient::onTransactionComplete);
		(*it)->cancel();
	}
	_transactions.clear();
}


APIRequest* APIClient::createRequest(const APIMethod& method)
{
	FastMutex::ScopedLock lock(_mutex);
	return new APIRequest(method, _credentials);
}

	
APIRequest* APIClient::createRequest(const string& method, 
									 const string& format, 
									 const StringMap& params)
{
	return createRequest(methods().get(method, format, params));
}


APITransaction* APIClient::call(const string& method, 
								const string& format, 
								const StringMap& params)
{
	return call(createRequest(method, format, params));
}


APITransaction* APIClient::call(const APIMethod& method)
{
	return call(createRequest(method));
}


APITransaction* APIClient::call(APIRequest* request)
{
	log("trace") << "Calling: " << request->method.name << endl;
	APITransaction* transaction = new APITransaction(request);
	transaction->Complete += delegate(this, &APIClient::onTransactionComplete);
	FastMutex::ScopedLock lock(_mutex);
	_transactions.push_back(transaction);
	return transaction;
}


AsyncTransaction* APIClient::callAsync(const string& method, 
									   const string& format, 
									   const StringMap& params)
{
	return callAsync(createRequest(method, format, params));
}


AsyncTransaction* APIClient::callAsync(const APIMethod& method)
{
	return callAsync(createRequest(method));
}


AsyncTransaction* APIClient::callAsync(APIRequest* request)
{
	log("trace") << "Calling: " << request->method.name << endl;
	AsyncTransaction* transaction = new AsyncTransaction();
	transaction->setRequest(request);
	transaction->Complete += delegate(this, &APIClient::onTransactionComplete);
	FastMutex::ScopedLock lock(_mutex);
	_transactions.push_back(transaction);
	return transaction;
}


void APIClient::onTransactionComplete(void* sender, HTTP::Response&)
{
	log("trace") << "Transaction Complete" << endl;
	FastMutex::ScopedLock lock(_mutex);	
	for (vector<APITransaction*>::iterator it = _transactions.begin(); it != _transactions.end(); ++it) {	
		if (*it == sender) {
			log("trace") << "Removing Transaction: " << sender << endl;
			_transactions.erase(it);
			// The transaction is responsible for it's own destruction.
			// TODO: Not so sure about this?
			assert(0);
			return;
		}
	}
}


bool APIClient::loaded()
{
	FastMutex::ScopedLock lock(_mutex);	
	return _methods.loaded();
}


APIMethods& APIClient::methods()
{
	FastMutex::ScopedLock lock(_mutex);
	return _methods;
}


string APIClient::endpoint()
{
	FastMutex::ScopedLock lock(_mutex);
	return _credentials.endpoint;
}


// ---------------------------------------------------------------------
// API Request
//
void APIRequest::prepare()
{	
	setMethod(method.httpMethod);
	setURI(method.uri.toString());
	HTTP::Request::prepare();

	if (!method.anonymous) {
		
		// Using basic auth over SSL
		Poco::Net::HTTPBasicCredentials cred(
			credentials.username,
			credentials.password);
		cred.authenticate(*this); 
	}
}


// ---------------------------------------------------------------------
// API Method Description
//
APIMethod::APIMethod() 
{
}


void APIMethod::interpolate(const StringMap& params) 
{
	string path = uri.getPath();
	for (StringMap::const_iterator it = params.begin(); it != params.end(); ++it) {	
		replaceInPlace(path, (*it).first, (*it).second);
	}
	uri.setPath(path);
}


void APIMethod::format(const string& format) 
{
	string path = uri.getPath();
	replaceInPlace(path, ":format", format.data());
	uri.setPath(path);
}


// ---------------------------------------------------------------------
// API Methods
//
APIMethods::APIMethods(APIClient& client) :
	_client(client)
{
	LogTrace("APIMethods") << "Creating" << endl;
}


APIMethods::~APIMethods()
{
	LogTrace("APIMethods") << "Destroying" << endl;
}


bool APIMethods::loaded() const
{
	return (*this).empty();
}


void APIMethods::load()
{
	try {			
		FastMutex::ScopedLock lock(_mutex); 					
		JSON::Reader reader;
		if (!reader.parse(APIv1, *this))
			throw Exception(reader.getFormatedErrorMessages());
	} 
	catch (Exception& exc) 
	{
		LogError() << "API Load Error: " << exc.displayText() << endl;
		exc.rethrow();
	} 
	catch (exception& exc) 
	{
		LogError() << "API Load Error: JSON Parser:" << exc.what() << endl;
		throw exc;
	} 
}


string APIMethods::endpoint()
{
	FastMutex::ScopedLock lock(_mutex); 
	return ANIONU_API_ENDPOINT;
}


APIMethod APIMethods::get(const string& name, const string& format, const StringMap& params)
{	
	if (!loaded())
		throw Exception("Anionu API methods lot loaded.");
	
	APIMethod method;
	try
	{			
		FastMutex::ScopedLock lock(_mutex); 	
		LogTrace("APIMethods") << "Get: " << name << endl;	
		for (JSON::ValueIterator it = this->begin(); it != this->end(); it++) {	
			JSON::Value& meth = (*it);		
			if (meth.isObject() &&
				meth["name"] == name) {
				method.name = meth["name"].asString();
				method.httpMethod = meth["http"].asString();
				method.uri = (isMember("endpoint") ? 
					(*this)["endpoint"].asString() : 
					ANIONU_API_ENDPOINT) + meth["uri"].asString();
				method.anonymous = meth["anon"].asBool();
				break;
			}
		}

		if (method.name.empty())
			throw Exception("Unknown API method: " + name);

		// Update the format and interpolate parameters
		method.format(format);
		method.interpolate(params);
	}
	catch (Exception& exc)
	{
		LogError("APIMethods") << "Get Error: " << exc.displayText() << endl;
		exc.rethrow();
	}
	
	return method;
}


void APIMethods::print(ostream& os) const
{
	JSON::StyledWriter writer;
	os << writer.write(*this);
}


// ---------------------------------------------------------------------
// API Transaction
//
APITransaction::APITransaction(APIRequest* request) : 
	HTTP::Transaction(request)
{
	LogTrace("APITransaction") << "Creating" << endl;
}


APITransaction::~APITransaction()
{
	LogTrace("APITransaction") << "Destroying" << endl;
	assert(0);

}


void APITransaction::onComplete()
{	
	LogTrace("APITransaction") << "Callbacks: " << !cancelled() << endl;

	assert(!cancelled());
	// Send the complete event from the APITransaction instance.
	HTTP::Transaction::Complete.emit(this, _response);
}


} } // namespace Scy::Anionu