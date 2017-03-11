#include <stdio.h>
#include <curl/curl.h>
#include <iostream>
#include <cstring>

using namespace std;
int main() {
    
	CURL *curl;
    CURLcode res;

    curl = curl_easy_init();
    if(curl) {
         /* Set username and password */ 
         curl_easy_setopt(curl, CURLOPT_USERPWD, "bluszcak:Googl44");

         /* This will only fetch the message with ID "1" of the given mailbox */ 
         curl_easy_setopt(curl, CURLOPT_URL, "pop3s://bluszcak@pop3.eng.utoledo.edu/1");

         /* Perform the request, res will get the return code */ 
         res = curl_easy_perform(curl);

         /* Check for errors */ 
         if(res != CURLE_OK)
         fprintf(stderr, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(res));

         /* always cleanup */ 
         curl_easy_cleanup(curl);
    }

	cout << "We made it to the end of the program. " << "\n" << endl;
	printf("Press enter to continue...\n");
	getchar();
	return 0;
}