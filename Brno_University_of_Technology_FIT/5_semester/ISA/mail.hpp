/*
 * project: 	ISA project - POP3 server
 * author: 		Jan Ondruch, xondru14
 * file:			mail.hpp
 */

#ifndef _MAIL_HPP
#define _MAIL_HPP

#include <ctime>
#include <limits.h>
#include <string>
#include <algorithm>
#include <cstring>
#include <string>
#include <sstream>
#include <vector>
#include <numeric>
#include <iterator>
#include <fstream>
#include <glob.h>
#include <string.h>
#include <unistd.h>
#include <mutex>
#include <sys/file.h>
#include <streambuf>
#include <signal.h>
#include <limits.h>

#include "md5.hpp"
#include "main.hpp"
#include "string.hpp"

using namespace std;

extern mutex mtx;

// struct for thread attributes
struct arg_struct
{
	int newsockfd;
  bool clear_pass;
  string time_stamp;
  string prev_tok;
  string username;
  string password;
  string maildir;
  bool valid_username;
};

// struct for case-insensitive comparison
struct iequal
{
  bool operator()(int c1, int c2) const
  {
    return toupper(c1) == toupper(c2);
  }
};

bool iequals(const string& str1, const string& str2);
string encrypt_password(arg_struct *args);
string generate_uid(string filename);
ifstream::pos_type convert_file(const char* filename);
bool copy_file(const char *source, const char* destination);
int nth_substr(int n, const string& s, const string& p);
string get_log_item(int row, int col);
int total_messages_size();
int total_messages_count();
string list_messages();
string uidl_messages();
bool delete_message(int row);
void rset_messages();
string retr_message(int row);
string update_messages();
string get_popser_path();
string get_popser_log();
string get_popser_log_dupl();
int move_new_to_cur(arg_struct *args);
int communicate(arg_struct *args, string clnt_msg);
string get_time_stamp();
void send_resp(string resp, int s);
void *client_thread(void *arguments);

#endif