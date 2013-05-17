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
			// The transaction is self destructing.
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
// API Methods
//
APIMethods::APIMethods(APIClient& client) :
	_client(client)
{
}


APIMethods::~APIMethods()
{
}


bool APIMethods::loaded() const
{
	return !(*this).empty();
}


void APIMethods::load()
{
	try {			
		FastMutex::ScopedLock lock(_mutex); 					
		JSON::Reader reader;
		if (!reader.parse(APIv1, *this))
			throw Exception(reader.getFormatedErrorMessages());
		LogTrace() << "Loaded API Methods: " << JSON::stringify(*this, true) << endl;
	} 
	catch (Exception& exc) 
	{
		LogError() << "API Load Error: " << exc.displayText() << endl;
		exc.rethrow();
	}  
}


APIMethod APIMethods::get(const string& name, const string& format, const StringMap& params)
{	
	if (!loaded())
		throw Exception("Anionu API methods not loaded.");
	
	APIMethod method;
	try
	{			
		FastMutex::ScopedLock lock(_mutex); 	
		LogTrace("APIMethods") << "Get: " << name << endl;	
		for (JSON::ValueIterator it = this->begin(); it != this->end(); it++) {	
			JSON::Value& meth = (*it);		
			LogTrace() << "Get API Method: " << JSON::stringify(meth, true) << endl;
			if (meth.isObject() &&
				meth["name"] == name) {
				LogTrace() << "Get API Method name: " << meth["name"].asString() << endl;
				method.name = meth["name"].asString();
				method.httpMethod = meth["http"].asString();
				method.url = _client.endpoint() + meth["uri"].asString();
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
// API Method Description
//
APIMethod::APIMethod() 
{
}


void APIMethod::interpolate(const StringMap& params) 
{
	string path = url.getPath();
	for (StringMap::const_iterator it = params.begin(); it != params.end(); ++it) {	
		replaceInPlace(path, (*it).first, (*it).second);
	}
	url.setPath(path);
}


void APIMethod::format(const string& format) 
{
	string path = url.getPath();
	replaceInPlace(path, ":format", format.data());
	url.setPath(path);
}


// ---------------------------------------------------------------------
// API Request
//
void APIRequest::prepare()
{	
	setMethod(method.httpMethod);
	setURI(method.url.toString());
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
}


void APITransaction::onComplete()
{	
	LogTrace("APITransaction") << "Callbacks: " << !cancelled() << endl;

	assert(!cancelled());
	// Send from the current instance, so the
	// sender can be cast as an APITransaction.
	HTTP::Transaction::Complete.emit(this, _response);
}


} } // namespace Scy::Anionu