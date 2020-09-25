/*
 * project: 	IPK project 1 - webclient
 * author: 		Jan Ondruch, xondru14
 */


#include "webclient.h"


int main(int argc, char *argv[]) {
	
	string url;

	if (argc == 2)
		url = argv[1];
	else
		return printErr(E_WRONG_PARAMS);

	return client(url);
}


/*
 * Client - the whole program logic
 */
int client(string url) {

	if (redirects > 5) printErr(E_REDIRS_LIMIT);

	// PARSE URL
	string parsedFilePath;	// filepath of the input url
	string parsedHost;		  // hostname of the input url

	unsigned int portNumber = 80;

	url.erase(0,7);

	string::iterator si;
	bool flg_port = false;

	// get host
  for (si = url.begin(); si < url.end(); si++) {

  	if (*si == '/') break;
  	if (*si == ':') {
  		flg_port = true;
  		break;
  	}

	parsedHost += (*si);

  }

  // get port
  if (flg_port) {

  	string tempNum;	
  	while (isdigit(*(++si)))
  		tempNum += (*si);

  	portNumber = atoi(tempNum.c_str());

  }

  // get filepath
  while (*si++)
  	parsedFilePath += (*si);

  // CREATE SOCKET
	int sockfd;
  int err_code;
  if (err_code = create_socket(&sockfd, parsedHost, portNumber)) 
    return err_code;

  // SEND HEAD REQUEST
    	
  // create message - ask for HEAD data
  string head_get = "HEAD";
  string http_ver = "HTTP/1.1";
  string msg = create_msg(head_get, parsedFilePath, http_ver, parsedHost);

	// send message
	if (send_msg(sockfd, msg)) return printErr(E_SEND_REQUST);
	
  // READ RESPONSE & resolve code
  string resp = read_resp(sockfd);
  string frstLine = resp.substr(0, resp.find('\n', 0)); // resolve code

  // SOCKET CLOSING
  if (close(sockfd) < 0)
    return printErr(E_CLOSE_SOCKET);


  // if 400 is returned -> send GET request using http 1.0
  if (frstLine.find("400") != string::npos)
    http_ver = "HTTP/1.0";
  if (frstLine.find("200") != string::npos) {

    // CREATE NEW SOCKET
    if (err_code = create_socket(&sockfd, parsedHost, portNumber)) 
      return err_code;

    // SEND GET REQUEST
    head_get = "GET";
    msg = create_msg(head_get, parsedFilePath, http_ver, parsedHost);

    // send message && read response & resolve code
    if (send_msg(sockfd, msg)) 
      return printErr(E_SEND_REQUST);

    resp = read_resp(sockfd);

    // check chunked data
    bool chunk;
    if (resp.find("Transfer-Encoding: chunked") != string::npos) 
      chunk = true;
    else
      chunk = false;

    // get content
    size_t var_size = resp.find("\r\n\r\n");
    if (var_size != string::npos)
      resp.erase(0, var_size + 4);

    // chunked content parsing
    if (chunk)
      resp = chunkedDecode(resp);

    // redirection filename correction
    if (!redir_filename.empty())
      parsedFilePath = redir_filename;
    // get filename && write page content to a file
    string downloadFile;    // file to be downloaded
    downloadFile = getFileName(parsedFilePath);
    writeContent(downloadFile, resp);

    // SOCKET CLOSING
    if (close(sockfd) < 0)
      return printErr(E_CLOSE_SOCKET);

  }
 	else if (frstLine.find("401") != string::npos)	return printErr(E_401);
	else if (frstLine.find("403") != string::npos)	return printErr(E_403);
	else if (frstLine.find("404") != string::npos)	return printErr(E_404);
  else if (frstLine.find("500") != string::npos)	return printErr(E_500);
  else if (frstLine.find("503") != string::npos)	return printErr(E_503);
  else if (frstLine.find("505") != string::npos)	return printErr(E_505);
  else if (frstLine.find("30") != string::npos) {

 	  resp.find("\nLocation") + 11;	// integer of char position
 	  string locRedir = (resp.substr(resp.find("\nLocation") + 11))
 	        			  .substr(0, (resp.substr(resp.find("\nLocation") + 11))
 	        			  .find('\n', 0));

    // save filename for the sake of the right filename in further redirects
    redir_filename = getFileName(parsedFilePath);

 	  redirects++;
 	  return client(locRedir);

 	}
  else {
    return printErr(E_OTHER);
  }

	return EXIT_SUCCESS;

}

  
/*
 * Creates and connects socket
 */
int create_socket(int *sock, string parsedHost, unsigned int portNumber) {

  // CREATE SOCKET 
  int sockfd; // socket file descriptor

  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    return printErr(E_OPENING_SOCKET);


  // GETHOSTNAME
  struct sockaddr_in serv_addr;
  struct hostent *server;

  // nulling struct
  bzero((char *) &serv_addr, sizeof(serv_addr));  
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(portNumber);

  if ((server = gethostbyname(parsedHost.c_str())) == NULL)
    return printErr(E_NO_HOST);

  memcpy(&serv_addr.sin_addr, server->h_addr_list[0], server->h_length);


  // CONNECT
  if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) != 0)
    return printErr(E_CONNECT_SOCKET);

  *sock = sockfd;

  return EXIT_SUCCESS;

}


/*
 * Creates message for sending
 */
string create_msg(string head_get, string parsedFilePath, string http_ver, string parsedHost) {

  string msg;

  // replace all possible spaces in the filename path
  while (parsedFilePath.find(' ') != string::npos) 
    parsedFilePath.replace(parsedFilePath.find(' '), 1, "%20");

  msg.append(head_get.c_str());
  msg.append(" /");
  msg.append(parsedFilePath.c_str());
  msg.append(" ");
  msg.append(http_ver.c_str());
  msg.append("\r\nHost: ");
  msg.append(parsedHost.c_str());
  msg.append("\r\nConnection: close\r\n\r\n");

  return msg;

}


/*
 * Sends message to a server
*/
int send_msg(int sockfd, string msg) {

  if (write(sockfd, msg.c_str(), msg.length()) < 0)
    return E_SEND_REQUST;

  return EXIT_SUCCESS;

}


/*
 * Reads response from the server
 */
string read_resp(int sockfd) {

  string resp;
  char buf;

  while ((read(sockfd, &buf, 1)) > 0)
    resp += buf;

  return resp;

}


/*
 * Prints out error code
 */
int printErr(int code) {

	cerr << "Error: " << errMsg[code] << ".\n";
	return -1;

}


/*
 * Resolves name of the file to be page's content to
*/
string getFileName(string parsedFilePath) {

  reverse(parsedFilePath.begin(), parsedFilePath.end());
  parsedFilePath = parsedFilePath.substr(0, parsedFilePath.find("/"));  
  reverse(parsedFilePath.begin(), parsedFilePath.end());

  if (strlen(parsedFilePath.c_str()) == 0)
     parsedFilePath = "index.html";

  return parsedFilePath; 

}

/*
 * Writes content of the page to a file
*/
void writeContent(string downloadFile, string resp) {

  ofstream file;
  file.open(downloadFile.c_str(), ios::binary);
  file << resp;
  file.close();
      
}

/*
 * Chunked data encoding
 * Function is initialized only when chunked data was sent
*/
string chunkedDecode(string resp) {

  // <head><CRLF>
  // <CRLF>
  // {<size of chunk in hexa><CRLF>
  // <data><CRLF>}*
  // 0<CRLF> last chunk

  int num = 1;  // anything but 0
  size_t var_size;

  string hex = ""; 
  string parsed_resp = "";

  // cycle as until it hits 0<CRLF> -> break
  while (1) {

    var_size = resp.find("\r\n");

    // {<size of chunk in hexa><CRLF><data><CRLF>}*
    if (var_size != string::npos) {

      hex = resp.substr(0, var_size); // size of chunk in hexa
      hex.append("\0");
      num = strtol(hex.c_str(), NULL, 16);  // convert value

      parsed_resp.append(resp.erase(0, var_size + 2), 0, num); // delete crlf && copy data
      resp.erase(0, num + 2); // delete crlf

    }

    // possible last chunk
    if (num == 0) 
      break;

  }

  return parsed_resp;

}