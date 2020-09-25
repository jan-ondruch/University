/*
 * project: 	IPK project 2 - file transfer
 * program:		client
 * file:		client.cpp
 * author: 		Jan Ondruch, xondru14
 */


#include "client.h"


/*
 * Arguments of the program
 */
class Arguments {

	public:
		string hostname;		// hostname
		unsigned int port;		// portnumber
		bool upload;			// upload = true, download = false
		string filename;		// filename of the file to downlaod

		void parseArgs(int argc, char const *argv[]);

};


/*
 * Parses arguments and return an object representing the arguments
 * client ‐h hostname -­p port [-­d|u] file_name	 
 */
void Arguments::parseArgs(int argc, char const *argv[]) {

	// help message
	if ((argc == 2) && ((!strcmp(argv[1], "--help") || (!strcmp(argv[1], "-h"))))) {

		printf("Usage:\nSynopsis: clinet -h hostname -p port [-d|u] file_name\n");
		exit(0);
		
	}

	// checking arguments validity
	if ((argc == 7) && (!strcmp(argv[1], "-h")) && (!strcmp(argv[3], "-p")) && 
		((!strcmp(argv[5], "-d")) || (!strcmp(argv[5], "-u")))) {

		char *endPtr;

		hostname = argv[2];
		port = strtol(argv[4], &endPtr, 10);

		if (!strcmp(argv[5], "-u"))
			upload = true;

		filename = argv[6];

	}
	else {
		// wrong arguments
		cerr << "client: Invalid arguments.\n";
		exit(-1);
	}

}


/*
 * Client connecting to a server
 */
class Cli {

	public:
		int sockfd;	// socket for connection

		void connectSocket(Arguments Args);
		void resolveRequest(Arguments Args);
		string readServerResp();
		void sendMsg(string msg);
		void closeSocket();

};


/*
 * Closes socket
 */
void Cli::closeSocket() {

	if (close(sockfd) < 0) {
		cerr << "client: Error closing socket.\n";
		exit(-1);
	}

}


/*
 * Creates a new socket and connects it
 */
void Cli::connectSocket(Arguments Args) {

	int sock;

	// create socket
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		cerr << "client: Error creating socket.\n";
    	exit(-1);
    }

   	// get hostname
  	struct sockaddr_in serv_addr;
  	struct hostent *server;

  	bzero((char *) &serv_addr, sizeof(serv_addr));  
  	serv_addr.sin_family = AF_INET;
  	serv_addr.sin_port = htons(Args.port);

  	if ((server = gethostbyname(Args.hostname.c_str())) == NULL) {
		cerr << "client: Error getting hostname.\n";
    	exit(-1);
    }
    	
    memcpy(&serv_addr.sin_addr, server->h_addr_list[0], server->h_length);

    // connect
    if (connect(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) != 0) {
		cerr << "client: Error connecting socket.\n";
    	exit(-1);
    }

    // save socket to object 'sockfd' attribute
    sockfd = sock;

}


/*
 * Sends message to the client
*/
void Cli::sendMsg(string msg) {

	//printf("CLIENT_SEND:\n%s\n", msg.c_str());

	if (write(sockfd, msg.data(), msg.size()) < 0) {
		closeSocket();
		exit(-1);
	}

}


/*
 * Reads message from the server
 */
string Cli::readServerResp() {

	string resp;
	int n;
	char buf[512];
	size_t index;

	while ((n = read(sockfd, buf, 512)) > 0) {
		resp.append(buf, n);
		if ((index = resp.find("\r\n\r\n")) != string::npos) {
			resp.erase(index);
			break;
		}
	}

	//printf("CLIENT_READ:\n%s\n", resp.c_str());
	return resp;

}


/*
 * Talk to the server and resolve what needs to be resolved
 */
void Cli::resolveRequest(Arguments Args) {

	int filesize;
	string resp;
	string msg;

	// upload
	if (Args.upload == true) {

		/* 1.send file upload request with filesize of the file & filename */

		// get filesize
		ifstream file(Args.filename.c_str(), ifstream::in | ifstream::binary);
		if(!file.is_open()) {
		    cerr << "client: File does not exist.\n";
		    closeSocket();
			exit(-1);
		}

		file.seekg(0, ios::end);
		filesize = file.tellg();
		file.close();

		// convert to filesize string
		stringstream convert;
		convert << filesize;
		string fileszstr = convert.str();

		// upload + filesize + filename + \r\n\r\n
        msg = "u\n" + fileszstr + "\n" + Args.filename + "\r\n\r\n";
		sendMsg(msg);


		/* 2.read OK that server acknowledged and start sending the file */	
		resp = readServerResp();
		if (!(resp.find("OK") != string::npos)) {
			cerr << "client: Error while uploading data.\n";
			closeSocket();
			exit(-1);
		}

		// we sent the whole file
		ifstream fin(Args.filename.c_str(), ios::binary);
		stringstream buffer;

		buffer << fin.rdbuf();
		string content = buffer.str();
		content = content + "\r\n\r\n";

  		if (write(sockfd, content.data(), content.size()) < 0) {
  			cerr << "client: Error sending file data to server.\n";
  			closeSocket();
        	exit(-1);
    	}

    	/* 3.read OK that server acknowledged and in the roman style - kills itself */
    	resp = readServerResp();
		if (!(resp.find("OK") != string::npos)) {
			cerr << "client: Error while uploading data from client.\n";
			closeSocket();
			exit(-1);
		}

		return;

	}
	// download
	else {

		/* 1. send request for file download */
		msg = "d\n" + Args.filename + "\r\n\r\n";
		sendMsg(msg);

		/* 2. receive server response - OK if found, save filesize, FAIL - file not found */
		resp = readServerResp();
		if (resp.find("FAIL") != string::npos) {
			cerr << "client: Requested file does not exist.\n";
			closeSocket();
			exit(-1);
		}
		else {
			// in our protocol, we don't use "OK" - filesize comes
			string fileszstr = msg.substr(0, msg.find("\r\n\r\n"));
		}

		// send "OK"
		msg = "OK\r\n\r\n";
		sendMsg(msg);

		/* 3. read the whole file from server - donwload it, until reaches filesize */

		// should read it until hits the filesize ...
		resp = readServerResp();

		// get propper filename
		if (Args.filename.find("/") != string::npos) {
			Args.filename = Args.filename.substr(Args.filename.find_last_of("\\/"));
			Args.filename = Args.filename.substr(1, Args.filename.size());
		}

		ofstream file;
  		file.open(Args.filename.c_str(), ios::binary);
  		file << resp;
 		file.close();

 		// send OK to the server
 		msg = "OK\r\n\r\n";
 		sendMsg(msg);

 		return;

	}


}


int main(int argc, char const *argv[]) {

	// parse arguments
	Arguments Args;
	Args.parseArgs(argc, argv);

	// create clinet instance and connect it to a server
	Cli Client;
	Client.connectSocket(Args);

	// let the gummy bear talk to the server
	Client.resolveRequest(Args);

	// close socket
	Client.closeSocket();

	return 0;

}