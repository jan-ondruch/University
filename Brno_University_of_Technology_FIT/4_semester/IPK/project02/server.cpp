/*
 * project: 	IPK project 2 - file transfer
 * program:		server
 * file:		server.cpp
 * author: 		Jan Ondruch, xondru14
 */


#include "server.h"


 /*
 * Arguments of the program
 */
class Arguments {

	public:
		unsigned int port;		// portnumber
		void parse_args(int argc, char const *argv[]);

};


/*
 * Parses arguments and return an object representing the arguments
 * server -p port	 
 */
void Arguments::parse_args(int argc, char const *argv[]) {

	// help message
	if ((argc == 2) && ((!strcmp(argv[1], "--help") || (!strcmp(argv[1], "-h"))))) {

		printf("Usage:\nSynopsis: server -p port\n");
		exit(0);

	}

	// checking arguments validity
	if ((argc == 3) && (!strcmp(argv[1], "-p"))) {

		char *endPtr;
		port = strtol(argv[2], &endPtr, 10);

	}
	else {
		// wrong arguments
		cerr << "server: Invalid arguments.\n";
		exit(-1);
	}

}


/*
 * Client - Sever interface
 * Messages and data sent between those 2
 */
class CSInterface {

	public:
		bool upload;		// false - download, true - upload
							// donwload set implicitly
		string filesize;	// upload filesize 
		string filename;	// donwload filename
		string resp;		// response to the client

};


/*
 * Sever content - connection and handling clinets
 */
class Server {

	public:
		int sockfd;					// socket from client
		int newsockfd;				// server socket for communication
		struct sockaddr_in serv;	// server for connection
		socklen_t sinlen;
		pid_t pid;					// forking resolver

		void connectSocket(Arguments Args);
		void resolveClientRequest();
		void resolveClientResp(string msg, CSInterface *CSData);
		void executeClientRequest(CSInterface *CSData);
		string readClientResp();
		void sendMsg(string msg);

};


/*
 * Creates a new socket and connects it
 */
void Server::connectSocket(Arguments Args) {

	int sock;

	// create socket
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		cerr << "server: Error creating socket.\n";
    	exit(-1);
    }

   	// set stuff 
  	struct sockaddr_in serv_addr;
  	serv_addr.sin_family = AF_INET;
  	serv_addr.sin_port = htons(Args.port);
  	serv_addr.sin_addr.s_addr = INADDR_ANY;

  	// bind the socket
  	if (bind(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) != 0) {
  		cerr << "server: Error binding socket.\n";
    	exit(-1);
  	}

  	// listen to clients
  	// go to sleep mode and wait for the incoming connection
  	if (listen(sock, 5)) {
  		cerr << "server: Error listening to clients.\n";
  	}

  	// save the data to object attributes
    sockfd = sock;
    serv = serv_addr;
    sinlen = sizeof(serv);

}


/*
 * Reads request from the client
 */
string Server::readClientResp() {

	string resp;
	int n;
	char buf[BUFSIZE];
	size_t index;

	while ((n = read(newsockfd, buf, BUFSIZE)) > 0) {
		resp.append(buf, n);
		if ((index = resp.find("\r\n\r\n")) != string::npos) {
			resp.erase(index);
			break;
		}
	}

	//printf("SERVER_READ:\n%s\n", resp.c_str());
	return resp;	

}


/*
 * Sends message to the client
*/
void Server::sendMsg(string msg) {

	//printf("SERVER_SEND:\n%s\n", msg.c_str());

	if (write(newsockfd, msg.c_str(), msg.length()) < 0) {
		cerr << "server: Error sending data to a client.\n";
	}

}


/*
 * Resolves request content - upload or download and sends appropriate response
 */
void Server::resolveClientResp(string msg, CSInterface *CSData) {

	string frstLine = msg.substr(0, msg.find('\n', 0));

	if (frstLine.find("d") != string::npos) {
		// download
		CSData->upload = false;

		// get filename
		CSData->filename = msg.substr(msg.find('\n', 1), msg.find("\r\n\r\n"));	
		CSData->filename = CSData->filename.substr(1, CSData->filename.size() - 1);

		// check if file exists and send response to the client
		if (FILE *fd = fopen(CSData->filename.c_str(), "r")) {
			fclose(fd);

			// get filesize
			int filesz;
			ifstream file(CSData->filename.c_str(), ifstream::in | ifstream::binary);

			file.seekg(0, ios::end);
			filesz = file.tellg();
			file.close();

			// convert to filesize string
			stringstream convert;
			convert << filesz;
			string fileszstr = convert.str();

			// write the message
        	CSData->resp = fileszstr + "\r\n\r\n";
    			
    	} else {
        	CSData->resp = "FAIL\r\n\r\n";
    	}   
	}
	else if (frstLine.find("u") != string::npos) {
		// upload

		// get filesize and filename
		CSData->upload = true;
		CSData->filesize = msg.substr(msg.find('\n', 1), msg.find('\n', 2));
		CSData->filename = msg.substr(msg.find('\n', 2), msg.find("\r\n\r\n"));
		// trim the leading '\n' chars
		CSData->filesize = CSData->filesize.substr(1, CSData->filename.size());
		CSData->filename = CSData->filename.substr(1, CSData->filename.size());

		// send acknowledgement
		CSData->resp = "OK\r\n\r\n";
	}
	else {
		CSData->resp = "FAIL\r\n\r\n";
		cerr << "server: Wrong client request sent.\n";
	}

	// send the response
	sendMsg(CSData->resp);

}


/*
 * Deals with the reponse with uploading or sending data
*/
void Server::executeClientRequest(CSInterface *CSData) {

	string clientMsg;
	// download
	// wait for OK message and start sending the file
	if (CSData->upload == false) {

		clientMsg = readClientResp();
		if (clientMsg.find("OK") != string::npos) {
			// we sent the whole file
			ifstream fin(CSData->filename.c_str(), ios::binary);
			stringstream buffer;

			buffer << fin.rdbuf();
			string content = buffer.str();
			content = content + "\r\n\r\n";

			// sent the whole file data
			sendMsg(content);

    		// now we only wait for the final "OK" acknowledgement
    		clientMsg = readClientResp();
    		if (clientMsg.find("OK") != string::npos) {
    			return;
    		}
    		else {
    			cerr << "server: Error sending file data to client, client didn't receive.\n";
    		}
		}
		else {
			// something went wrong
			cerr << "server: Something went wrong preparing sending data.\n";
		}

	}
	// upload
	else {

		clientMsg = readClientResp();
	
		// get propper filename
		if (CSData->filename.find("\\/") != string::npos) {
			CSData->filename = CSData->filename.substr(CSData->filename.find_last_of("\\/"));
			CSData->filename = CSData->filename.substr(1, CSData->filename.size());
		}

		// write content to a file
		ofstream file;
  		file.open(CSData->filename.c_str(), ios::binary);
  		file << clientMsg;
 		file.close();

 		// send OK to the client
 		CSData->resp = "OK\r\n\r\n";
 		sendMsg(CSData->resp);
	}

}


/*
 * Resolves client request for upload || donwload
*/
void Server::resolveClientRequest() {

	string clientMsg;
	CSInterface CSData;

	// read client message
	// resolve if upload or download request and send response to the client
	clientMsg = readClientResp();
	resolveClientResp(clientMsg, &CSData);

	// wait for client response
	// if download - sent the data
	// if upload - read the reponse and save it to a file
	executeClientRequest(&CSData);

}


/*
 * Handles signals and prints out an error mesasge
*/
void signalHandler(int signum) {

    cout << "server: Interrupt signal (" << signum << ") received.\n";
   	exit(signum);  

}


int main(int argc, char const *argv[]) {

	// cleaning up server mess
	signal(SIGTERM, signalHandler);
	signal(SIGINT, signalHandler);
	signal(SIGCHLD, SIG_IGN);

	// parse arguments
	Arguments Args;
	Args.parse_args(argc, argv);

	// estabilish server connection
	Server Serv;
	Serv.connectSocket(Args);

	// start accepting clinet requests and handling them
	// forking process needed
	while(1) {

		// accept new client connection
		if ((Serv.newsockfd = accept(Serv.sockfd, (struct sockaddr *) &Serv.serv, 
			&Serv.sinlen)) < 0) {
			cerr << "server: Error accepting a client.\n";
		}

		// start forking - handling multiple clients accepted
		Serv.pid = fork();
		if (Serv.pid == 0) {	// child process

			// let's deal with the client requests
			Serv.resolveClientRequest();

			// close both server and client connections
			// close the server connection
			if (close(Serv.newsockfd) < 0) {
				cerr << "server: Error closing server connection.\n";
    			exit(-1);
			}
			// close the client connetion
			if (close(Serv.sockfd) < 0) {
				cerr << "server: Error closing client connection.\n";
			}

			exit(0);
		}
		else if (Serv.pid > 0) { // parent process
			// close the server connection
			if (close(Serv.newsockfd) < 0) {
				cerr << "server: Error closing server connection.\n";
    			exit(-1);
			}
		}
		else {	// fork failed
			// kill all the kids
			cerr << "server: Error fork() failed.\n";
    		exit(-1);
		}

	}

	// close socket
	if (close(Serv.sockfd) < 0) {
		cerr << "server: Error closing client connection.\n";
	}

	return 0;

}