#ifndef CURLTEST_MAILSERVER_H
#define CURLTEST_MAILSERVER_H

#include <cstdlib>
#include <vector>
#include <string>
//Simple header file for the mailserver

using namespace std;


class MailServer {

public:
	MailServer();
	~MailServer();

	vector<string> message, messageInfo, usrName, pass, colon, usrPopServ, curlUsrPassParam;
	

private:

};


#endif //CURLTEST_MAILSERVER_H

