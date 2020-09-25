#ifndef _WEBCLIENT_H
#define _WEBCLIENT_H


using namespace std;

#include <iostream>
#include <cstdlib>
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


int printErr(int code);
int client(string url);


char const *errMsg[] = {

	"Wrong input argument",
	"Error opening socket",
	"Error connecting socket",
	"Error - no such host",
	"Cannot send a request",
	"Cannot close socket"

};


enum errCodes {

	E_WRONG_PARAMS 		= 0,
	E_OPENING_SOCKET,
	E_CONNECT_SOCKET,
	E_NO_HOST,
	E_SEND_REQUST,
	E_CLOSE_SOCKET

};

#endif