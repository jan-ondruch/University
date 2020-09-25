/*
 * project: 	ISA project - POP3 server
 * author: 		Jan Ondruch, xondru14
 * file:			main.cpp
 */

#include "main.hpp"

// to enable closing the server by signal
Server Serv;  // server

/**
 * Resets server after the '-r' option is given.
 * Removes the helper files and moves any mail from the cur to the new folder.
 * @return  success of operation
 */
int reset_server()
{
  bool invalid = false;
  bool invalid_mov = false;

  // check if poser.log exists
  // if yes, move the files to 'new' and rename them, else just remove the files
  ifstream file(get_popser_log().c_str());
  if (file.good())
  {
    string maildir_path, new_path, cur_path, line;

    // get the first entry of the file and extract the path to the cur folder from it
    getline(file, line);
    // remove everything from the line to the Maildir folder
    maildir_path = line.substr(0, line.find("cur/", 0));

    cur_path = maildir_path;
    new_path = maildir_path;
    cur_path.append("cur/*");
    new_path.append("new/");

    // get the list of all the files within the new folder
    glob_t glob_result;
    glob(cur_path.c_str(), GLOB_TILDE, NULL, &glob_result);
    for (unsigned int i = 0; i < glob_result.gl_pathc; ++i) 
    {
      // the cur folder files
      string new_path_mod = new_path;
      string pathv = glob_result.gl_pathv[i];
      int k = pathv.find_last_of("/");
      
      // get only the filename part
      string filename = pathv.substr(k+1);
      new_path_mod.append(filename);

      // remove the ":2,S" part
      new_path_mod = new_path_mod.substr(0, new_path_mod.find(":", 0));

      // move files with the modified unique names to the new directory
      // and remove the converted "\r\n" back to simple "\n"
      if ((rename(pathv.c_str(), new_path_mod.c_str())) != 0)
      {
        if (!invalid_mov)
        {
          cerr << "Error Maildir: moving files from cur to new directory failed.\r\n";
          invalid_mov = true;
        }
      }
    }
    file.close();
  }
  // remove the temporary files
  ifstream file2(get_popser_log().c_str());
  if (file2.good())
  {
    if (!invalid_mov) // don't remove the temporary files if moving of the mails failed
    {
      if (remove(get_popser_log().c_str()) != 0 )
      {
        cerr << "Error removing primary log file.\r\n";
        invalid = true;
      }
      else
        file2.close();
    }
  }

  ifstream file3(get_popser_log_dupl().c_str());
  if (file3.good())
  {
    if (!invalid_mov)
    {
      if (remove(get_popser_log_dupl().c_str()) != 0 )
      {
        cerr << "Error removing secondary log file.\r\n";
        invalid = true;
      }
      file3.close();
    }
  }

  return invalid || invalid_mov;
}


/**
 * SIGINT handler.
 * Ends the program properly.
 * @param int sig signal value
 */
void SIGhandler(int sig)
{
  signal(sig, SIG_IGN);
  close(Serv.sockfd);
  exit(0);
}


/**
 * Main.
 * @param argc argument count
 * @param argv argument vector
 * @return  success of the program
 */
int main(int argc, char *argv[])
{

  // catch SIGINT and close program
  signal(SIGINT, SIGhandler);

  // parse arguments
  Arguments Args(argc, argv);

  // if reset option has been set
  if (Args.reset)
  {
    if (!reset_server())
      return 0;
    else 
      return -1;
  }

  // estabilish server connection and start accepting client requests
	Serv.ConnectSocket(Args);
  if (!Serv.AcceptClients(Args))
  	return 0;
  else
  {
  	cerr << "There was an error encountered.\r\n";
  	return -1;
  }
}