/*
 * project: 	ISA project - POP3 server
 * author: 		Jan Ondruch, xondru14
 * file:			server.hpp
 */

#ifndef _SERVER_HPP
#define _SERVER_HPP

#define BUFSIZE	512

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <pthread.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

#include "args.hpp"
#include "mail.hpp"

using namespace std;

// class for basic server setup
class Server
{
	private:
		struct sockaddr_in serv_addr, clnt_addr;	// addresses structs
		socklen_t sinlen;	// sock length
	public:
		int sockfd;	// socket for estabilishing communication
		void ConnectSocket(Arguments Args);
		int AcceptClients(Arguments Args);
};

#endif