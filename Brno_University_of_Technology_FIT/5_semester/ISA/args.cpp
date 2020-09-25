/*
 * project: 	ISA project - POP3 server
 * author: 		Jan Ondruch, xondru14
 * file:			args.cpp
 */

/*
Copyright:
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>
*/

#ifndef _ARGS_CPP
#define _ARGS_CPP

#include "args.hpp"

/**
 * Prints help message.
 */
void Arguments::PrintHelp()
{
	cout << "POP3 server (ISA project 2017)\n\
    \nusage:\n\
    \t./popser [-h] [-a PATH] [-c] [-p PORT] [-d PATH] [-r]\n\
    \narguments:\n\
    \t[-h|--help]\tPrints help (this message) and exit\n\
    \t[-a]\t\tPath to the authentication file\n\
    \t[-c]\t\tAccepts non-encrypted login password method (optional parameter)\n\
    \t[-p]\t\tRuns application on this port\n\
    \t[-d]\t\tPath to the Maildir folder\n\
    \t[-r]\t\tResets the Maildir (optional parameter)\n";
	return;
}


/**
 * Parses the auth file username and password.
 */
void Arguments::ParseAuthFile()
{
  ifstream input(auth_file_path);
  string line;
  vector<string> parsed_line;

  // get the first line: username
  getline(input, line);
  if (line.empty())
  {
    cerr << "Invalid auth file format.\n";
    exit(-1);
  }

  parsed_line = parse_string(line);
  if (parsed_line.size() != 3)  // invalid format
  {
    cerr << "Invalid auth file format (username).\n";
    exit(-1);
  }

  if ((parsed_line[0] == "username") && (parsed_line[1] == "="))
  {
    // check possible trailing whitechars in the username
    if (isspace(line.back()))
    {
      cerr << "Invalid auth file format (username).\n";
      exit(-1);
    }
    if (isspace(parsed_line[2].at(0)))
    {
      cerr << "Invalid auth file format (username).\n";
      exit(-1);
    }

    username = parsed_line[2];
  }
  else
  {
    cerr << "Invalid auth file format (username).\n";
    exit(-1);
  }

  // get the second line: password
  getline(input, line);
  if (line.empty())
  {
    cerr << "Invalid auth file format.\n";
    exit(-1);
  }

  parsed_line = parse_string(line);
  if (parsed_line.size() < 3) // invalid format
  {
    cerr << "Invalid auth file format (password).\n";
    exit(-1);
  }

  if ((parsed_line[0] == "password") && (parsed_line[1] == "="))
  { 
    // everything after "=_" is password
    password = line.substr(line.find("=", 0));
    password = password.substr(2);
  }
  else
  {
    cerr << "Invalid auth file format (password).\n";
    exit(-1);
  }

  // check if the file doesn't contain anything else
  getline(input, line);
  if (!line.empty())
  {
    // line ended without "\n"
    if (!((line.find("password")) != string::npos))
    {
      cerr << "Invalid auth file format.\n";
      exit(-1);
    }
  }

  input.close();

  return;
}


/**
 * Parses program arguments.
 * @param argc int argument count
 * @param argv int argument vector
 */
Arguments::Arguments(int argc, char **argv)
{
	int opt; // argument option
	reset = false; // reset flag
  clear_pass = false; // clear_pass flag

	if (argc == 1) 
	{
		cerr << "Invalid arguments.\n";
		exit(-1);
	}

	// if the first argument is '-h' or '--help'
	if (argc == 2 && (strcmp(argv[1], "-h") == 0 || 
    strcmp(argv[1], "--help") == 0))
	{
		PrintHelp();
		exit(0);
	}

  // '-r' standalone parameter, resets server
  if (argc == 2 && (strcmp(argv[1], "-r") == 0))
  {
    reset = true;
    return;
  } 

  // flags to prevent multiple occurrence of arguments
  bool aflag, cflag, pflag, dflag, rflag;
  aflag = cflag = pflag = dflag = rflag = false;
	
	// [-h] [-a PATH] [-c] [-p PORT] [-d PATH] [-r]
	while ((opt = getopt(argc, argv, "ha:cp:d:r")) != -1)
    switch(opt)
    {
      case 'h':
      	cerr << "Invalid use of -h option.\n";
      	exit(-1);
      case 'a':
      	if (!aflag)
      	{
      		auth_file_path = optarg;
          // check file validity
          ifstream file(auth_file_path.c_str());
          if(!file.good())
          {
            cerr << "Auth File does not exist.\n";
            exit(-1);
          }
          ParseAuthFile();
      		aflag = true;
      	}
      	else
      	{
      		fprintf(stderr, "Repeated option -%c.\n", opt);
      		exit(-1);
      	}
        break;
      case 'c':
	      if (!cflag)
	      {
	      	clear_pass = true;
	      	cflag = true;
	      }
	      else
      	{
      		fprintf(stderr, "Repeated option -%c.\n", opt);
      		exit(-1);
      	}
        break;
      case 'p':
      	if (!pflag)
	      {
	        port = atoi(optarg);
	      	pflag = true;
	      }
	      else
      	{
      		fprintf(stderr, "Repeated option -%c.\n", opt);
      		exit(-1);
      	}
        break;
      case 'd':
      	if (!dflag)
	      {
	        maildir_path = optarg;
          // check directory validity
          struct stat buf;
          if (!(stat(maildir_path.c_str(), &buf) == 0 && S_ISDIR(buf.st_mode)))
          {
            cerr << "Maildir does not exist.\n";
            exit(-1);
          }
	      	dflag = true;
	      }
	      else
      	{
      		fprintf(stderr, "Repeated option -%c.\n", opt);
      		exit(-1);
      	}
        break;
      case 'r':
      	if (!rflag)
	      {
      		reset = true;
	      	rflag = true;
	      }
	      else
      	{
      		fprintf(stderr, "Repeated option -%c.\n", opt);
      		exit(-1);
      	}
        break;  
      case '?':
        if (optopt == 'a' || optopt == 'p' || optopt == 'd')
          cerr << "Option -" << optopt << " requires an argument.\n";
        else if (isprint (optopt))
          fprintf(stderr, "Unknown option `-%c'.\n", optopt);
        else
          fprintf(stderr, "Unknown option character `\\x%x'.\n", optopt);
        exit(-1);
      default:
      	exit(-1);
    }

  for (int i = optind; i < argc; i++)
  {
    fprintf(stderr, "Non-option argument %s\n", argv[i]);
    exit(-1);
  }

  // check port validity
  if (port > 65535 || port < 1)
  {
  	cerr << "Port number has to be within a range <1, 65535>.\n";
  	exit(-1);
  }

  // some of the mandatory arguments missing
  if (!(aflag && dflag && pflag))
  {
    cerr << "Invalid arguments.\n";
    exit(-1);
  }
}

#endif