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


#ifndef ANIONU_APIClient_H
#define ANIONU_APIClient_H


#include "Sourcey/Base.h"
#include "Sourcey/Signal.h"
#include "Sourcey/HTTP/Request.h"
#include "Sourcey/HTTP/Response.h"
#include "Sourcey/HTTP/Transaction.h"
#include "Sourcey/JSON/JSON.h"

	
namespace Sourcey { 
namespace Anionu {


//#ifdef _DEBUG
//#define ANIONU_API_ENDPOINT "http://localhost:3000"
//#else
#define ANIONU_API_ENDPOINT "https://anionu.com"
//#endif


// ---------------------------------------------------------------------
//
class APIMethod 
{
public:
	APIMethod();

	virtual void format(const std::string& format);
		/// Sets the return format as .json, .xml, .api

	virtual void interpolate(const StringMap& params);
		/// Interpolates a URI parameter eg. :key => value

public:
	std::string name;
	std::string httpMethod; // HTTP method ie. GET, POST, PUT, DELETE
	std::string contentType;
	Poco::URI uri;
	bool anonymous;
};


// ---------------------------------------------------------------------
//
class APIClient;


class APIMethods: public JSON::Value
	/// Stores and parses the Anionu API method descriptions.
	/// The internal JSON will look something like:
	///
	/// {
	///		"endpoint": "https://anionu.com",
	///		"formats": [ "json", "xml" ],
	///		"methods": [
	///			{
	///				"name": "SomeMethod",			// API method name
	///				"http": "GET",					// HTTP method
	///				"anon": true,					// Authentication is required?
	///				"uri": "/random/:id/.:format"	// Method uri. 
	///			}									// Method params starting with ":" require 
	///		]										// interpolation @see APIMethod::interpolate
	///	{
	///
{
public:
	APIMethods(APIClient& client);
	virtual ~APIMethods();

	virtual void update();
	virtual bool valid() const;

	virtual std::string endpoint();
	
	virtual APIMethod get(const std::string& name, const std::string& format = "json", const StringMap& params = StringMap());

	void print(std::ostream& os) const;

private:
	mutable Poco::FastMutex	_mutex;
	APIClient& _client;
};


// ---------------------------------------------------------------------
//
struct APICredentials
{	
	std::string username;
	std::string password;
	std::string endpoint;

	APICredentials(
		const std::string& username = "", 
		const std::string& password = "", 
		const std::string& endpoint = ANIONU_API_ENDPOINT) :
		username(username), password(password), endpoint(endpoint) {} 
};


// ---------------------------------------------------------------------
//
struct APIRequest: public HTTP::Request
{
	APIRequest() {} 	
	APIRequest(const APIMethod& method, 
			   const APICredentials& credentials) :
		method(method), 
		credentials(credentials) {}
	virtual ~APIRequest() {}

	virtual void prepare();
		// MUST be called after setting all information and
		// credentials before sending the request.

	APIMethod method;

	APICredentials credentials;
		// HTTP authentication credentials.

private:
	APIRequest(APIRequest& r) {}
		// The copy constructor is private (Poco limitation).
};


// ---------------------------------------------------------------------
//
class APITransaction: public HTTP::Transaction
{
public:
	APITransaction(APIRequest* request = NULL);
	virtual ~APITransaction();
	
	Signal2<APIMethod&, HTTP::Response&> APITransactionComplete;

protected:
	virtual void dispatchCallbacks();
};

	
typedef AsyncSendable<APITransaction> AsyncTransaction;
typedef std::vector<APITransaction*> APITransactionList;


// ---------------------------------------------------------------------
//
class APIClient: public ILoggable
{
public:
	APIClient();
	virtual ~APIClient();
	
	virtual void setCredentials(
		const std::string& username, 
		const std::string& password, 
		const std::string& endpoint = ANIONU_API_ENDPOINT);
		// Sets the HTTP authentication credentials.
		// This must be called before calling any API methods.

	virtual bool loadMethods(bool whiny = true);
		// Loads the method descriptions from the Anionu server.
		// This must be called before calling any API methods.

	virtual bool isOK();
		// Returns true when method descriptions are loaded and
		// the API is available.

	virtual APIMethods& methods();
		// Returns the API method descriptions.

	virtual std::string endpoint();
		// Returns the API HTTP endpoint.
		// Defaults to ANIONU_API_ENDPOINT declaration.
	
	virtual APIRequest* createRequest(const APIMethod& method);
	virtual APIRequest* createRequest(const std::string& method, 
									  const std::string& format = "json", 
									  const StringMap& params = StringMap());

	virtual APITransaction* call(APIRequest* request);
	virtual APITransaction* call(const APIMethod& method);
	virtual APITransaction* call(const std::string& method, 
								 const std::string& format = "json", 
								 const StringMap& params = StringMap());
	
	virtual AsyncTransaction* callAsync(APIRequest* request);
	virtual AsyncTransaction* callAsync(const APIMethod& method);
	virtual AsyncTransaction* callAsync(const std::string& method, 
										const std::string& format = "json", 
										const StringMap& params = StringMap());

	virtual const char* className() const { return "APIClient"; }

protected:
	void cancelTransactions();
	void onTransactionComplete(void* sender, HTTP::Response& response);

private:
	mutable Poco::FastMutex _mutex;
	APIMethods			_methods;
	APICredentials		_credentials;
	APITransactionList	_transactions;
};


} } // namespace Sourcey::Anionu


#endif