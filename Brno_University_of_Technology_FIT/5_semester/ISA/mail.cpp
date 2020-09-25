/*
 * project: 	ISA project - POP3 server
 * author: 		Jan Ondruch, xondru14
 * file:			mail.cpp
 */

#ifndef _MAIL_CPP
#define _MAIL_CPP

#include "mail.hpp"

mutex mtx;


/**
 * Compares 2 case-insensitive strings.
 * @param str1 string
 * @param str1 string
 * @return  bool equality of the 2 strings
 */
bool iequals(const string& str1, const string& str2)
{
	if (str1.length() == str2.length()) 
  	return equal(str1.begin(), str1.end(), str2.begin(), iequal());
  else
  	return false;
}


/**
 * Encrypts password using md5 hash.
 * @param args arg_struct arguments containing the password
 * @return  string hash
 */
string encrypt_password(arg_struct *args)
{
	string hash_feed;
	hash_feed.append(args->time_stamp);
	hash_feed.append(args->password);

	return md5(hash_feed);
}


/**
 * Generates the unique uid using md5 hash.
 * @param filename string to be hashed
 * @return  string hash
 */
string generate_uid(string filename)
{
	string hash_feed;
	hash_feed = filename;

	return md5(hash_feed);
}


/**
 * Resolves filesize.
 * Converts '\n' to "\r\n".
 * @param filename char filename path
 * @return 	size of the converted file in bytes
 */
ifstream::pos_type convert_file(const char* filename)
{
	// open the mail
	ifstream input(filename);

	// bytes to be added counter to equalize the size on different 
	// always add them since we are working on linux machine (Merlin)
	int add_bytes = 0;

	// recount the filesize
	string line;
	for ( ; getline(input, line); )
	{
		// if \r is found, don't add extra bytes
		if ((line.find("\r")) != string::npos)
			continue;
		else
			add_bytes++;
	}

	if (!line.empty())
		add_bytes++; 

	input.close();

	// get the filesize
	ifstream in(filename, ifstream::ate | ifstream::binary);

	return ((int) in.tellg() + add_bytes);
}


/**
 * Copies file from one folder to another.
 * @param source char source file
 * @param destination char destination file
 * @return  success of the operation
 */
bool copy_file(const char *source, const char* destination)
{
  ifstream src(source, ios::binary);
  ofstream dest(destination, ios::binary);
  
  dest << src.rdbuf();

  return src && dest;
}


/**
 * Find nth substring in a given string.
 * @param n int nth occurence
 * @param s string string start position
 * @param p string string end position
 * @return  int index position of the substring
 */
int nth_substr(int n, const string& s, const string& p) {
  string::size_type i = s.find(p);     // Find the first occurrence

  int j;
  for (j = 1; j < n && i != string::npos; ++j)
     i = s.find(p, i+1); // Find the next occurrence

  if (j == n)
    return(i);
  else
    return(-1);
}


/**
 * Get the item on the given row and column of popser.log file.
 * Deleted items are excluded.
 * @param row int in the table - popser.log file
 * @param col int in the table - popser.log file
 * @return  string on the given position or deleted or empty (not found)
 */
string get_log_item(int row, int col)
{
	ifstream input(get_popser_log().c_str());
	
	int rc = 1;	// row counter
	string item = "";	// searched item
	string dele = ""; // deleted or not
	for (string line; getline(input, line); )
	{
		// get the item
		int i = nth_substr(col, line, "\t");
		i++;
		item = line.substr(i);

		// get the dele column and check if it's delete or not
		int j = nth_substr(3, line, "\t");
		j++;
		dele = line.substr(j);

		// the item was found and check if it was marked as deleted 
		if (rc == row)
		{
			// wasn't deleted
			if (dele == "N")
				return item.substr(0, item.find("\t"));
			else
				return "deleted";
		}
		rc++;
	}

	return "empty";
}


/**
 * Gets total messages size, excluding those marked as deleted ('Y').
 * @return  int total size of the messages
 */
int total_messages_size()
{
	string::size_type sz;
	string length, deleted;
	int size = 0;

	int i = 1;
	while (1)
	{
		length = get_log_item(i, 2);
		if (length == "empty")	// we got the the end of the file and didn't find the item
			break;
		else if (length != "deleted")	// wasn't marked as deleted, we include it
			size+=stoi(length, &sz);
		else {}	// marked as deleted, we don't include it
		i++;
	}

	return size;
}


/**
 * Gets total messages count, excluding those marked as deleted ('Y').
 * @return  int total messages count
 */
int total_messages_count()
{
	int i = 1;	// row counter, also for deleted messages
	int j = 1;	// row counter only for non-deleted messages
	while (1)
	{
		// if we got to the end of the list, end the counting
		// if the item was deleted, don't count it in
		if (get_log_item(i, 1) == "empty")
			break;
		else if (get_log_item(i, 1) != "deleted")
			j++;
		
		i++;
	}

	return j-1;
}


/**
 * Gets list of messages, excluding those marked as deleted ('Y').
 * @return  string list of messages
 */
string list_messages()
{
	string length, deleted, list;

	list = "";
	int i = 1;	// row counter
	while (1)
	{
		length = get_log_item(i, 2);
		if (length == "empty")
			break;
		else if (length != "deleted")
		{
			list.append(to_string(i));
			list.append(" ");
			list.append(length);
			list.append("\r\n");
		}
		else {}

		i++;
	}

	return list.append(".\r\n");
}


/**
 * Gets list of uidl messages, excluding those marked as deleted ('Y').
 * @return  string list of uidl messages
 */
string uidl_messages()
{
	string uid, deleted, list;

	list = "";
	int i = 1;	// row counter
	while (1)
	{
		uid = get_log_item(i, 1);
		if (uid == "empty")
			break;
		else if (uid != "deleted")
		{
			list.append(to_string(i));
			list.append(" ");
			list.append(uid);
			list.append("\r\n");
		}
		else {}

		i++;
	}
	
	return list.append(".\r\n");
}


/**
 * Changes flag of a message to be deleted.
 * @return  bool message was already deleted
 */
bool delete_message(int row)
{
	bool already_deleted = false;
	ofstream logfile;

	// duplicate the popser.log file and clear up content of the popser.log file
	copy_file(get_popser_log().c_str(), get_popser_log_dupl().c_str());

	// open the duplicate
	ifstream input(get_popser_log_dupl().c_str());

	// clear the original popser.log file
	logfile.open(get_popser_log().c_str(), std::ofstream::out | std::ofstream::trunc);
	logfile.close();

	// open it up again and write the changed lines to it
	logfile.open(get_popser_log().c_str(), std::ios::app);

	// changes the flag 'N' to 'Y' if deleted, otherwise does nothing and returns false
	int rc = 1;	// row counter
	for (string line; getline(input, line); )
	{
		if (rc == row)
		{
			int i = nth_substr(3, line, "\t");
			i++;
			string dele_old = line.substr(i);
			if (dele_old == "Y")
				already_deleted = true;
			else
				line.replace(i, dele_old.size(), "Y");
		}
		logfile << line << '\n';
		rc++;
	}

	logfile.close();
	input.close();

	return already_deleted;
}


/**
 * Resets all DELE flags from 'Y' to 'N'.
 */
void rset_messages()
{
	ofstream logfile;

	// duplicate the popser.log file and clear up content of the popser.log file
	copy_file(get_popser_log().c_str(), get_popser_log_dupl().c_str());

	// open the duplicate
	ifstream input(get_popser_log_dupl().c_str());

	// clear the original popser.log file
	logfile.open(get_popser_log().c_str(), std::ofstream::out | std::ofstream::trunc);
	logfile.close();

	// open it up again and write the changed lines to it
	logfile.open(get_popser_log().c_str(), std::ios::app);

	// changes the flag 'Y' to 'N' if deleted, otherwise does nothing
	for (string line; getline(input, line); )
	{
		int i = nth_substr(3, line, "\t");
		i++;
		string dele_old = line.substr(i);
		if (dele_old == "Y")
			line.replace(i, dele_old.size(), "N");

		logfile << line << '\n';
	}

	logfile.close();
	input.close();

	return;
}


/**
 * Reads the whole message from the given file.
 * @param row int the given file position in the logfile table.
 * @return  string message.
 */
string retr_message(int row)
{
	string path, line, str;
	path = get_log_item(row, 0);

	str = "";
	ifstream input(path.c_str());

	// byte-stuff all "." starting on the first line with ".."
	// replace "\n" with "\r\n"
	while (getline(input, line))
	{
		if (!line.empty() && line.at(0) == '.')
			str.append(".");

		// append changed line to the message
		str.append(line);

		// check if the file already contains "\r\n"
		if ((line.find("\r")) != string::npos) 
			str.append("\n");
		else
			str.append("\r\n");
	}

	input.close();

	return str;
}


/**
 * Removes messages marked as 'Y' from the logfile and deletes them from the cur folder.
 * @return  string of client reponse about the operation result
 */
string update_messages()
{
	ofstream logfile;
	bool success = true;
	bool deleted = false;

	// duplicate the popser.log file and clear up content of the popser.log file
	copy_file(get_popser_log().c_str(), get_popser_log_dupl().c_str());

	// open the duplicate
	ifstream input(get_popser_log_dupl().c_str());

	// clear the original popser.log file
	logfile.open(get_popser_log().c_str(), std::ofstream::out | std::ofstream::trunc);
	logfile.close();

	// open it up again and write the changed lines to it
	logfile.open(get_popser_log().c_str(), std::ios::app);

	// removes the lines marked with 'Y'
	for (string line; getline(input, line); )
	{
		// get the DELE item
		int i = nth_substr(3, line, "\t");
		i++;
		string dele_old = line.substr(i);

		// get the FILENAME item
		string filename_to_remove = line.substr(0, line.find("\t", 0));

		// if the file is marked as 'Y', delete it
		if (dele_old != "Y")
			logfile << line << '\n';
		else
		{
			if (remove(filename_to_remove.c_str()) != 0)	// delete the file
				success = false;
			deleted = true;
		}
	}

	logfile.close();
	input.close();

	if (success && deleted) // success and deleted some message
		return "+OK logging out, messages deleted\r\n";
	else if (success) // success but no messages deleted
		return "+OK logging out\r\n";
	else // there were some problems
		return "-ERR some deleted messages not removed\r\n";
}


/**
 * Gets the program's path.
 * @return 	string path
 */
string get_popser_path()
{
  char buf[PATH_MAX];
  ssize_t lenght = readlink("/proc/self/exe", buf, sizeof(buf)-1);
  
  if (lenght >= 0) // path was retrieved successfully
  {
    buf[lenght] = '\0';
    return string(buf);
  }
  else
  {
  	cerr << "Readlink() failed.\n";
  	return "./popser";
  }
}


/**
 * Gets the popser.log file.
 * @return 	string path
 */
string get_popser_log()
{
	string logfile_path, popser_path;
	
	// get path of popser 
	popser_path = get_popser_path();
	// get the folder, the popser is in
	int idx = popser_path.find_last_of("/");
	popser_path = popser_path.substr(0, idx+1);

	logfile_path = popser_path;
	logfile_path.append("popser.log");

	return logfile_path;
}


/**
 * Gets the popser.dupl.log file.
 * @return 	string path
 */
string get_popser_log_dupl()
{
	string logfile_dupl_path, popser_path;
	
	// get path of popser 
	popser_path = get_popser_path();
	// get the folder, the popser is in
	int idx = popser_path.find_last_of("/");
	popser_path = popser_path.substr(0, idx+1);

	logfile_dupl_path = popser_path;
	logfile_dupl_path.append("popser.dupl.log");

	return logfile_dupl_path;
}


/**
 * Moves messages from the new to the cur folder.
 * Resets the dele flags.
 * @param	args arg_struct thread arguments
 * @return  int of operation success
 */
int move_new_to_cur(arg_struct *args)
{
	string before_path, new_path, cur_path, popser_path, logfile_path;
	char full_path[PATH_MAX];
	ofstream logfile;

	// Maildir original path
	before_path = args->maildir;

	// get an abosulte path of Maildir
	if (realpath(before_path.c_str(), full_path) == NULL)
	{	// problem with accessing Maildir
		send_resp("-ERR cannot access Maildir\r\n", args->newsockfd);
		return -1;
	};

	// get folder paths and check maildir structure
	new_path = full_path;
	new_path.append("/new/");

	cur_path = full_path;
	cur_path.append("/cur/");

	// check maildir structure (new and cur folders)
	struct stat buf;
	if (!(stat(new_path.c_str(), &buf) == 0 && S_ISDIR(buf.st_mode)))
	{
		send_resp("-ERR invalid Maildir structure\r\n", args->newsockfd);
	  return -1;
	}
	if (!(stat(cur_path.c_str(), &buf) == 0 && S_ISDIR(buf.st_mode)))
	{
	  send_resp("-ERR invalid Maildir structure\r\n", args->newsockfd);
	  return -1;
	}

	new_path.append("*");

	// get the logfile path
	// check if poser.log exists, if not, create it
	logfile_path = get_popser_log();
	ifstream file(logfile_path.c_str());
	if(!file.good())
	  ofstream logfile(logfile_path.c_str());
	
	// append new messages to the end of the logfile
	logfile.open(logfile_path.c_str(), std::ios::app);

	// get the list of all the files within the new folder
	glob_t glob_result;
	glob(new_path.c_str(), GLOB_TILDE, NULL, &glob_result);
	for (unsigned int i = 0; i < glob_result.gl_pathc; ++i) 
	{
		// the new folder files
		string cur_path_mod = cur_path;
	  string pathv = glob_result.gl_pathv[i];
	  int k = pathv.find_last_of("/");
	  
	  // get only the filename part
	  string filename = pathv.substr(k+1);
	  cur_path_mod.append(filename);

	  // append unique:info -> unique:2, (standart format) 
	  // automatically it is marked as "Seen"
	  cur_path_mod.append(":2,S");

	  // append filename to the logfile
	  // table row syntax: PATH&MOD_NAME/tUNIQUE_UD/tLENGHT/tDELETED/n
	  logfile << cur_path_mod << '\t';
	  logfile << generate_uid(filename) << '\t';
	  logfile << convert_file(pathv.c_str()) << '\t';
	  logfile << 'N' << '\n';

	  // move files with the modified unique names to the cur directory
	  if ((rename(pathv.c_str(), cur_path_mod.c_str())) != 0) {
	  	send_resp("-ERR moving new messages within Maildir failed\r\n",
	  		args->newsockfd);
	  	return -1;
	  }
	}

	// reset DELE flag to 'N' (if the previous session didn't close properly)
	rset_messages();
	
	// close the file and save the changes
	logfile.close();
	
	return 0;	// success
}


/**
 * Finite automata implementing the whole communication logic with client (protocol).
 * Replies to client's messages.
 * @param	args arg_struct thread attributes
 * @param	clnt_msg string client's message
 * @return  int success of the operation
 */
int communicate(arg_struct *args, string clnt_msg)
{
	// parse clinet message and split it into tokens
	vector<string> act_toks = parse_string(clnt_msg);
	string resp;	// response string

	// finite automata for C-S communication
	if (act_toks.size() < 1)
	{
		send_resp("-ERR invalid command\r\n", args->newsockfd);
	} 
	else if (iequals(args->prev_tok, "AUTH"))
	{ // USER || APOP || QUIT expected
		if (iequals(act_toks[0], "USER"))
		{ // USER name
			if (!(args->clear_pass))
			{ // encrypted communication, USER command is not valid.
				send_resp("-ERR the communication is encrypted, "
					"use the command 'APOP' to log in\r\n", args->newsockfd);
				args->prev_tok = "AUTH";
			}
			else
			{ // USER name
				if (act_toks.size() != 2)
				{ // there is no 'name' token or there are too many tokens
					send_resp("-ERR invalid 'USER' command options\r\n", args->newsockfd);
					args->prev_tok = "AUTH";
				}
				else
				{	// number of tokens is valid

					// don't give a hint to a potential attacker
					send_resp("+OK\r\n", args->newsockfd);
					
					if (act_toks[1] == args->username)
						args->valid_username = true;
					else
						args->valid_username = false;

					args->prev_tok = "USER";
				}
			}
		}
		else if (iequals(act_toks[0], "APOP"))
		{ // APOP name digest
			if (args->clear_pass)
			{ // non-encrypted communication, APOP command is not valid.
				send_resp("-ERR the communication is not encrypted, "
					"use the command 'USER' to log in\r\n", args->newsockfd);
				args->prev_tok = "AUTH";
			}
			else if (act_toks.size() != 3)
			{ // there is no 'pass' token or there are too many tokens
				send_resp("-ERR invalid 'APOP' command options\r\n", args->newsockfd);
				args->prev_tok = "AUTH";
			}
			else
			{ // numer of tokens is valid
				if ((act_toks[1] == args->username) && 
					(act_toks[2] == encrypt_password(args)))
				{	// name and digest correspond			
					if (mtx.try_lock())
					{
						if (move_new_to_cur(args))	// error
							return -1;
						
						resp.append("+OK ");
						resp.append(args->username);
						resp.append("'s maildrop has ");
						resp.append(to_string(total_messages_count()));
						resp.append(" messages (");
						resp.append(to_string(total_messages_size()));
						resp.append(" octets)\r\n");
						send_resp(resp, args->newsockfd);
						
						args->prev_tok = "TRAN";
					}
					else
					{
						send_resp("-ERR maildrop already locked\r\n", args->newsockfd);
						args->prev_tok = "AUTH";
					}
				}
				else
				{ // invalid username of password
					send_resp("-ERR invalid username or password\r\n", args->newsockfd);
					args->prev_tok = "AUTH";
				}
			}
		}
		else if (iequals(act_toks[0], "QUIT"))
		{ // QUIT issued in the AUTH section -> no UPDATE
			// bye bye
			send_resp("+OK logging out\r\n", args->newsockfd);
			return 1;
		}
		else
		{ // INVALID COMMAND
			send_resp("-ERR invalid command\r\n", args->newsockfd);
			args->prev_tok = "AUTH";
		}
	}
	else if (iequals(args->prev_tok, "USER"))
	{ // PASS || QUIT expected
		if (iequals(act_toks[0], "PASS"))
		{
			if (act_toks.size() < 2)
			{ // there is no 'pass' token
				send_resp("-ERR invalid 'PASS' command options\r\n", args->newsockfd);
				args->prev_tok = "AUTH";
			}
			else
			{	// number of tokens is valid
				string pass = "";
				pass = clnt_msg.erase(0, 5);	// remove "PASS_"

				if ((pass == args->password) && args->valid_username)
				{ // valid username and password, TRAN state will come next
					resp = "";

					// try to lock the maildrop, else return to AUTH state
					if (mtx.try_lock())
					{
						if (move_new_to_cur(args))	// error
							return -1;
						
						resp.append("+OK ");
						resp.append(args->username);
						resp.append("'s maildrop has ");
						resp.append(to_string(total_messages_count()));
						resp.append(" messages (");
						resp.append(to_string(total_messages_size()));
						resp.append(" octets)\r\n");
						send_resp(resp, args->newsockfd);

						args->prev_tok = "TRAN";
					}
					else
					{
						send_resp("-ERR maildrop already locked\r\n", args->newsockfd);
						args->prev_tok = "AUTH";
					}
				}
				else
				{ // name is not a valid username, back to AUTH state
					send_resp("-ERR authentication failed\r\n", args->newsockfd);
					args->prev_tok = "AUTH";
				}
			}
		}
		else if (iequals(act_toks[0], "QUIT"))
		{ // QUIT
			// bye bye
			send_resp("+OK logging out\r\n", args->newsockfd);
			return 1;
		}
		else 
		{ // INVALID COMMAND
			send_resp("-ERR invalid command ('PASS' expected)\r\n", args->newsockfd);
			args->prev_tok = "AUTH";
		}
	}
	else if (iequals(args->prev_tok, "TRAN"))
	{ // QUIT || STAT || LIST [msg] || RETR msg || DELE msg || NOOP || RSET || 
		// UIDL [msg] expected
		if (iequals(act_toks[0], "STAT"))
		{ // STAT
			resp = "";
			resp.append("+OK ");
			resp.append(to_string(total_messages_count()));
			resp.append(" ");
			resp.append(to_string(total_messages_size()));
			resp.append("\r\n");
			send_resp(resp , args->newsockfd);

			args->prev_tok = "TRAN";
		}
		else if (iequals(act_toks[0], "NOOP"))
		{ // NOOP
			send_resp("+OK\r\n", args->newsockfd);
			args->prev_tok = "TRAN";
		}
		else if (iequals(act_toks[0], "LIST"))
		{ // LIST
			if (act_toks.size() > 2)
			{ // there are too many tokens
				send_resp("-ERR invalid 'LIST' command options\r\n", args->newsockfd);
				args->prev_tok = "TRAN";
			}
			else
			{ // number of tokens is valid
				if (act_toks.size() == 1)
				{ // LIST
					resp = "";
					resp.append("+OK ");
					if (total_messages_count() != 0)
					{
						resp.append(to_string(total_messages_count()));
						resp.append(" messages (");
						resp.append(to_string(total_messages_size()));
						resp.append(" octets)\r\n");
						send_resp(resp , args->newsockfd);
						// list all the messages
						resp = "";
						resp = list_messages();
						send_resp(resp , args->newsockfd);
					}
					else
					{
						resp.append("0 messages in maildrop\r\n.\r\n");
						send_resp(resp , args->newsockfd);
					}
					args->prev_tok = "TRAN";
				}
				else
				{ // LIST number
					char * pEnd;
					long row = strtol(act_toks[1].c_str(), &pEnd, 10);
					string item = "";

					if (((item = get_log_item(row, 2)) != "empty") && 
						((item = get_log_item(row, 2)) != "deleted")) 
					{
						resp = "";
						resp.append("+OK ");
						resp.append(to_string(row));
						resp.append(" ");
						resp.append(item);
						resp.append("\r\n");
						send_resp(resp , args->newsockfd);
					}
					else 
					{
						send_resp("-ERR no such message in a maildrop\r\n", args->newsockfd);
					}
					args->prev_tok = "TRAN";
				}
			}
		}
		else if (iequals(act_toks[0], "DELE"))
		{ // DELE
			if (act_toks.size() != 2)
			{ // there are too little or too many tokens
				send_resp("-ERR invalid 'DELE' command options\r\n", args->newsockfd);
				args->prev_tok = "TRAN";
			}
			else
			{
				char * pEnd;
				long row = strtol(act_toks[1].c_str(), &pEnd, 10);
				string item = "";
				bool already_deleted = false;
				resp = "";

				// check if the message exists
				if ((item = get_log_item(row, 1)) != "empty") 
				{
					already_deleted = delete_message(row);
					if (already_deleted)
					{
						resp.append("-ERR message ");
						resp.append(to_string(row));
						resp.append(" already deleted\r\n");
						send_resp(resp, args->newsockfd);
					}
					else
					{
						resp.append("+OK message ");
						resp.append(to_string(row));
						resp.append(" deleted\r\n");
						send_resp(resp, args->newsockfd);
					}
				}
				else 
				{
					send_resp("-ERR no such message in a maildrop\r\n", args->newsockfd);
				}
				args->prev_tok = "TRAN";
			}
		}
		else if (iequals(act_toks[0], "RSET"))
		{ // RSET
			resp = "";
			rset_messages();
			resp.append("+OK ");
			resp.append("maildrop has ");
			resp.append(to_string(total_messages_count()));
			resp.append(" messages (");
			resp.append(to_string(total_messages_size()));
			resp.append(" octets)\r\n");
			send_resp(resp, args->newsockfd);
			args->prev_tok = "TRAN";
		}
		else if (iequals(act_toks[0], "RETR"))
		{ // RETR
			if (act_toks.size() != 2)
			{ // there are too little or too many tokens
				send_resp("-ERR invalid 'RETR' command options\r\n", args->newsockfd);
				args->prev_tok = "TRAN";
			}
			else
			{
				char * pEnd;
				long row = strtol(act_toks[1].c_str(), &pEnd, 10);
				string item = "";
				resp = "";

				// check if the message exists
				if (((item = get_log_item(row, 2)) != "empty") && 
						((item = get_log_item(row, 2)) != "deleted")) 
				{
					resp.append("+OK ");
					resp.append(get_log_item(row, 2));
					resp.append(" octets\r\n");
					send_resp(resp, args->newsockfd);

					// retrieve the message
					// send message and "\r\n" separately
					resp = "";
					resp = retr_message(row);
					send_resp(resp, args->newsockfd);
					resp = "";
					resp.append(".\r\n");
					send_resp(resp, args->newsockfd);
				}
				else 
				{
					send_resp("-ERR no such message in a maildrop\r\n", args->newsockfd);
				}
				args->prev_tok = "TRAN";
			}
		}
		else if (iequals(act_toks[0], "UIDL"))
		{ // UIDL
			if (act_toks.size() > 2)
			{ // there are too many tokens
				send_resp("-ERR invalid 'UIDL' command options\r\n", args->newsockfd);
				args->prev_tok = "TRAN";
			}
			else
			{ // number of tokens is valid
				if (act_toks.size() == 1)
				{ // UIDL
					resp = "";
					resp.append("+OK ");
					if (total_messages_count() != 0)
					{
						resp.append(to_string(total_messages_count()));
						resp.append(" messages\r\n");
						send_resp(resp , args->newsockfd);
						// uidl list all the messages
						resp = "";
						resp = uidl_messages();
						send_resp(resp , args->newsockfd);
					}
					else
					{
						resp.append("0 messages in maildrop\r\n.\r\n");
						send_resp(resp , args->newsockfd);
					}
					args->prev_tok = "TRAN";
				}
				else
				{ // UIDL number
					char * pEnd;
					long row = strtol(act_toks[1].c_str(), &pEnd, 10);
					string item = "";

					if (((item = get_log_item(row, 1)) != "empty") &&
						((item = get_log_item(row, 1)) != "deleted")) 
					{
						resp = "";
						resp.append("+OK ");
						resp.append(to_string(row));
						resp.append(" ");
						resp.append(item);
						resp.append("\r\n");
						send_resp(resp , args->newsockfd);
					}
					else 
					{
						send_resp("-ERR no such message in a maildrop\r\n", args->newsockfd);
					}
					args->prev_tok = "TRAN";
				}
			}
		}
		else if (iequals(act_toks[0], "QUIT"))
		{ // QUIT
			// bye bye
			// UPDATE section, delete messages marked as deleted
			// both in the logile and also in the cur folder
			send_resp(update_messages(), args->newsockfd);

			return 1;
		}
		else
		{ // INVALID COMMAND
			send_resp("-ERR invalid command\r\n", args->newsockfd);
			args->prev_tok = "TRAN";
		}
	}

	return 0;	// everything ok
}


/**
 * Get time stamp.
 * @return  string created stamp
 */
string get_time_stamp()
{
	long serv_pid;
	time_t t;
	int result;
	string time_stamp;
	char hostname[HOST_NAME_MAX];

	serv_pid = getpid();
	t = time(0);

	result = gethostname(hostname, HOST_NAME_MAX);
	if (result)
		return "";

	time_stamp.append("<");
	time_stamp.append(to_string(serv_pid));
	time_stamp.append(".");
	time_stamp.append(to_string(t));
	time_stamp.append("@");
	time_stamp.append(hostname);
	time_stamp.append(">");

	return time_stamp;
}


/**
 * Sends response to the client via socket.
 * @param	resp string message to be sent
 * @param	s int communication socket
 */
void send_resp(string resp, int s)
{
	if (write(s, resp.c_str(), resp.length()) < 0)
		cerr << "Error sending data to a client.\r\n";
}


/**
 * Client thread created by server.
 * Each thread create has its own "attributes".
 * Entry point in the server-client communication.
 * @param	arguments void passed context from the server
 */
void *client_thread(void *arguments)
{
	struct serv_info_struct *temp = (struct serv_info_struct *)arguments;
	
	// set up local structure (unique for every thread)
	struct arg_struct *args = new arg_struct();

	args->username = temp->username;
	args->password = temp->password;
	args->clear_pass = temp->clear_pass;
	args->maildir = temp->maildir;
	args->newsockfd = temp->newsockfd;
	args->time_stamp = "";
	args->prev_tok = "AUTH";
	args->valid_username = false;

	string resp;
	char buf[BUFSIZE];
	size_t index;
	int n;

	// get time stamp
	if ((args->time_stamp = get_time_stamp()) == "")
	{
		send_resp("-ERR server cannot establish connection\r\n", args->newsockfd);
		goto end_connection;
	}

	// send greeting
	resp.append("+OK POP3 server ready ");
	resp.append(args->time_stamp);
	resp.append("\r\n");
	send_resp(resp, args->newsockfd);

	memset(buf, 0, sizeof(buf));

	// set timeout on recv function
	struct timeval sock_timeout;
	sock_timeout.tv_sec = 600;
	sock_timeout.tv_usec = 0;
	if (setsockopt(args->newsockfd, SOL_SOCKET, SO_RCVTIMEO, (char*)&sock_timeout, 
										sizeof(sock_timeout)) < 0)
	  cerr << "setsockopt(SO_RCVTIMEO) failed\n";
	if (setsockopt (args->newsockfd, SOL_SOCKET, SO_SNDTIMEO, (char *)&sock_timeout,
	                  sizeof(sock_timeout)) < 0)
	  cerr << "setsockopt(SO_SNDTIMEO) failed\n";
	

	// select start
	fd_set fds, readfds;
	int max_fd, rc;

	FD_ZERO(&fds);
	max_fd = args->newsockfd;
	FD_SET(args->newsockfd, &fds);

	// timeout for new connection, that didn't get to recv yet
	struct timeval timeout;
	timeout.tv_sec = 600;
	timeout.tv_usec = 0;

	// listen to the client and handle communication
	while (1)
	{
		// clear the socket set
		FD_ZERO(&readfds);

		// copy the master fd_set over to the read fd_set
		memcpy(&readfds, &fds, sizeof(fds));

		rc = select(max_fd + 1, &readfds, NULL, NULL, &timeout);
		
		if (rc < 0)	// error on select
		{
		   cerr << "Select() error.\n";
		   break;
		}
		else if(rc == 0)	// timeout
			break;
		
		else 
		{
			if (FD_ISSET(args->newsockfd, &readfds))
			{
				// nulling the string
				resp = "";
				memset(buf, 0, sizeof(buf));

				// receiving data
				while ((n = recv(args->newsockfd, buf, BUFSIZE, 0)) > 0)
				{
					resp.append(buf, n);
					if ((index = resp.find("\r\n")) != string::npos)
						resp.erase(index);
					if (communicate(args, resp)) // 1 or -1: successful QUIT or ERROR
						goto end_connection;

					resp = "";
				}
				
				// error receiving data
				if (n == 0)	// client terminated the connection, e.g. closed telnet
					break;
				else
				{
					// timeout on recv
					if ((errno == EAGAIN) || (errno == EWOULDBLOCK))
		        break;

		      // something went wrong
					cerr << "Error reading clinet data.\n";
					break;
				}
			}
		}
	}

	end_connection:
	
	// unlock maildir and terminate the connection
	mtx.unlock();
	
	FD_CLR(args->newsockfd, &fds);
	close(args->newsockfd);

	return (void *) 0;
}

#endif