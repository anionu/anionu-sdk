#include "scy/base.h"
#include "scy/application.h"
#include "anionu/apiclient.h"
#include "anionu/event.h"
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
#define Anionu_API_ENDPOINT    "http://localhost:3000" //"https://anionu.com" //
#define Anionu_API_OAUTH_TOKEN "ya29.1.AADtN_V5-IqRTru3ZOS5PNhrMcl_9Ui8se7P7gp8bndEE7HW_Cb_R6y0_oDymM3H5fPtew"


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
			client.setEndpoint(Anionu_API_ENDPOINT);
#if Anionu_ENABLE_PASSWORD
			client.setCredentials(Anionu_API_USERNAME, Anionu_API_KEY, Anionu_API_ENDPOINT); 
#else
			client.setCredentials(Anionu_API_OAUTH_TOKEN);
#endif

			// Run tests
			runAssetUploadTest();
#if 0
			//runCreateEventTest();
			//testAuthentication();
			//runCreateEventTest();
			//runAssetDownloadTest();
			runLoop();
#endif

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
		auto conn = http::Client::instance().createConnection(Anionu_API_ENDPOINT);
		conn->Headers += sdelegate(this, &Tests::onAuthenticationHeaders);
		conn->Complete += sdelegate(this, &Tests::onAuthenticationComplete);
		conn->setReadStream(new std::stringstream);
		conn->request().setURI("/authenticate.json");
		conn->send(); // send default GET /
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

	// ============================================================================
	// Create Event Test
	//
	void runCreateEventTest() 
	{
		TraceL << "Starting" << endl;
		
		// Create the event
		Event event("Random Event", "Umm ... something happened");

		// Create the transaction
		http::ClientConnection::Ptr trans = client.call("CreateEvent");
		trans->Complete += sdelegate(this, &Tests::onCreateEventComplete);

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
			
	// ============================================================================
	// Transaction Asset Upload
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


	// ============================================================================
	// Transaction Asset Download
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

	// ============================================================================
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
	Logger::instance().add(new ConsoleChannel("debug", LTrace));
	{
		anio::Tests run;
	}
	Logger::shutdown();
	return 0;
}