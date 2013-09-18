//
// LibSourcey
// Copyright (C) 2005, Sourcey <http://sourcey.com>
//
// LibSourcey is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// LibSourcey is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.
//


#include "Anionu/APIClient.h"
#include "Sourcey/Logger.h"

/*
//#include "Poco/Net/HTTPClientSession.h"
//#include "Poco/Net/HTTPBasicAuthenticator.h"
#include "Poco/StreamCopier.h"
#include "Poco/Format.h"
*/

#include <assert.h>


using namespace std;

//
//using namespace scy;


namespace scy {
namespace anio {


APIClient::APIClient() :
	_methods(*this)
{
	log("trace") << "Create" << endl;
	_methods.load();
}


APIClient::~APIClient()
{
	log("trace") << "Destroy" << endl;
	//cancelTransactions();
}


void APIClient::setCredentials(const std::string& username, const std::string& password, const std::string& endpoint) 
{ 
	log("trace") << "Set credentials for " << endpoint << endl;
	Mutex::ScopedLock lock(_mutex);
	_credentials.username = username;
	_credentials.password = password;
	_credentials.endpoint = endpoint;
}


/*
APIRequest* APIClient::createRequest(const APIMethod& method)
{
	Mutex::ScopedLock lock(_mutex);
	return new APIRequest(method, _credentials);
}

	
APIRequest* APIClient::createRequest(const std::string& method, 
									 const std::string& format, 
									 const StringMap& params)
{
	return createRequest(methods().get(method, format, params));
}
*/


APITransaction* APIClient::call(const std::string& method, 
								const std::string& format, 
								const StringMap& params)
{
	//return call(createRequest(method, format, params));
	return call(methods().get(method, format, params));
}


APITransaction* APIClient::call(const APIMethod& method)
{
	APITransaction* transaction = new APITransaction(this, method);

	transaction->request().setMethod(method.httpMethod);
	transaction->request().setURI(method.url.str());

	if (method.authenticatable) {		
		http::BasicAuthenticator auth(
			_credentials.username,
			_credentials.password);
		auth.authenticate(transaction->request());
	}

	return transaction;
}

		//assert(0);
	//return call(createRequest(method));
		
	//http::Request::prepare();
		/* // createConnectionT<APITransaction>();
		// Using basic auth over SSL
		http::BasicAuthenticator cred(
			_credentials.username,
			_credentials.password);
		cred.authenticate(transaction->request()); 
		*/

/*
APITransaction* APIClient::call(APIRequest* request)
{
	log("trace") << "Calling: " << request->method.name << endl;
	APITransaction* transaction = createConnectionT<APITransaction>();
	return transaction;

	//APITransaction* transaction = new APITransaction(request);
	//transaction->Complete += delegate(this, &APIClient::onTransactionComplete);
	//ScopedLock lock(_mutex);
	//_transactions.push_back(transaction);
	//return transaction;
}


AsyncTransaction* APIClient::callAsync(const std::string& method, 
									   const std::string& format, 
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
	Mutex::ScopedLock lock(_mutex);
	_transactions.push_back(transaction);
	return transaction;
}

void APIClient::cancelTransactions()
{
	log("trace") << "Canceling Transactions" << endl;	
	Mutex::ScopedLock lock(_mutex);
	for (APITransactionList::const_iterator it = _transactions.begin(); it != _transactions.end(); ++it) {	
		(*it)->Complete -= delegate(this, &APIClient::onTransactionComplete);
		(*it)->cancel();
	}
	_transactions.clear();
}

void APIClient::onTransactionComplete(void* sender, http::Response&)
{
	log("trace") << "Transaction Complete" << endl;
	Mutex::ScopedLock lock(_mutex);	
	for (APITransactionList::iterator it = _transactions.begin(); it != _transactions.end(); ++it) {	
		if (*it == sender) {
			log("trace") << "Removing Transaction: " << sender << endl;
			_transactions.erase(it);
			// The transaction is self destructing.
			return;
		}
	}
}
*/


bool APIClient::loaded()
{
	Mutex::ScopedLock lock(_mutex);	
	return _methods.loaded();
}


APIMethods& APIClient::methods()
{
	Mutex::ScopedLock lock(_mutex);
	return _methods;
}


string APIClient::endpoint()
{
	Mutex::ScopedLock lock(_mutex);
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
		Mutex::ScopedLock lock(_mutex); 					
		json::Reader reader;
		if (!reader.parse(APIv1, *this))
			throw std::runtime_error(reader.getFormatedErrorMessages());
		traceL() << "Loaded API Methods: " << json::stringify(*this, true) << endl;
	} 
	catch (std::exception& exc) 
	{
		errorL() << "API Load Error: " << exc.what() << endl;
		throw exc;/*exc.rethrow();*/
	}  
}


APIMethod APIMethods::get(const std::string& name, const std::string& format, const StringMap& params)
{	
	if (!loaded())
		throw std::runtime_error("Anionu API methods not loaded.");
	
	APIMethod method;
	try
	{			
		Mutex::ScopedLock lock(_mutex); 	
		//traceL("APIMethods") << "Get: " << name << endl;	
		for (auto it = this->begin(); it != this->end(); it++) {	
			json::Value& meth = (*it);		
			//traceL() << "Get API Method: " << json::stringify(meth, true) << endl;
			if (meth.isObject() &&
				meth["name"] == name) {
				//traceL() << "Get API Method name: " << meth["name"].asString() << endl;
				method.name = meth["name"].asString();
				method.httpMethod = meth["http"].asString();
				method.url = _client.endpoint() + meth["uri"].asString();
				method.authenticatable = meth["auth"].asBool();
				break;
			}
		}

		if (method.name.empty())
			throw std::runtime_error("Unknown API method: " + name);

		// Update the format and interpolate parameters
		method.format(format);
		method.interpolate(params);
	}
	catch (std::exception& exc)
	{
		errorL("APIMethods") << "Get Error: " << exc.what() << endl;
		throw exc;/*exc.rethrow();*/
	}
	
	return method;
}


void APIMethods::print(std::ostream& os) const
{
	json::StyledWriter writer;
	os << writer.write(*this);
}


// ---------------------------------------------------------------------
// API Method Description
//
APIMethod::APIMethod() : 
	httpMethod("GET"),
	authenticatable(true)
{
}


void APIMethod::interpolate(const StringMap& params) 
{
	std::string path = url.path();
	for (StringMap::const_iterator it = params.begin(); it != params.end(); ++it) {	
		util::replaceInPlace(path, (*it).first, (*it).second);
	}
	url = http::URL(url.scheme(), url.authority(), path);
	//url.updatePath(path);
}


void APIMethod::format(const std::string& format) 
{
	std::string path = url.path();
	util::replaceInPlace(path, ":format", format.c_str());
	url = http::URL(url.scheme(), url.authority(), path);
	//url.updatePath(path);
}


/*
// ---------------------------------------------------------------------
// API Request
//
void APIRequest::prepare()
{	
	setMethod(method.httpMethod);
	setURI(method.url.str());
	http::Request::prepare();

	if (!method.authenticatable) {
		
		// Using basic auth over SSL
		http::BasicAuthenticator cred(
			credentials.username,
			credentials.password);
		cred.authenticate(*this); 
	}
}
*/


// ---------------------------------------------------------------------
// API Transaction
//
APITransaction::APITransaction(APIClient* client, const APIMethod& method) : //APIRequest* request
	http::ClientConnection(client, method.url) //.getHost(), method.url.getPort()
{
	traceL("APITransaction") << "Create" << endl;
}


APITransaction::~APITransaction()
{
	traceL("APITransaction") << "Destroy" << endl;
}


/*
void APITransaction::onComplete()
{	
	//traceL("APITransaction") << "Callbacks: " << !cancelled() << endl;

	//assert(!cancelled());
	// Send from the current instance, so the
	// sender can be cast as an APITransaction.
	http::ClientConnection::Complete.emit(this, _response);
}
*/


} } // namespace scy::anio