/*
 * project: 	IPK project 1 - webclient
 * author: 		Jan Ondruch, xondru14
 */


/*
4xx, 5xx na stderr
3xx implemet 301, 302 at least
max num of redirections = 5
 */

#include "webclient.h"


unsigned short redirects;	// redirection count, max 5


int main(int argc, char *argv[]) {
	
	string url;

	if (argc == 2)
		url = argv[1];
	else
		return printErr(E_WRONG_PARAMS);

	return client(url);
}


int client(string url) {

	// PARSE URL
	string parsedFilePath;	// /fruit/kiwi.gif (first "/" is already there)
	string parsedHost;		// www.vutbr.cz

	unsigned int portNumber = 80;

	url.erase(0,7);

	string::iterator si;
	bool flg_port = false;

	// get host
  	for (si = url.begin(); si < url.end(); si++) {

  		if (*si == '/') break;
  		if (*si == ':') {
  			flg_port = true;
  			break;
  		}

		parsedHost += (*si);

  	}

  	// get port
  	if (flg_port) {

  		string tempNum;	
  		while (isdigit(*(++si)))
  			tempNum += (*si);

  		portNumber = atoi(tempNum.c_str());

  	}

  	// get filepath
  	while (*si++)
  		parsedFilePath += (*si);


	//printf("PARSEDHOST: %s\n", parsedHost.c_str());
	//printf("PARSEDFPATH: %s\n", parsedFilePath.c_str());
	//if (flg_port) 
	//	printf("PARSEDPORT: %d\n", portNumber);


	// CREATE SOCKET 
	int sockfd;	// socket file descriptor

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
  		return printErr(E_OPENING_SOCKET);


  	// GETHOSTNAME
  	struct sockaddr_in serv_addr;
  	struct hostent *server;

  	// nulling struct
  	bzero((char *) &serv_addr, sizeof(serv_addr));	
  	//serv_addr.sin_family = AF_INET;	// already set in socket() func?
  	serv_addr.sin_port = htons(portNumber);

  	if ((server = gethostbyname(parsedHost.c_str())) == NULL)
		return printErr(E_NO_HOST);

	memcpy(&serv_addr.sin_addr, server->h_addr_list[0], server->h_length);


  	// CONNECT
  	if (connect(sockfd, (const struct sockaddr *) &serv_addr, sizeof(serv_addr)) != 0)
  		return printErr(E_CONNECT_SOCKET);


  	// SEND REQUEST
  	
  	// create message 
  	string msg;	// message to send

	msg.append("HEAD /");	// or HEAD ???
	msg.append(parsedFilePath.c_str());	
	msg.append(" HTTP/1.1\r\nHost: ");
	msg.append(parsedHost.c_str());
	msg.append("\r\nConnection: close\r\n\r\n");

	printf("%s", msg.c_str());

	// send message
	 
	if (write(sockfd, msg.c_str(), msg.length()) < 0) 
	//if (send(sockfd, msg.c_str(), msg.length(), 0) < 0)
        return printErr(E_SEND_REQUST);
		

  	// READ RESPONSE
  	string resp;
  	char buf;

  	// read the response by one char and save it 
  	while(read(sockfd, &buf, 1) > 0) {

  		if (!isspace(buf))
  			resp.append(buf, 1);

  		// maybe some special whitechar checking ... ?

  	}

  	// resolve return code
  	printf("SERVER RESPONSE: %s\n", resp.c_str());


	// destroy socket - shutdown would only hold it
	if (close(sockfd) < 0)
		return printErr(E_CLOSE_SOCKET);

	return EXIT_SUCCESS;

}


/*
 * prints out error code
 */
int printErr(int code) {

	cerr << "Error: " << errMsg[code] << ".\n";
	return code;

}


