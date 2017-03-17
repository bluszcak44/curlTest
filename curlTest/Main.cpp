#include <stdio.h>
#include <curl/curl.h>
#include <iostream>
#include <cstring>
#include <sstream>
#include "MailServer.h"

#ifdef _WIN32
#include <Windows.h>
#elif defined __linux__
#include <termios.h>
#include <unistd.h>
#endif

//Written by: Blaine Luszcak

//prototype functions
void mailboxInfo(MailServer &mailSrv);
static size_t writeCallback(void *contents, size_t size, size_t nmemb, void *userp);
void mailboxMessages(MailServer &mailserv); 
void echoWindows(bool on);
void echoLinux(bool on);

using namespace std;

int main() {
    
	MailServer *mailSrv = new MailServer();
    
	mailboxInfo(*mailSrv);
	mailboxMessages(*mailSrv);

	cout << "We made it to the end of the program. " << "\n" << endl;
	printf("Press enter to continue...\n");
	getchar();
	return 0;
}

//retrieve the messages from the eng pop3 server
void mailboxInfo(MailServer &mailSrv) {
	CURL *curl;
	CURLcode res;
	string readBuff, tempUsr, usrName, tempPass, pass, curlUsrPassParam, colon, colonTemp, usrPopServ;


	cout << "Please enter a username: ";
	cin >> tempUsr;
	mailSrv.usrName.push_back(tempUsr);

	echoWindows(false); //use on win
	//echoLinux(false); //use on linux
	cout << "Please enter the password: ";
	cin >> tempPass;
	mailSrv.pass.push_back(tempPass);
	echoWindows(true); //use on win
	//echoLinux(true); //use on linux
	cout << endl; 

	colonTemp = ":";
	mailSrv.colon.push_back(colonTemp);
	
	//saving this for when we need to cyle through the messages in the next funct
	usrName = mailSrv.usrName[0];
	pass = mailSrv.pass[0];
	colon = mailSrv.colon[0];

	//pass into curlParam for Username and password
	curlUsrPassParam = usrName + colon + pass;
	mailSrv.curlUsrPassParam.push_back(curlUsrPassParam);

	//pass {id} into pop3 req
	usrPopServ = "pop3s://" + usrName + "@pop3.eng.utoledo.edu/";
	mailSrv.usrPopServ.push_back(usrPopServ);

	curl = curl_easy_init();
	if (curl) {
		/* Set username and password */
		curl_easy_setopt(curl, CURLOPT_USERPWD, curlUsrPassParam.c_str());

		/* This will get us the messages + sizes */
		curl_easy_setopt(curl, CURLOPT_URL, usrPopServ.c_str());

		//attempting to read the email messages into a string
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuff);
		
		/* Perform the request, res will get the return code */
		res = curl_easy_perform(curl);

		/* Check for errors */
		if (res != CURLE_OK)
			fprintf(stderr, "curl_easy_perform() failed: %s\n",
				curl_easy_strerror(res));

		/* always cleanup */
		curl_easy_cleanup(curl);

		//push into a string so we can count how many lines there are = # of messages there are
		stringstream stream(readBuff);
		while (getline(stream, readBuff)) {
			mailSrv.messageInfo.push_back(readBuff);
		}

		for (int i = 0; i < mailSrv.messageInfo.size(); i++) {
			cout << mailSrv.messageInfo[i] << endl;
		}
	}
}

//create a callback to read the message contents into a string
static size_t writeCallback(void *contents, size_t size, size_t nmemb, void *userp) {
	((string*)userp)->append((char*)contents, size * nmemb);
	return size * nmemb;
}

void mailboxMessages(MailServer &mailserv) {
	CURL *curl;
	CURLcode res;
	string readBuff, usrName, pass, curlUsrPassParam, usrPopServ, addFrom;

	for (int i = 0; i < mailserv.messageInfo.size(); i++) {
		int id;

		id = i + 1;

		curlUsrPassParam = mailserv.usrName[0] + mailserv.colon[0] + mailserv.pass[0];
		usrPopServ = mailserv.usrPopServ[0] + to_string(id);

		curl = curl_easy_init();
		if (curl) {
			/* Set username and password */
			curl_easy_setopt(curl, CURLOPT_USERPWD, curlUsrPassParam.c_str());

			/* This will only fetch the message with ID "1" of the given mailbox */
			curl_easy_setopt(curl, CURLOPT_URL, usrPopServ.c_str());

			//attempting to read the email messages into a string
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuff);

			/* Perform the request, res will get the return code */
			res = curl_easy_perform(curl);

			/* Check for errors */
			if (res != CURLE_OK)
				fprintf(stderr, "curl_easy_perform() failed: %s\n",
					curl_easy_strerror(res));

			/* always cleanup */
			curl_easy_cleanup(curl);

			addFrom = "From - \n" + readBuff;

			mailserv.message.push_back(addFrom);
			readBuff = "";

		}
	}
}

//turn off echo in Windows
void echoWindows(bool on = true) {
	DWORD  mode;
	HANDLE hConIn = GetStdHandle(STD_INPUT_HANDLE);
	GetConsoleMode(hConIn, &mode);
	mode = on
		? (mode | ENABLE_ECHO_INPUT)
		: (mode & ~(ENABLE_ECHO_INPUT));
	SetConsoleMode(hConIn, mode);
}

//turn off echo in Linux
//void echoLinux(bool on = true) {
//	struct termios settings;
//	tcgetattr(STDIN_FILENO, &settings);
//	settings.c_lflag = on
//		? (settings.c_lflag | ECHO)
//		: (settings.c_lflag & ~(ECHO));
//	tcsetattr(STDIN_FILENO, TCSANOW, &settings);
//}