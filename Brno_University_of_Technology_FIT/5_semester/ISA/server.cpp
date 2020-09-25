/*
 * project: 	ISA project - POP3 server
 * author: 		Jan Ondruch, xondru14
 * file:			server.cpp
 */

#ifndef _SERVER_CPP
#define _SERVER_CPP

#include "server.hpp"


/**
 * Establishes TCP connection.
 * @param Args Arguments parsed arguments
 */
void Server::ConnectSocket(Arguments Args)
{
	// fill memory with a constant byte
	memset(&serv_addr, 0, sizeof(serv_addr));

	// create socket
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
	{
		cerr << "Server: Error creating socket.\n";
    exit(-1);
  }
	
  serv_addr.sin_family = AF_INET;	// IPv4
  serv_addr.sin_port = htons(Args.port);	// endians conversion
  serv_addr.sin_addr.s_addr = INADDR_ANY;	// use any IP available

  // enables to quickly restart the server
  int optval = 1;
  if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int)) < 0)
    cerr << "setsockopt(SO_REUSEADDR) failed\n";

  // bind the socket
  if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) != 0)
  {
  	cerr << "Error binding socket. Given port may be already in use.\n";
  	exit(-1);
  }

  // listen to clients
  if (listen(sockfd, 5))
  	cerr << "Error listening to clients.\n";
}


/**
 * Accepts clients in a loop and creates new threads for them.
 * Uses non-blocking socket and ensures parallel communication with clients.
 * @param Args Arguments parsed arguments
 * @return  int success value
 */
int Server::AcceptClients(Arguments Args)
{
	// save arguments to a struct to pass to threads
	struct serv_info_struct serv_info;
	serv_info.clear_pass = Args.clear_pass;
	serv_info.username = Args.username;
	serv_info.password = Args.password;
	serv_info.maildir = Args.maildir_path;
	serv_info.reset = Args.reset;

	// listen to clients, accept them and keep the server running
	while(1)
	{
		sinlen = sizeof(clnt_addr);
		// accept new client
		int newsockfd;
		if ((newsockfd = accept(sockfd, (struct sockaddr *) &clnt_addr, 
			&sinlen)) < 0)
			cerr << "Error accepting a client.\n";

		serv_info.newsockfd = newsockfd;
		// start creating new threads
		pthread_t tid;
		if (int result = pthread_create(&tid, NULL, &client_thread, (void *)&serv_info) != 0)
		{
			if (result == EAGAIN)
			{
				cerr << "Server run out of threads.\n";
				continue;
			}
			else
			{
				cerr << "Error while creating thread for a client.\n";
				continue;
			}
		}

		// automatically release resources
		pthread_detach(tid);
	}

	// terminate calling thread
	pthread_exit(NULL);

	close(sockfd);
	return 0;
}

#endif