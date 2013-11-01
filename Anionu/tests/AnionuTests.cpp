#include "Sourcey/Base.h"
#include "Sourcey/Application.h"
#include "Anionu/APIClient.h"
#include "Anionu/Event.h"
#include "Sourcey/Net/SSLSocket.h"
#include "Sourcey/Net/SSLManager.h"
#include "Sourcey/HTTP/Form.h"
#include "Sourcey/HTTP/Packetizers.h"
#include "Sourcey/Util.h"

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
#define Anionu_API_ENDPOINT "https://anionu.com" //"http://localhost:3000"


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
#if TEST_SSL
			// Initialize SSL Context 
			net::SSLContext::Ptr ptrContext = new net::SSLContext(
				net::SSLContext::CLIENT_USE, "", "", "", 
				net::SSLContext::VERIFY_NONE, 9, false, 
				"ALL:!ADH:!LOW:!EXP:!MD5:@STRENGTH");		
			net::SSLManager::instance().initializeClient(ptrContext);
#endif
		
			// Initialize client credentials
			client.setCredentials(Anionu_API_USERNAME, Anionu_API_PASSWORD, Anionu_API_ENDPOINT); 
			
			// Run tests
			runCreateEventTest();
			//testAuthentication();

			//for (int i = 0; i < 10; i++) {
				//runCreateEventTest();
				//runAssetUploadTest();
				//runAssetDownloadTest();
			//}
			runLoop();

#if TEST_SSL
			// Shutdown SSL
			net::SSLManager::instance().shutdown();
#endif

			// Shutdown the API client
			//client.shutdown();

			// Shutdown the garbage collector to free all memory
			//GarbageCollector::instance().shutdown();
		
			// Run the final cleanup
			runCleanup();
		}
	}
	
	// ============================================================================
	// Authentication Test
	//	
	void testAuthentication()
	{
		auto conn = http::createConnection(Anionu_API_ENDPOINT);
		conn->Headers += delegate(this, &Tests::onAuthenticationHeaders);
		conn->Complete += delegate(this, &Tests::onAuthenticationComplete);
		conn->setReadStream(new std::stringstream);
		conn->request().setURI("/authenticate.json");
		conn->send(); // send default GET /
	}	
	
	void onAuthenticationHeaders(void*, http::Response& res)
	{	
		debugL("StandaloneClientConnectionTest") << "On response headers: " << res << endl;
	}
	
	void onAuthenticationComplete(void* sender, const http::Response& response)
	{		
		auto self = reinterpret_cast<http::ClientConnection*>(sender);
		debugL("StandaloneClientConnectionTest") << "On response complete" 
			<< response << self->readStream<std::stringstream>()->str() << endl;
		self->close();
	}

	// ============================================================================
	// Create Event Test
	//
	void runCreateEventTest() 
	{
		traceL("CreateEventTest") << "Starting" << endl;
		
		// Create the event
		Event event("Random Event", "Umm ... something happened");

		// Create the transaction
		APITransaction* trans = client.call("CreateEvent");
		trans->Complete += delegate(this, &Tests::onCreateEventComplete);

		// Attach a HTML form writer for event fields
		// TODO: Add a helper method for filling event form from Event object
		http::FormWriter* form = http::FormWriter::create(*trans);
		form->set("event[name]", event.name);
		form->set("event[message]", event.message);
		form->set("event[severity]", event.severityStr());
		form->set("event[origin]", event.origin); //Str()
		//form->set("event[timestamp]", event.formatTime());

		trans->send();

		//runLoop();
		
		traceL("CreateEventTest") << "Ended" << endl;
	}

	void onCreateEventComplete(void* sender, const http::Response& response)
	{
		auto trans = reinterpret_cast<APITransaction*>(sender);
		
		debugL("CreateEventTest") << "Transaction Complete:" 
			<< "\n\tRequest Head: " << trans->request()
			<< "\n\tResponse Head: " << response
			//<< "\n\tResponse Body: " << trans->incomingBuffer()
			<< endl;

		//assert(response.success());
	}
			
	// ============================================================================
	// Transaction Asset Upload
	//
	bool gotUploadProgress;
	bool gotUploadComplete;

	void runAssetUploadTest()
	{
		traceL("Test") << "Starting" << endl;

		// Create the transaction
		APITransaction* trans = client.call("UploadAsset");	
		trans->OutgoingProgress += delegate(this, &Tests::onAssetUploadProgress);
		trans->Complete += delegate(this, &Tests::onAssetUploadComplete);

		// Attach a HTML form writer for uploading files
		http::FormWriter* form = http::FormWriter::create(*trans, http::FormWriter::ENCODING_MULTIPART);
		form->set("asset[type]", "Image");
		form->addFile("asset[file]", new http::FilePart("D:/test.jpg"));
		
		// Send the request
		trans->send();

		//runLoop();

		//assert(gotUploadProgress);
		//assert(gotUploadComplete);
		
		traceL("Test") << "Ended" << endl;
	}

	void onAssetUploadProgress(void* sender, const double& progress)
	{
		auto trans = reinterpret_cast<APITransaction*>(sender);
		gotUploadProgress = true;

		debugL("UploadAssetTest") << "Upload Progress:" << progress << endl;
	}

	void onAssetUploadComplete(void* sender, const http::Response& response)
	{
		auto trans = reinterpret_cast<APITransaction*>(sender);
		gotUploadComplete = true;

		debugL("UploadAssetTest") << "Transaction Complete:" 
			<< "\n\tRequest Head: " << trans->request()
			<< "\n\tResponse Head: " << response
			//<< "\n\tResponse Body: " << trans->incomingBuffer()
			<< endl;

		assert(response.success());
	}


	// ============================================================================
	// Transaction Asset Download
	//
	bool gotProgress;
	bool gotComplete;

	void runAssetDownloadTest()
	{
		traceL("Test") << "Starting" << endl;

		APIMethod method;
		method.url = "http://localhost:3000/assets/streaming/1645/Administrator_hapyyy_1370318967.jpg";
		APITransaction* trans = client.call(method);

		trans->setReadStream(new std::ofstream("somefile.jpg", std::ios_base::out | std::ios_base::binary));
		trans->IncomingProgress += delegate(this, &Tests::onAssetTransactionIncomingProgress);
		trans->Complete += delegate(this, &Tests::onAssetTransactionComplete);
		trans->send();	

		//runLoop();
		
		//assert(gotProgress);
		//assert(gotComplete);
	}

	void onAssetTransactionIncomingProgress(void* sender, const double& progress)
	{
		auto trans = reinterpret_cast<APITransaction*>(sender);
		gotProgress = true;

		debugL() << "Anionu API Incoming Progress:" << progress << endl;
	}

	void onAssetTransactionComplete(void* sender, const http::Response& response) //APIMethod& service, 
	{
		auto trans = reinterpret_cast<APITransaction*>(sender);
		gotComplete = true;
		//assert(trans->incomingBuffer().position() == 0);
		//assert(trans->incomingBuffer().available() == response.getContentLength());

		// TODO: Check file veracity

		debugL() << "Anionu API Response:" 
			<< "\n\tHeaders: " << response
			<< "\n\tPayload Size: " << response.getContentLength()
			<< endl;

		assert(response.success());
	}

	// ============================================================================
	//
	void runLoop() {
		debugL("Tests") << "#################### Running" << endl;
		app.run();
		debugL("Tests") << "#################### Ended" << endl;
	}

	void runCleanup() {
		debugL("Tests") << "#################### Finalizing" << endl;
		app.finalize();
		debugL("Tests") << "#################### Exiting" << endl;
	}
};


} } // namespace scy::anio


int main(int argc, char** argv) 
{	
	Logger::instance().add(new ConsoleChannel("debug", LTrace));
	{
		anio::Tests run;
	}
	Logger::shutdown();
	return 0;
}