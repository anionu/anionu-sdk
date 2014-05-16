#include "scy/base.h"
#include "scy/application.h"
#include "anionu/apiclient.h"
#include "anionu/event.h"
#include "anionu/ssl.h"
#include "scy/net/sslsocket.h"
#include "scy/net/sslmanager.h"
#include "scy/http/form.h"
#include "scy/http/packetizers.h"
#include "scy/util.h"

#include "assert.h"


using std::endl;
using namespace scy;


/*
// Detect memory leaks on winders
#if defined(_DEBUG) && defined(_WIN32)
#include "MemLeakDetect/MemLeakDetect.h"
#include "MemLeakDetect/MemLeakDetect.cpp"
CMemLeakDetect memLeakDetect;
#endif
*/


namespace scy {
namespace anio {

	
#define TEST_SSL 1
#define Anionu_API_ENDPOINT    "https://anionu.com" //"http://localhost:3000"
#define Anionu_API_OAUTH_TOKEN "authtokenhere"


class Tests
{
public:
	Application app; 
	APIClient client;

	Tests()
	{	
#ifdef _MSC_VER
		_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
		{		
			// Initialize client credentials
			client.setEndpoint(Anionu_API_ENDPOINT);
#if Anionu_ENABLE_PASSWORD
			client.setCredentials(Anionu_API_USERNAME, Anionu_API_KEY, Anionu_API_ENDPOINT); 
#else
			client.setCredentials(Anionu_API_OAUTH_TOKEN);
#endif

			testDefaultGet();
#if 0
			runAssetUploadTest();
			runCreateEventTest();
			testAuthentication();
			runCreateEventTest();
			runAssetDownloadTest();
#endif
			runLoop();
			runCleanup();
		}
	}
	
	
	//
	/// Get Request
	//	


	void testDefaultGet()
	{
		auto conn = http::Client::instance().createConnection(Anionu_API_ENDPOINT);
		conn->Headers += sdelegate(this, &Tests::onDefaultHeaders);
		conn->Complete += sdelegate(this, &Tests::onDefaultComplete);
		conn->setReadStream(new std::stringstream);
		conn->request().setURI("/");
		conn->send(); // send default GET /
	}	
	
	void onDefaultHeaders(void*, http::Response& res)
	{	
		DebugL << "On response headers: " << res << endl;
	}
	
	void onDefaultComplete(void* sender, const http::Response& response)
	{		
		auto conn = reinterpret_cast<http::ClientConnection*>(sender);
		DebugL << "On response complete" << response << 
			conn->readStream<std::stringstream>()->str() << endl;
		conn->close();
	}

	
	//
	/// Authentication Test
	//	


	void testAuthentication()
	{
		auto conn = http::Client::instance().createConnection(Anionu_API_ENDPOINT);
		conn->Headers += sdelegate(this, &Tests::onAuthenticationHeaders);
		conn->Complete += sdelegate(this, &Tests::onAuthenticationComplete);
		conn->setReadStream(new std::stringstream);
		conn->request().setURI("/authenticate.json");
		conn->send();
	}	
	
	void onAuthenticationHeaders(void*, http::Response& res)
	{	
		DebugL << "On response headers: " << res << endl;
	}
	
	void onAuthenticationComplete(void* sender, const http::Response& response)
	{		
		auto self = reinterpret_cast<http::ClientConnection*>(sender);
		DebugL << "On response complete" 
			<< response << self->readStream<std::stringstream>()->str() << endl;
		self->close();
	}


	//
	/// Create Event Test
	//


	void runCreateEventTest() 
	{
		TraceL << "Starting" << endl;
		
		// Create the event
		Event event("Random Event", "ZOMG, something happened!");

		// Create the transaction
		http::ClientConnection::Ptr trans = client.call("CreateEvent");
		trans->Complete += sdelegate(this, &Tests::onCreateEventComplete);

		// Attach a HTML form writer for event fields
		// TODO: Add a helper method for filling event form from Event object
		http::FormWriter* form = http::FormWriter::create(*trans);
		form->set("event[name]", event.name);
		form->set("event[message]", event.message);
		form->set("event[severity]", event.severityStr());
		form->set("event[origin]", event.origin);
		//form->set("event[timestamp]", event.formatTime());

		trans->send();

		//runLoop();
		
		TraceL << "Ended" << endl;
	}

	void onCreateEventComplete(void* sender, const http::Response& response)
	{
		auto trans = reinterpret_cast<http::ClientConnection*>(sender);
		
		DebugL << "Transaction Complete:" 
			<< "\n\tRequest Head: " << trans->request()
			<< "\n\tResponse Head: " << response
			//<< "\n\tResponse Body: " << trans->incomingBuffer()
			<< endl;

		//assert(response.success());
	}
		

	//
	/// Transaction Asset Upload
	//


	bool gotUploadProgress;
	bool gotUploadComplete;

	void runAssetUploadTest()
	{
		TraceL << "Starting" << endl;

		// Create the transaction
		http::ClientConnection::Ptr trans = client.call("UploadAsset");	
		trans->request().setChunkedTransferEncoding(true);
		trans->OutgoingProgress += sdelegate(this, &Tests::onAssetUploadProgress);
		trans->Complete += sdelegate(this, &Tests::onAssetUploadComplete);

		// Attach a HTML form writer for uploading files
		http::FormWriter* form = http::FormWriter::create(*trans, http::FormWriter::ENCODING_MULTIPART_FORM);
		form->set("asset[type]", "Image");
		form->addPart("asset[file]", new http::FilePart("D:/test.jpg"));
		
		// Send the request
		trans->send();

		//runLoop();

		//assert(gotUploadProgress);
		//assert(gotUploadComplete);
		
		TraceL << "Ended" << endl;
	}

	void onAssetUploadProgress(void* sender, const double& progress)
	{
		auto trans = reinterpret_cast<http::ClientConnection*>(sender);
		gotUploadProgress = true;

		DebugL << "Upload Progress:" << progress << endl;
	}

	void onAssetUploadComplete(void* sender, const http::Response& response)
	{
		auto trans = reinterpret_cast<http::ClientConnection*>(sender);
		gotUploadComplete = true;

		DebugL << "Transaction Complete:" 
			<< "\n\tRequest Head: " << trans->request()
			<< "\n\tResponse Head: " << response
			//<< "\n\tResponse Body: " << trans->incomingBuffer()
			<< endl;

		assert(response.success());
	}


	//
	/// Transaction Asset Download
	//


	bool gotProgress;
	bool gotComplete;

	void runAssetDownloadTest()
	{
		TraceL << "Starting" << endl;

		APIMethod method;
		method.url = "http://localhost:3000/assets/streaming/1645/Administrator_hapyyy_1370318967.jpg";
		http::ClientConnection::Ptr trans = client.call(method);

		trans->setReadStream(new std::ofstream("somefile.jpg", std::ios_base::out | std::ios_base::binary));
		trans->IncomingProgress += sdelegate(this, &Tests::onAssetTransactionIncomingProgress);
		trans->Complete += sdelegate(this, &Tests::onAssetTransactionComplete);
		trans->send();	

		//runLoop();
		
		//assert(gotProgress);
		//assert(gotComplete);
	}

	void onAssetTransactionIncomingProgress(void* sender, const double& progress)
	{
		auto trans = reinterpret_cast<http::ClientConnection*>(sender);
		gotProgress = true;

		DebugL << "Anionu API Incoming Progress:" << progress << endl;
	}

	void onAssetTransactionComplete(void* sender, const http::Response& response) //APIMethod& service, 
	{
		auto trans = reinterpret_cast<http::ClientConnection*>(sender);
		gotComplete = true;
		//assert(trans->incomingBuffer().position() == 0);
		//assert(trans->incomingBuffer().available() == response.getContentLength());

		// TODO: Check file veracity

		DebugL << "Anionu API Response:" 
			<< "\n\tHeaders: " << response
			<< "\n\tPayload Size: " << response.getContentLength()
			<< endl;

		assert(response.success());
	}


	//
	/// Event Loop Helpers
	//

	void runLoop() {
		DebugL << "#################### Running" << endl;
		app.run();
		DebugL << "#################### Ended" << endl;
	}

	void runCleanup() {
		DebugL << "#################### Finalizing" << endl;
		app.finalize();
		DebugL << "#################### Exiting" << endl;
	}
};


} } // namespace scy::anio


int main(int argc, char** argv) 
{	
	//assert(Anionu_API_OAUTH_TOKEN != "authtokenhere" && 
	//	"Get an auth token from http://anionu.com/auth_token");

	Logger::instance().add(new ConsoleChannel("debug", LTrace));
#if TEST_SSL			
	anio::initDefaultSSLContext();				
#endif
	{
		anio::Tests run;
	}
#if TEST_SSL	
	net::SSLManager::destroy();		
#endif
	Logger::destroy();
	return 0;
}