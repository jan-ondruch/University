#ifndef _WEBCLIENT_H
#define _WEBCLIENT_H


using namespace std;

#include <iostream>
#include <cstdlib>
#include <fstream>
#include <cstdio>
#include <stdlib.h>
#include <locale.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <regex.h>
#include <unistd.h>
#include <string.h>
#include <algorithm>


int printErr(int code);
int client(string url);
int create_socket(int *sock, string parsedHost, unsigned int portNumber);
string create_msg(string head_get, string parsedFilePath, string http_ver, string parsedHost);
string read_resp(int sockfd);
int send_msg(int sockfd, string msg);
string getFileName(string parsedFilePath);
void writeContent(string downloadFile, string resp);
string chunkedDecode(string resp);


unsigned short redirects = 0;
string redir_filename = "";

char const *errMsg[] = {

	"Wrong input argument",
	"Error opening socket",
	"Error connecting socket",
	"Error - no such host",
	"Cannot send a request",
	"Cannot close socket",
	"Maximum number of redirections reached",
	"401 - Unauthorized: Access is denied due to invalid credentials",
	"403 - Forbidden",
	"404 - Not Found",
	"500 - Internal server error",
	"503 - Service unavailable",
	"505 - HTTP version not supported",
	"Other error",
	"Cannot open file"

};

enum errCodes {

	E_WRONG_PARAMS 		= 0,
	E_OPENING_SOCKET,
	E_CONNECT_SOCKET,
	E_NO_HOST,
	E_SEND_REQUST,
	E_CLOSE_SOCKET,
	E_REDIRS_LIMIT,				
	E_401,			
	E_403,
	E_404,
	E_500,
	E_503,
	E_505,
	E_OTHER,
	E_OPEN_FILE

};

#endif