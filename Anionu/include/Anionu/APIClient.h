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


#ifndef Anionu_APIClient_H
#define Anionu_APIClient_H


#include "Anionu/Config.h"
#include "Sourcey/Signal.h"
#include "Sourcey/HTTP/Client.h"
#include "Sourcey/HTTP/Authenticator.h"
#include "Sourcey/JSON/JSON.h"

	
namespace scy { 
namespace anio {

	
static const char* APIv1 = "["
"  {"
"    \"name\" : \"GetAccount\","
"    \"http\" : \"GET\","
"    \"auth\" : true,"
"    \"uri\" : \"/account.:format\""
"  },"
"  {"
"    \"name\" : \"GetEvents\","
"    \"http\" : \"GET\","
"    \"auth\" : true,"
"    \"uri\" : \"/events.:format\""
"  },"
"  {"
"    \"name\" : \"CreateEvent\","
"    \"http\" : \"POST\","
"    \"auth\" : true,"
"    \"uri\" : \"/events.:format\""
"  },"
"  {"
"    \"name\" : \"DeleteEvent\","
"    \"http\" : \"DELETE\","
"    \"auth\" : true,"
"    \"uri\" : \"/events/:id.:format\""
"  },"
"  {"
"    \"name\" : \"GetAssets\","
"    \"http\" : \"GET\","
"    \"auth\" : true,"
"    \"uri\" : \"/assets.:format\""
"  },"
"  {"
"    \"name\" : \"GetAsset\","
"    \"http\" : \"GET\","
"    \"auth\" : true,"
"    \"uri\" : \"/asset/:id.:format\""
"  },"
"  {"
"    \"name\" : \"UploadAsset\","
"    \"http\" : \"POST\","
"    \"auth\" : true,"
"    \"uri\" : \"/assets.:format\""
"  },"
"  {"
"    \"name\" : \"DeleteAsset\","
"    \"http\" : \"DELETE\","
"    \"auth\" : true,"
"    \"uri\" : \"/assets/:id.:format\""
"  }"
"]";
	

// ---------------------------------------------------------------------
//
class APIMethod
	/// Basic Anionu REST API method description.
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
	http::URL url;
	bool authenticatable;
};


// ---------------------------------------------------------------------
//
class APIClient;
class APIMethods: public json::Value
	/// Stores the Anionu REST API method descriptions.
	/// See the top of the file for JSON API specification.
	///
	/// TODO: Parse methods as necessary, no need to keep the
	/// JSON object in memory.
{
public:
	APIMethods(APIClient& client);
	virtual ~APIMethods();

	virtual void load();
		/// Load and the JSON API specification template.

	virtual bool loaded() const;
		/// Returns true methods have been loaded.
	
	virtual APIMethod get(const std::string& name, const std::string& format = "json", const StringMap& params = StringMap());

	void print(std::ostream& os) const;

private:
	mutable Mutex	_mutex;
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
		const std::string& endpoint = Anionu_API_ENDPOINT) :
		username(username), password(password), endpoint(endpoint) {} 
};


/*
// ---------------------------------------------------------------------
//
struct APIRequest: public http::Request
{
	APIRequest() {} 	
	APIRequest(const APIMethod& method, 
			   const APICredentials& credentials) :
		method(method), 
		credentials(credentials) {}
	virtual ~APIRequest() {}

	virtual void prepare();
		/// MUST be called after setting all information and
		/// credentials before sending the request.

	APIMethod method;

	APICredentials credentials;
		/// HTTP authentication credentials.

//private:
	//APIRequest(APIRequest& r) {}
		/// The copy constructor is private
};
*/


// ---------------------------------------------------------------------
//
class APITransaction: public http::ClientConnection
{
public:
	APITransaction(APIClient* client, const APIMethod& method); //APIRequest* request = NULL
	virtual ~APITransaction();

//protected:
	//virtual void onComplete();
};

	
//typedef AsyncSendable<APITransaction> AsyncTransaction;
typedef std::vector<APITransaction*> APITransactionList;


// ---------------------------------------------------------------------
//
class APIClient: public http::Client
{
public:
	APIClient();
	virtual ~APIClient();
	
	virtual void setCredentials(
		const std::string& username, 
		const std::string& password = "", 
		const std::string& endpoint = Anionu_API_ENDPOINT);
		// Sets the HTTP authentication credentials.
		// This must be called before calling any API methods.

	virtual bool loaded();
		// Returns true when method descriptions are loaded and
		// the API is available.

	virtual APIMethods& methods();
		// Returns the API method descriptions.

	virtual std::string endpoint();
		// Returns the API HTTP endpoint.
		// Defaults to Anionu_API_ENDPOINT declaration.
	
	//virtual APIRequest* createRequest(const APIMethod& method);
	//virtual APIRequest* createRequest(const std::string& method, 
	//								  const std::string& format = "json", 
	//								  const StringMap& params = StringMap());

	//virtual APITransaction* call(APIRequest* request);
	virtual APITransaction* call(const APIMethod& method);
	virtual APITransaction* call(const std::string& method, 
								 const std::string& format = "json", 
								 const StringMap& params = StringMap());
	
	/*
	virtual AsyncTransaction* callAsync(APIRequest* request);
	virtual AsyncTransaction* callAsync(const APIMethod& method);
	virtual AsyncTransaction* callAsync(const std::string& method, 
										const std::string& format = "json", 
										const StringMap& params = StringMap());
										*/

	virtual const char* className() const { return "APIClient"; }

//protected:
	//void cancelTransactions();
	//void onTransactionComplete(void* sender, http::Response& response);

private:
	mutable Mutex _mutex;
	APIMethods			_methods;
	APICredentials		_credentials;
	//APITransactionList	_transactions;
};


} } // namespace anionu


#endif