#include "Sourcey/Anionu/APIClient.h"
#include "Sourcey/Runner.h"
#include "Sourcey/Util.h"

#include "Poco/format.h"
#include "Poco/Path.h"
#include "Poco/File.h"

#include "Poco/Net/HTMLForm.h"
#include "Poco/Net/FilePartSource.h"

#include <string>
#include <vector>
#include <assert.h>
#include <conio.h>


using namespace std;
using namespace Poco;
using namespace Poco::Net;
using namespace Sourcey;
using namespace Sourcey::Anionu;
using namespace Sourcey::Util;


// Detect Memory Leaks
/*
#ifdef _DEBUG
#include "MemLeakDetect/MemLeakDetect.h"
CMemLeakDetect memLeakDetect;
#endif<DataPacket>
*/


string selectListItem(const string& hint, const vector<string>& list)
{
	cout << hint << ": " << endl;
	for (int i = 0; i < list.size(); i++) {
		cout << "[" << i << "]" << "	" << list[i] << endl;
	}      
	int n = 0;
	cin >> n;
	cout << "Selected: " << n << endl;
	if (list.size() > n)
		return list[n];
	return "";
}


void printList(const string& hint, const vector<string>& list, ostream& ost)
{
	ost << hint << ":\n\n";
	for (unsigned int i = 0; i < list.size(); i++)
		ost << "[" << i << "]" << "	" << list[i] << endl;
	ost << "\n";
}


vector<string> getInputList()
{
	cout << "Please enter comma separated values: " << endl;
	string s;
	getline(cin,s);
	return split(s, ",");
}


bool getYesNoAnswer(const string& question)
{
	cout << question << " [Y/N]" << endl;	
	char ch = toupper(getch());
	if (ch == 'Y')
		return true;
	return false;
}


class AnionuAPIConsole: public APIClient
{
public:
	AnionuAPIConsole()
	{
		Logger::instance().add(new ConsoleChannel("debug", TraceLevel));
	}

	~AnionuAPIConsole()
	{
		Logger::uninitialize();
	}

	void onAPITransactionComplete(void*, APIMethod& service, HTTP::Response& response)
	{
		LogTrace() << "Anionu API Response:" 
			<< "\n\tMethod: " << service.name
			<< "\n\tStatus: " << response.getStatus()
			<< "\n\tReason: " << response.getReason()
			<< "\n\tSuccess: " << response.success()
			<< endl;
		
		LogTrace() << "Anionu API Response Headers:" << endl;
		response.write(cout);

		LogTrace() << "Anionu API Response Body:" << endl;
		cout << response.body.str() << endl;
	}
};


int main(int argc, char** argv)
{
	AnionuAPIConsole api;	
	api.setCredentials(Anionu_API_USERNAME, Anionu_API_PASSWORD, "http://localhost:3000");
	api.loadMethods();
	
	string service("GetAccount");
	string assetPath("D:/test.avi");
	string assetType("Video");
	vector<string> assetTypes;
	assetTypes.push_back("Video");
	assetTypes.push_back("Audio");
	assetTypes.push_back("Image");
	assetTypes.push_back("Archive");

	char o = 0;
	while (o != 'Q') 
	{	
		cout << 
			"COMMANDS:\n\n"
			"  A	Set Target API Method\n"
			"  C	Call Basic Method\n"
			"  T	Set Asset Type\n"
			"  Y	Set Asset Path\n"
			"  U	Call Upload Asset Method\n"
			"  L	List Methods\n"
			"  Q	Quit.\n\n";
		
		o = toupper(getch());
		
		// Set Target API Method
		if (o == 'A') {	
			cout << "Enter target service name: " << endl;
			getline(cin, service);
		}

		// Call Basic Method
		else if (o == 'C') {
			assert(!service.empty());
			APITransaction* transaction = api.call(service);
			transaction->APITransactionComplete += delegate(&api, &AnionuAPIConsole::onAPITransactionComplete);
			transaction->send();	
		}
		
		// Set Asset Type
		else if (o == 'T') {
			selectListItem("Select asset type", assetTypes);
		}

		// Set Asset Path
		else if (o == 'Y') {
			cout << "Enter asset path: " << endl;
			getline(cin, assetPath);
		}

		// Call Upload Asset Method
		else if (o == 'U') {
			APITransaction* transaction = api.call("UploadAsset");
			transaction->request().form = new HTMLForm(HTMLForm::ENCODING_MULTIPART);
			transaction->request().form->set("asset[type]", assetType);
			transaction->request().form->addPart("asset[file]", new FilePartSource(assetPath));
			transaction->APITransactionComplete += delegate(&api, &AnionuAPIConsole::onAPITransactionComplete);
			transaction->send();
		} 

		// List Methods
		else if (o == 'L') {
			LogTrace() << "Listing Methods:" << endl;
			api.methods().print(cout);
		} 

		// Reload Method List
		else if (o == 'R') {
			api.loadMethods();
		}
	}
	
	LogTrace() << "Bye!" << endl;
	return 0;
}