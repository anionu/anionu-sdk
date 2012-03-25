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


#include "Sourcey/Anionu/APIClient.h"
#include "Sourcey/HTTP/Authenticator.h"
#include "Sourcey/Logger.h"

#include "Poco/Net/HTTPClientSession.h"
#include "Poco/StreamCopier.h"
#include "Poco/Format.h"

#include <assert.h>


using namespace std;
using namespace Poco;
using namespace Poco::Net;


namespace Sourcey { 
namespace Anionu {


// ---------------------------------------------------------------------
//
// APIClient
//
// ---------------------------------------------------------------------
APIClient::APIClient()
{
	Log("debug") << "[APIClient] Creating" << endl;
	//loadMethods(false);
}


APIClient::~APIClient()
{
	Log("debug") << "[APIClient] Destroying" << endl;

	cancelTransactions();

	Log("debug") << "[APIClient] Destroying: OK" << endl;
}


bool APIClient::loadMethods(bool whiny)
{
	Log("debug") << "[APIClient] Loading Methods" << endl;
	FastMutex::ScopedLock lock(_mutex);	
	try 
	{
		_methods.update();
	} 
	catch (Exception& exc) 
	{
		if (whiny)
			throw exc;
	}
	
	return _methods.valid();
}


bool APIClient::isOK()
{
	FastMutex::ScopedLock lock(_mutex);	
	return _methods.valid();
}


APIMethods& APIClient::methods()
{
	FastMutex::ScopedLock lock(_mutex);
	return _methods;
}


void APIClient::setCredentials(const string& username, const string& password) 
{ 
	FastMutex::ScopedLock lock(_mutex);
	_credentials.username = username;
	_credentials.password = password;
}


void APIClient::cancelTransactions()
{
	Log("debug") << "[APIClient] Stopping Workers" << endl;
	
	FastMutex::ScopedLock lock(_mutex);

	for (vector<APITransaction*>::const_iterator it = _transactions.begin(); it != _transactions.end(); ++it) {	
		(*it)->TransactionComplete -= delegate(this, &APIClient::onTransactionComplete);
		(*it)->cancel();
	}
	_transactions.clear();
}


void APIClient::onTransactionComplete(void* sender, HTTP::Response&)
{
	Log("debug") << "[APIClient] Transaction Complete" << endl;

	FastMutex::ScopedLock lock(_mutex);
	
	for (vector<APITransaction*>::iterator it = _transactions.begin(); it != _transactions.end(); ++it) {	
		if (*it == sender) {
			Log("debug") << "[APIClient] Removing Transaction: " << sender << endl;
			_transactions.erase(it);

			// The transaction is responsible for it's own destruction.
			return;
		}
	}
}


APIRequest* APIClient::createRequest(const APIMethod& method)
{
	FastMutex::ScopedLock lock(_mutex);
	return new APIRequest(method, _credentials);
}

	
APIRequest* APIClient::createRequest(const std::string& method, 
									 const std::string& format, 
									 const StringMap& params)
{
	return createRequest(methods().get(method, format, params));
}


APITransaction* APIClient::call(const std::string& method, 
								const std::string& format, 
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
	Log("debug") << "[APIClient] Calling: " << request->method.name << endl;
	APITransaction* transaction = new APITransaction(request);
	transaction->TransactionComplete += delegate(this, &APIClient::onTransactionComplete);
	FastMutex::ScopedLock lock(_mutex);
	_transactions.push_back(transaction);
	return transaction;
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
	Log("debug") << "[APIClient] Calling: " << request->method.name << endl;
	AsyncTransaction* transaction = new AsyncTransaction();
	transaction->setRequest(request);
	transaction->TransactionComplete += delegate(this, &APIClient::onTransactionComplete);
	FastMutex::ScopedLock lock(_mutex);
	_transactions.push_back(transaction);
	return transaction;
}


// ---------------------------------------------------------------------
//
// API Request
//
// ---------------------------------------------------------------------
void APIRequest::prepare()
{
	setMethod(method.httpMethod);
	setURI(method.uri.toString());
	HTTP::Request::prepare();
	set("User-Agent", "Anionu C++ API");
	if (!form &&
		method.httpMethod == "POST" || 
		method.httpMethod == "PUT")
		set("Content-Type", "application/xml");

	if (method.requireAuth) {
		set("Authorization", 
			HTTP::AnionuAuthenticator::generateAuthHeader(
				credentials.username,
				credentials.password, 
				method.httpMethod, 
				method.uri.toString(),
				getContentType(), 
				get("Date")
			)
		);
	}

	Log("debug") << "[APIRequest] Output: " << endl;
	write(cout);
}



// ---------------------------------------------------------------------
//
// API Method Description
//
// ---------------------------------------------------------------------
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
//
// API Methods
//
// ---------------------------------------------------------------------
APIMethods::APIMethods()
{
	Log("debug") << "[APIMethods] Creating" << endl;
}


APIMethods::~APIMethods()
{
	Log("debug") << "[APIMethods] Destroying" << endl;
}


bool APIMethods::valid() const
{
	return !(*this).empty() && !(*this)["methods"].empty();
}


void APIMethods::update()
{
	Log("debug") << "[APIMethods] Updating" << endl;
	FastMutex::ScopedLock lock(_mutex); 	

	try 
	{
		string endpoint(ANIONU_API_ENDPOINT);
		replaceInPlace(endpoint, "https", "http");
		URI uri(endpoint + "/api.json");
		HTTPClientSession session(uri.getHost(), uri.getPort());
		HTTPRequest req("GET", uri.getPathAndQuery(), HTTPMessage::HTTP_1_1);
		session.setTimeout(Timespan(6,0));
		session.sendRequest(req);
		HTTPResponse res;
		istream& rs = session.receiveResponse(res);

		Log("debug") << "[APIMethods] Request Complete: " 
			<< res.getStatus() << " " 
			<< res.getReason() << endl;

		string data;
		StreamCopier::copyToString(rs, data);
					
		JSON::Reader reader;
		if (!reader.parse(data, *this))
			throw Exception(reader.getFormatedErrorMessages());
		
		/*
		for (JSON::ValueIterator it = root.begin(); it != root.end(); it++) {		
			RemotePackage* package = new RemotePackage(*it);
			if (!package->valid()) {
				Log("error") << "[PackageManager] Unable to parse package: " << package->name() << endl;
				delete package;
				continue;
			}
			_remotePackages.add(package->name(), package);
		}
		*/

		//load(xml.data());

		//Log("debug") << "[APIMethods] API Methods XML:\n" << endl;
		//print(cout);
	} 
	catch (Exception& exc) 
	{
		Log("debug") << exc.displayText() << endl;
		exc.rethrow();
	} 
}


std::string APIMethods::endpoint()
{
	FastMutex::ScopedLock lock(_mutex); 
	return isMember("endpoint") ? (*this)["endpoint"].asString() : ANIONU_API_ENDPOINT;
}


APIMethod APIMethods::get(const string& name, const string& format, const StringMap& params)
{	
	Log("debug") << "[APIMethods] Get: " << name << endl;
	FastMutex::ScopedLock lock(_mutex); 	
	APIMethod method;

	try
	{
		// Attempt to update if invalid...
		if (!valid())
			update();

		// Throw if we fail...
		if (!valid())
			throw Exception("No method description available.");
				
		JSON::Value& methods = (*this)["methods"];
		for (JSON::ValueIterator it = methods.begin(); it != methods.end(); it++) {	
			JSON::Value& meth = (*it);		
			if (meth.isObject() &&
				meth["name"] == name) {
				method.name = meth["name"].asString();
				method.httpMethod = meth["http"].asString();
				method.uri = (isMember("endpoint") ? 
					(*this)["endpoint"].asString() : 
					ANIONU_API_ENDPOINT) + meth["uri"].asString();
				method.requireAuth = meth["auth"].asBool();
			}
		}

		if (method.name.empty())
			throw Exception("No method description available for: " + name);

		/*
		XML::Node node = select_single_node(Poco::format("//name[text()='%s']/..", name).data()).node();
		if (node.empty())
			throw Exception("No method description available for: " + name);

		// Parse the Name value
		method.name = node.child("name").child_value();

		// Parse the Method value
		method.method = node.child("method").child_value();

		// Parse the URI value
		method.uri = URI(node.child("uri").child_value());

		// Set the URI from our config value in debug mode.
#ifdef _DEBUG 
		URI endpoint = URI(ANIONU_API_ENDPOINT);
		method.uri.setScheme(endpoint.getScheme());
		method.uri.setHost(endpoint.getHost());
		method.uri.setPort(endpoint.getPort());
#endif
		*/

		// Update the format and interpolate parameters
		method.format(format);
		method.interpolate(params);

		Log("debug") << "[APIMethods] get()\n"
			 << "Name: " << method.name << "\n"
			 << "Method: " << method.httpMethod << "\n"
			 << "Uri: " << method.uri.toString() << "\n"
			 << "Authenticated: " << method.requireAuth << "\n"
			 << endl;
	}
	catch (Exception& exc)
	{
		Log("error") << "[APIMethods] " << exc.displayText() << endl;
		exc.rethrow();
	}
	
	return method;
}


void APIMethods::print(std::ostream& os) const
{
	JSON::StyledWriter writer;
	os << writer.write(*this);
}


// ---------------------------------------------------------------------
//
// API Transaction
//
// ---------------------------------------------------------------------
APITransaction::APITransaction(APIRequest* request) : 
	HTTP::Transaction(request)
{
	Log("debug") << "[APITransaction] Creating" << endl;
}


APITransaction::~APITransaction()
{
	Log("debug") << "[APITransaction] Destroying" << endl;
}


void APITransaction::dispatchCallbacks()
{	
	Log("debug") << "[APITransaction] Process Callbacks: " << (!cancelled()) << endl;

	if (!cancelled()) {
		TransactionComplete.dispatch(this, _response);
		APITransactionComplete.dispatch(this, static_cast<APIRequest*>(_request)->method, _response);
	}
}


} } // namespace Sourcey::Anionu