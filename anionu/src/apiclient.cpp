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


#include "anionu/apiclient.h"
#include "scy/logger.h"
#include <assert.h>


using std::endl;


namespace scy {
namespace anio {


APIClient::APIClient() :
	_methods(*this),
	_endpoint(Anionu_API_ENDPOINT)
{
	TraceL << "Create" << endl;
	_methods.load();
}


APIClient::~APIClient()
{
	TraceL << "Destroy" << endl;
}


#if Anionu_ENABLE_PASSWORD
void APIClient::setCredentials(const std::string& username, const std::string& password) 
{ 
	TraceL << "Set credentials for " << username << endl;	
	_credentials.username = username;
	_credentials.password = password;
	_credentials.endpoint = endpoint;
}
#else
void APIClient::setCredentials(const std::string& token) 
{ 
	//TraceL << "Set OAuth token " << token << endl;	
	_credentials.token = token;
}
#endif


void APIClient::setEndpoint(const std::string& endpoint) 
{ 
	TraceL << "Set endpoint " << endpoint << endl;	
	_endpoint = endpoint;
}


http::ClientConnection::Ptr APIClient::call(const std::string& method, 
											const std::string& format, 
											const StringMap& params)
{
	return call(methods().get(method, format, params));
}


http::ClientConnection::Ptr APIClient::call(const APIMethod& method)
{
	auto transaction = http::Client::instance().createConnection(method.url);

	transaction->request().setMethod(method.httpMethod);
	//transaction->request().setURI(method.url.str());

	if (method.authenticate) {		
#if Anionu_ENABLE_PASSWORD
		http::BasicAuthenticator auth(
			_credentials.username,
			_credentials.password);
		auth.authenticate(transaction->request());
#else
		transaction->request().set("Authorization", "Bearer " + _credentials.token);
#endif
	}

	return transaction;
}


APIMethods& APIClient::methods()
{	
	return _methods;
}


std::string APIClient::endpoint()
{	
	return _endpoint;
}


bool APIClient::loaded()
{		
	return _methods.loaded();
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
		json::Reader reader;
		if (!reader.parse(APIv1, *this))
			throw std::runtime_error(reader.getFormatedErrorMessages());
		TraceL << "Loaded API Methods: " << json::stringify(*this, true) << endl;
	} 
	catch (std::exception& exc) {
		ErrorL << "API Load Error: " << exc.what() << endl;
		throw exc;
	}  
}


APIMethod APIMethods::get(const std::string& name, const std::string& format, const StringMap& params)
{	
	if (!loaded())
		throw std::runtime_error("Anionu API methods not loaded.");
	
	APIMethod method;
	try {
		//TraceL << "Get: " << name << endl;	
		for (auto it = this->begin(); it != this->end(); it++) {	
			json::Value& meth = (*it);		
			//TraceL << "Get API Method: " << json::stringify(meth, true) << endl;
			if (meth.isObject() &&
				meth["name"] == name) {
				//TraceL << "Get API Method name: " << meth["name"].asString() << endl;
				method.name = meth["name"].asString();
				method.httpMethod = meth["method"].asString();
				method.url = _client.endpoint() + meth["uri"].asString();
				method.authenticate = !meth.isMember("authenticate") || meth["authenticate"].asBool();
				break;
			}
		}

		if (method.name.empty())
			throw std::runtime_error("Unknown API method: " + name);

		// Update the format and interpolate parameters
		method.format(format);
		method.interpolate(params);
	}
	catch (std::exception& exc) {
		ErrorL << "Get Error: " << exc.what() << endl;
		throw exc;
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
	authenticate(true)
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


#if 0
// ---------------------------------------------------------------------
// API Request
//
void APIRequest::prepare()
{	
	setMethod(method.httpMethod);
	setURI(method.url.str());
	http::Request::prepare();

	if (method.authenticate) {
		
		// Using basic auth over SSL
		http::BasicAuthenticator cred(
			credentials.username,
			credentials.password);
		cred.authenticate(*this); 
	}
}


// ---------------------------------------------------------------------
// API Transaction
//
APITransaction::APITransaction(APIClient* client, const APIMethod& method) : 
	http::ClientConnection(method.url)
{
	TraceL << "Create" << endl;
}


APITransaction::~APITransaction()
{
	TraceL << "Destroy" << endl;
}
#endif


} } // namespace scy::anio