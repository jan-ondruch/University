/*
 * project: 	ISA project - POP3 server
 * author: 		Jan Ondruch, xondru14
 * file:			main.hpp
 */

#ifndef _MAIN_HPP
#define _MAIN_HPP

#include <stdio.h>
#include <signal.h>
#include <string>
#include <vector>

#include "args.hpp"
#include "server.hpp"

using namespace std;

// structure for server setup
struct serv_info_struct
{
	int newsockfd;		// client socket
  bool clear_pass;	// clear_pass flag
  string username;	// parsed username
  string password;	// parsed password
  string maildir;		// given Maildir path
  bool reset;				// reset flag
};

int reset_server();
void SIGhandler(int sig);

#endif