/*
 * project: 	ISA project - POP3 server
 * author: 		Jan Ondruch, xondru14
 * file:			args.hpp
 */

#ifndef _ARGS_HPP
#define _ARGS_HPP

#include <unistd.h>
#include <iostream>
#include <string.h>
#include <string>
#include <vector>
#include <fstream>
#include <sys/stat.h>
#include <fstream>
#include <iostream>
#include <cstddef>

#include "string.hpp"

using namespace std;

// arguments parsing class
class Arguments
{
	private:
		string auth_file_path;	// auth file pathname
	public: 
		bool clear_pass;	// clear pass, password can be sent without encryption
		string username;	// username from the auth_file, if given
		string password;	// password from the auth_file, if given
		string maildir_path;	// Maildir directory path
		bool reset;	// server reset, temp files and e-mails are deleted
		int port;	// port on which the server will run
		Arguments(int argc, char **argv);	// args constructor
		void PrintHelp();	// prints help message
		void ParseAuthFile();
};

#endif