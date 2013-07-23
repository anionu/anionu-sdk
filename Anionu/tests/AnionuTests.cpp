#include "Sourcey/Base.h"
#include "Sourcey/Application.h"
#include "Sourcey/Logger.h"
#include "Anionu/APIClient.h"
#include "Sourcey/HTTP/Form.h"
#include "Sourcey/HTTP/Packetizers.h"
#include "Sourcey/Util.h"

#include "Poco/Format.h"
#include "Poco/Path.h"
#include "Poco/File.h"

#include "assert.h"


using namespace std;
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


#define TEST_SSL 0 //1


class Tests
{
public:
	Application app; 

	Tests()
	{	
#ifdef _MSC_VER
		_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
		{			

#if TEST_SSL
			// Init SSL Context 
			SSLContext::Ptr ptrContext = new SSLContext(
				SSLContext::CLIENT_USE, "", "", "", 
				SSLContext::VERIFY_NONE, 9, false, 
				"ALL:!ADH:!LOW:!EXP:!MD5:@STRENGTH");		
			SSLManager::instance().initializeClient(ptrContext);
#endif
			
			//runTransactionTest();
			//runAuthenticationTest();
			runAssetUploadTransactionTest();
			//runAssetDownloadTransactionTest();
			
		// dome in base ytets + authenticate uri for Anionu tests

#if TEST_SSL
			// Shutdown SSL
			SSLManager::instance().shutdown();
#endif

			// Shutdown the garbage collector so we can free memory.
			GarbageCollector::instance().shutdown();
		
			// Run the final cleanup
			runCleanup();
		}
	}
	
	// ============================================================================
	// Transaction Test
	//
	void runTransactionTest(const string& service = "GetAccount") 
	{
		traceL("TransactionTest") << "Starting" << endl;

		APIClient client;
		client.setCredentials(Anionu_API_USERNAME, Anionu_API_PASSWORD, "http://localhost:3000");
		APITransaction* transaction = client.call(service);

		transaction->Complete += delegate(this, &Tests::onTransactionComplete);
		transaction->send();

		runLoop();
	}

	void onTransactionComplete(void* sender, const http::Response& response)
	{
		APITransaction* transaction = reinterpret_cast<APITransaction*>(sender);

		debugL() << "Anionu API Response:" 
			<< "\n\tHeaders: " << response
			<< endl;

		//assert(transaction->incomingBuffer().position() == 0);
		assert(transaction->incomingBuffer().available() == response.getContentLength());
	}
		
	
	// ============================================================================
	// Authentication Test
	//
	void runAuthenticationTest() 
	{
		traceL("TransactionTest") << "Starting" << endl;
		http::ClientConnection* conn = new http::ClientConnection("http://localhost:3000");
		conn->Complete += delegate(this, &Tests::onAuthenticationResponse);
		conn->request().setMethod("GET");
		conn->request().setKeepAlive(false);
		conn->request().setURI("http://localhost:3000/authenticate.json");

		http::BasicAuthenticator cred(Anionu_API_USERNAME, Anionu_API_PASSWORD);
		cred.authenticate(conn->request());

		conn->send();

		runLoop();
	}

	void onAuthenticationResponse(void* sender, const http::Response& response)
	{	
		http::ClientConnection* conn = reinterpret_cast<http::ClientConnection*>(sender);

		traceL("TransactionTest") << "API Authenticate Response:\n" 
			<< response.getStatus() << endl;	

		// Handle authentication success response.
		if (response.getStatus() == 200) {	
		}
	}
	
	// ============================================================================
	// Transaction Asset Upload
	//
	bool gotUploadProgress;
	bool gotUploadComplete;

	void runAssetUploadTransactionTest()
	{
		traceL("TransactionTest") << "Starting" << endl;
			
		// Create the API client
		APIClient client;
		client.setCredentials(Anionu_API_USERNAME, Anionu_API_PASSWORD, "http://localhost:3000");

		// Create the transaction
		APITransaction* conn = client.call("UploadAsset");	
		/*
		http::ClientConnection* conn = new http::ClientConnection("http://localhost:8000");
		conn->request().setMethod("POST");
		//conn->request().setKeepAlive(false);
		conn->request().setURI("/upload");
		*/

		conn->OutgoingProgress += delegate(this, &Tests::onAssetUploadProgress);
		conn->Complete += delegate(this, &Tests::onAssetUploadComplete);
		
		//Poco::File f("D:/somefile.jpg");
		//assert(f.exists());
		//conn->request().setContentLength(f.getSize());
		//conn->request().setContentLength(1);

		// Attach a HTML form writer for uploading files
		http::FormWriter* form = new http::FormWriter(*conn, http::FormWriter::ENCODING_MULTIPART);
		form->set("asset[type]", "Image");
		form->addPart("asset[file]", new http::FilePart("D:\\somefile.png"));
		conn->Outgoing.attachSource(form, true, true);
		conn->Outgoing.attach(new http::ChunkedAdapter(*conn), 0, true);
		
			//Poco::File f
		// Send the request
		conn->send();

		runLoop();

		assert(gotUploadProgress);
		assert(gotUploadComplete);
		
		traceL("TransactionTest") << "Ended" << endl;
	}

	void onAssetUploadProgress(void* sender, const http::TransferProgress& transfer)
	{
		APITransaction* transaction = reinterpret_cast<APITransaction*>(sender);
		gotUploadProgress = true;

		debugL("UploadAssetTest") << "Upload Progress:" 
			<< "\n\tCurrent: " << transfer.current
			<< "\n\tTotal: " << transfer.current
			<< "\n\tProgress: " << transfer.progress()
			<< endl;
	}

	void onAssetUploadComplete(void* sender, const http::Response& response)
	{
		APITransaction* transaction = reinterpret_cast<APITransaction*>(sender);
		gotUploadComplete = true;

		assert(response.success());

		debugL("UploadAssetTest") << "Transaction Complete:" 
			<< "\n\tRequest: " << transaction->request()
			<< "\n\tResponse: " << response
			<< "\n\tResponse Body: " << transaction->incomingBuffer()
			<< endl;
	}


	// ============================================================================
	// Transaction Asset Download
	//
	bool gotProgress;
	bool gotComplete;

	void runAssetDownloadTransactionTest()
	{
		traceL("TransactionTest") << "Starting" << endl;

		APIClient client;
		client.setCredentials(Anionu_API_USERNAME, Anionu_API_PASSWORD, "http://localhost:3000");

		APIMethod method;
		method.url = "http://localhost:3000/assets/streaming/1645/Administrator_hapyyy_1370318967.jpg";
		APITransaction* transaction = client.call(method);

		transaction->setRecvStream(new std::ofstream("somefile.jpg", std::ios_base::out | std::ios_base::binary));
		transaction->IncomingProgress += delegate(this, &Tests::onAssetTransactionIncomingProgress);
		transaction->Complete += delegate(this, &Tests::onAssetTransactionComplete);
		transaction->send();	

		runLoop();
		
		assert(gotProgress);
		assert(gotComplete);
	}

	void onAssetTransactionIncomingProgress(void* sender, const http::TransferProgress& transfer)
	{
		APITransaction* transaction = reinterpret_cast<APITransaction*>(sender);
		gotProgress = true;

		debugL() << "Anionu API Incoming Progress:" 
			<< "\n\tCurrent: " << transfer.current
			<< "\n\tTotal: " << transfer.current
			<< "\n\tProgress: " << transfer.progress()
			<< endl;
	}

	void onAssetTransactionComplete(void* sender, const http::Response& response) //APIMethod& service, 
	{
		APITransaction* transaction = reinterpret_cast<APITransaction*>(sender);
		gotComplete = true;
		//assert(transaction->incomingBuffer().position() == 0);
		//assert(transaction->incomingBuffer().available() == response.getContentLength());

		// TODO: Check file veracity

		debugL() << "Anionu API Response:" 
			<< "\n\tHeaders: " << response
			<< "\n\tPayload Size: " << response.getContentLength()
			<< endl;
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
		app.cleanup();
		debugL("Tests") << "#################### Exiting" << endl;
	}
};


} } // namespace scy::anio


int main(int argc, char** argv) 
{	
	Logger::instance().add(new ConsoleChannel("debug", TraceLevel));
	{
		anio::Tests run;
	}
	Logger::uninitialize();
	return 0;
}