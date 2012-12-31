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
#include "Sourcey/Anionu/Authenticator.h"
#include "Sourcey/Logger.h"

#include "Poco/Net/HTTPClientSession.h"
#include "Poco/Net/HTTPBasicCredentials.h"
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
APIClient::APIClient() :
	_methods(*this)
{
	Log("trace", this) << "Creating" << endl;
	//loadMethods(false);
}


APIClient::~APIClient()
{
	Log("trace", this) << "Destroying" << endl;
	cancelTransactions();
	Log("trace", this) << "Destroying: OK" << endl;
}


void APIClient::setCredentials(const string& username, const string& password, const string& endpoint) 
{ 
	FastMutex::ScopedLock lock(_mutex);
	_credentials.username = username;
	_credentials.password = password;
	_credentials.endpoint = endpoint;
}


bool APIClient::loadMethods(bool whiny)
{
	Log("trace", this) << "Loading Methods" << endl;
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


void APIClient::cancelTransactions()
{
	Log("trace", this) << "Stopping Workers" << endl;
	
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
	Log("trace", this) << "Calling: " << request->method.name << endl;
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
	Log("trace", this) << "Calling: " << request->method.name << endl;
	AsyncTransaction* transaction = new AsyncTransaction();
	transaction->setRequest(request);
	transaction->Complete += delegate(this, &APIClient::onTransactionComplete);
	FastMutex::ScopedLock lock(_mutex);
	_transactions.push_back(transaction);
	return transaction;
}


void APIClient::onTransactionComplete(void* sender, HTTP::Response&)
{
	Log("trace", this) << "Transaction Complete" << endl;

	FastMutex::ScopedLock lock(_mutex);
	
	for (vector<APITransaction*>::iterator it = _transactions.begin(); it != _transactions.end(); ++it) {	
		if (*it == sender) {
			Log("trace", this) << "Removing Transaction: " << sender << endl;
			_transactions.erase(it);

			// The transaction is responsible for it's own destruction.
			return;
		}
	}
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


string APIClient::endpoint()
{
	FastMutex::ScopedLock lock(_mutex);
	return _credentials.endpoint;
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

	/*
	if (!form &&
		method.httpMethod == "POST" || 
		method.httpMethod == "PUT")
		set("Content-Type", "application/json");
		*/

	if (!method.anonymous) {
		
		// Using basic auth over SSL
		Poco::Net::HTTPBasicCredentials cred(
			credentials.username,
			credentials.password);
		cred.authenticate(*this); 

		/*
		set("Authorization", 
			Authenticator::generateAuthHeader(
				credentials.username,
				credentials.password, 
				method.httpMethod, 
				method.uri.getPathAndQuery(),
				getContentType(), 
				get("Date")
			)
		);
		*/
	}
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
APIMethods::APIMethods(APIClient& client) :
	_client(client)
{
	Log("trace") << "[APIMethods] Creating" << endl;
}


APIMethods::~APIMethods()
{
	Log("trace") << "[APIMethods] Destroying" << endl;
}


bool APIMethods::valid() const
{
	return !(*this).empty() && !(*this)["methods"].empty();
}


void APIMethods::update()
{
	Log("trace") << "[APIMethods] Updating" << endl;
	FastMutex::ScopedLock lock(_mutex); 	

	try 
	{		
		// Make the API call to retrieve the remote manifest
		HTTP::Request* request = new HTTP::Request("GET", _client.endpoint() + "/api.json");	
		HTTP::Transaction transaction(request);
		HTTP::Response& response = transaction.response();
		if (!transaction.send())
			throw Exception(format("API query failed: HTTP Error: %d %s", 
				static_cast<int>(response.getStatus()), response.getReason()));

		Log("trace") << "[APIMethods] Request Complete: " 
			<< response.getStatus() << ": " << response.getReason() 
			<< "\n\tData: " << response.body.str() 
			<< endl;
					
		JSON::Reader reader;
		if (!reader.parse(response.body.str(), *this))
			throw Exception(reader.getFormatedErrorMessages());
	} 
	catch (Exception& exc) 
	{
		Log("error") << "API Error: " << exc.displayText() << endl;
		exc.rethrow();
	} 
}


string APIMethods::endpoint()
{
	FastMutex::ScopedLock lock(_mutex); 
	return isMember("endpoint") ? (*this)["endpoint"].asString() : ANIONU_API_ENDPOINT;
}


APIMethod APIMethods::get(const string& name, const string& format, const StringMap& params)
{	
	Log("trace") << "[APIMethods] Get: " << name << endl;
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
				method.anonymous = meth["anon"].asBool();
			}
		}

		if (method.name.empty())
			throw Exception("No method description available for: " + name);

		// Update the format and interpolate parameters
		method.format(format);
		method.interpolate(params);

		Log("trace") << "[APIMethods] Got Method"
			 << "\n\tName: " << method.name
			 << "\n\tMethod: " << method.httpMethod
			 << "\n\tUri: " << method.uri.toString()
			 << "\n\tAnonymous: " << method.anonymous
			 << endl;
	}
	catch (Exception& exc)
	{
		Log("error") << "[APIMethods] " << exc.displayText() << endl;
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
//
// API Transaction
//
// ---------------------------------------------------------------------
APITransaction::APITransaction(APIRequest* request) : 
	HTTP::Transaction(request)
{
	Log("trace") << "[APITransaction] Creating" << endl;
}


APITransaction::~APITransaction()
{
	Log("trace") << "[APITransaction] Destroying" << endl;
}


void APITransaction::dispatchCallbacks()
{	
	Log("trace") << "[APITransaction] Process Callbacks: " << (!cancelled()) << endl;

	if (!cancelled()) {
		//Complete.dispatch(this, _response);
		HTTP::Transaction::dispatchCallbacks();
		APITransactionComplete.dispatch(this, static_cast<APIRequest*>(_request)->method, _response);
	}
}


} } // namespace Sourcey::Anionu



		/*
		//string endpoint(_client.endpoint);
		//replaceInPlace(endpoint, "https", "http");
		//URI uri(endpoint + "/api.json");
		URI uri(_client.endpoint() + "/api.json");
		HTTPClientSession session(uri.getHost(), uri.getPort());
		HTTPRequest req("GET", uri.getPathAndQuery(), HTTPMessage::HTTP_1_1);
		session.setTimeout(Timespan(6,0));
		session.sendRequest(req);
		HTTPResponse res;
		istream& rs = session.receiveResponse(res);

		Log("trace") << "[APIMethods] Request Complete: " 
			<< res.getStatus() << ": " 
			<< res.getReason() << endl;

		string data;
		StreamCopier::copyToString(rs, data);
		*/
		
		/*
		for (JSON::ValueIterator it = root.begin(); it != root.end(); it++) {		
			RemotePackage* package = new RemotePackage(*it);
			if (!package->valid()) {
				Log("error") << "[PackageManager] Invalid package: " << package->name() << endl;
				delete package;
				continue;
			}
			_remotePackages.add(package->name(), package);
		}
		*/

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

		//load(xml.data());

		//Log("trace") << "[APIMethods] API Methods XML:\n" << endl;
		//print(cout);