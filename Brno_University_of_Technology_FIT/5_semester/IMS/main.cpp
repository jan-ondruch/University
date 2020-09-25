/*
 * Projekt: Simulace toku dat v systemu pro zpracovani radiovych casomernych signalu s vyuzitim v pozicnim systemu
 * Autor: Jan Ondruch, xondru14
 * Datum: 29.11.2017
 */

#include <getopt.h>
#include <iostream>
#include <string>
#include <simlib.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <vector>
#include <memory>

#include "tdoasystem.h"

using namespace std;


void help()
{
	cout << "Simulace toku dat v systemu pro zpracovani radiovych" << endl;
	cout << "casomernych signalu s vyuzitim v pozicnim systemu." << endl;
	cout << "" << endl;
	cout << "pouziti:" << endl;
	cout << "\t[-h]\t\t\tVypise tuto napovedu" << endl;
	cout << "\t[-t NUM_OF_TAGS]\tPocet pohybovych senzoru" << endl;
	cout << "\t[-a NUM_OF_ANCHORS]\tPocet vysilacu" << endl;
	cout << "\t[-l SIM_LENGTH]\t\tDelka simulace v [min]" << endl;
	cout << "\t[-o OUTPUT]\t\tVystupni soubor se statistikami" << endl;
	cout << "\t[-r TAG_REF_RATE]\tObnovovaci frekvence pohyboveho senzoru [ms]" << endl;
	cout << "\t[-b BATCH_TO_DB]\tPocet udaju hromadne ulozenych do databaze" << endl;
	cout << "\t[-s SEND_OVER_WIFI]\tVyuziti Wi-Fi pro prenos dat (1 ano, 0 ne)" << endl;
	cout << "\t[-w WIFI_PING]\t\tWi-Fi latence" << endl;
	cout << "\t[-w METHOD_TRANSFER]\tZpusob prenosu dat k uzivateli (1 REST API, 2 Websockets, 3 UDP)" << endl;
	cout << "\t[-c CPU_SEIZE_TIME]\tDoba vyhrazena procesu v procesoru [ms]" << endl;
}

// histograms
Histogram tag_to_DB("Prenos paketu od pohyboveho cidla do DB", 0, 10, 20);
Histogram tag_to_sensmap("Prenos paketu od pohyboveho cidla do Sensmap", 0, 20, 20);
Histogram tag_to_client("Prenos dat od pohyboveho cidla k uzivateli", 10, 30, 30);

// transfer methods
enum method {
	REST = 1,
	WS,
	UDP
};

// program setup
int SIM_LENGTH = 100;	// in minutes
double SIM_TIME_END = 0.0;
string OUTPUT = "model.out";

// default values
int NUM_OF_TAGS = 50;
int NUM_OF_ANCHORS = 6;
double TAG_PER = 0.01;
double UDP_ERR = 0.01;
double SEND_DELAY = 0.1;
int TAG_REF_RATE = 500;
int BATCH_TO_DB = 200;
bool SEND_OVER_WIFI = true;
int WIFI_PING = 15;
int METHOD_TRANSFER = REST;
float DB_RECORD_SIZE = 0.000255;	// mb
double CPU_SEIZE_TIME = 10; // ms
bool REAL_TIME = true;

// time
int SECONDS = 60;
int MILISECONDS = 1000;

// methods
int REST_MID = 450;
int REST_DEV = 15;
int WS_MID = 200;
int WS_DEV = 10;
int UDP_MID = 75;
int UDP_DEV = 5;

// CPU
Facility CPU("Procesor");
int iCPU = 0;	// CPU iterations counter

// RTLS SERVER
int computed = 0;

// TDOA
Queue qTDOA("Fronta pozadavku cekajicich na zpracovani RTLS serverem (TDOA)");

// Sensmap
Queue qSensmap("Fronta pozadavku cekajicich na zpracovani Sensmap serverem do DB");
Queue dq("Fronta pozadavku cekajicich na hromadny batch do DB");
int currCapacity = 0;
bool batching = false;
double start_time_to_batch = 0;
double max_time_to_batch = 0;

// capacity 
unsigned int dbRecords = 0;


/*
 * Ctor passing taking the Packet's @ArrivalTime.
 */
Record::Record(double ArrivalTime) : ArrivalTime(ArrivalTime) {}

/*
 * Packet's path from the RTLS server to the client.
 */
void Record::Behavior() {
	if (METHOD_TRANSFER == REST) {
		Wait(Normal(REST_MID, REST_DEV));
	}
	else if (METHOD_TRANSFER == WS) {
		Wait(Normal(WS_MID, WS_DEV));
	}
	else {
		Wait(Normal(UDP_MID, UDP_DEV));
		if (Random() <= UDP_ERR) {
			return;
		}
	}

	tag_to_client(Time - ArrivalTime);
	return;
}

/*
 * The main CPU login.
 * CPU time is distributed to each process.
 */
void Server::Behavior() {
	while (Time < SIM_TIME_END) {
		
		// choose proccss
		tCPU = Random();
		if (tCPU >= 0.50) {	// RTLS server TDoA
			if (!qTDOA.Empty()) {

				double pCPU = CPU_SEIZE_TIME;	// hold CPU for 10ms

				while (!qTDOA.Empty() && pCPU > 0) {
					Packet *tmp = (Packet *) qTDOA.GetFirst();
					
					if (computed < NUM_OF_ANCHORS) {
				 		computed++;
				 		tmp->Cancel();	// simulate aggregation of packets in the TDoA algorithm
				 	}
				 	else {
				 		computed = 0;
						
						double DelayRtlsServ = 0.0093*NUM_OF_ANCHORS*NUM_OF_ANCHORS*NUM_OF_ANCHORS-0.0682*NUM_OF_ANCHORS*NUM_OF_ANCHORS+0.1589*NUM_OF_ANCHORS+0.00000000002;
					 	double DelayCPU = DelayRtlsServ;
				
						Seize(CPU);
					 	Wait(DelayCPU);
						Release(CPU);
					 	
					 	pCPU -= DelayCPU;
						tmp->Activate();
				 	}
				}
				
				if (pCPU > 0) {
					Wait(pCPU);	// hold CPU despite having nothing to process
					pCPU = 0;	// end the While loop
				}
			}
			else Wait(CPU_SEIZE_TIME); // process holds the processor
		}
		else if (tCPU <= 0.10) {	// RTLS manager
			Seize(CPU);
			Wait(CPU_SEIZE_TIME);
			Release(CPU);
		}
		else if (tCPU > 0.10 && tCPU <= 0.20) {	// Sensmap
			Seize(CPU);
			Wait(CPU_SEIZE_TIME);
			Release(CPU);
		}
		else {	// Sensmapserver
			if (!qSensmap.Empty()) {

				double pCPU = CPU_SEIZE_TIME;	// hold CPU for 10ms

				while (!qSensmap.Empty() && pCPU > 0) {
					Packet *tmp = (Packet *) qSensmap.GetFirst();
					
					Seize(CPU);
					Wait(SEND_DELAY);
					(new Record(tmp->ArrivalTime))->Activate();
					Release(CPU);

					pCPU -= SEND_DELAY;

					// buffer for DB: once 200 records have been gathered, save them to DB
					// start time for batching to calculate the max time for batching
					if (currCapacity == 0) {
						start_time_to_batch = Time;
					}
					if (currCapacity < BATCH_TO_DB) {
						currCapacity++;
						dq.Insert(tmp);
						tmp->Passivate();
					}
					else {
						// save the max waiting time for batching
						if (max_time_to_batch < (Time - start_time_to_batch)) {
							max_time_to_batch = Time - start_time_to_batch;
						}

						Seize(CPU);
						Wait(CPU_SEIZE_TIME);
						Release(CPU);

						while (!dq.Empty()) {
							Packet *tmp = (Packet *) dq.GetFirst();
							tmp->Activate();
							dbRecords++;
							tag_to_DB(Time - tmp->ArrivalTime);
							tmp->Cancel();
						}

				 		pCPU = 0;
						currCapacity = 0;
					}
				}

				if (pCPU > 0) {
					Wait(pCPU);	// hold CPU despite having nothing to process
					pCPU = 0;	// end the While loop
				}
			}
			else Wait(CPU_SEIZE_TIME);	// process holds the processor
		}

		iCPU++;	// CPU iterations counter
	}
}

/*
 * Radio waves (packets) transmission from the source tags to the Server.
 */
void Packet::Behavior() {
	ArrivalTime = Time;
	if (Random() <= TAG_PER) {	// PER
		return; // the packet is lost
	}

	// if sent over wi-fi, add more delay
	if (SEND_OVER_WIFI == true) {
		Wait(Normal(WIFI_PING, 1)); // wi-fi transmit + jitter
	}
	else { // over cable
		Wait(Normal(1, 0.1)); // transmitt pres cable + IP switch
	}

	/* RTLS SERVER TDOA */
	qTDOA.Insert(this);
	Passivate();

 	/* SENSMAP SERVER */
	tag_to_sensmap(Time - this->ArrivalTime);
 	qSensmap.Insert(this);
 	Passivate();
}

/*
 * Tag creates new packets and sends them to the network.
 */
void Tag::Behavior() {
	for (int i = 0; i < NUM_OF_ANCHORS; i++) {
		shared_ptr<Packet> pTmp = make_shared<Packet>();
		packets.push_back(pTmp);
		pTmp->Activate();
	}
	Activate(Time + TAG_REF_RATE);	// generating new packet every TAG_REF_RATE ms
}

/*
 * System startup - server and network.
 */
void SystemInitiator::Behavior() {
	for (int i = 0; i < NUM_OF_TAGS; i++) {
		shared_ptr<Tag> tTmp = make_shared<Tag>();
		tags.push_back(tTmp);
		tTmp->Activate();
	}

	s = new Server();
	s->Activate();
}

// Dtor, releases the server
SystemInitiator::~SystemInitiator() {
	delete s;
}


int main(int argc, char **argv)
{
	int opt;

	while((opt = getopt(argc, argv, "ht:a:l:o:r:b:s:w:m:c:")) != -1)
	{
		switch(opt)
		{
			case 'h':
				help();
				return 0;
			case 't': // number of tags
				NUM_OF_TAGS = atoi(optarg);
				break;
			case 'a': // number of anchors
				NUM_OF_ANCHORS = atoi(optarg);
				break;	
			case 'l': // simulation length in minutes
				SIM_LENGTH = atoi(optarg);
				break;
			case 'o':	// output file
				OUTPUT = optarg;
				break;
			case 'r': // tag refresh rate 
				TAG_REF_RATE = atoi(optarg);
				break;
			case 'b': // number of records to save to DB in one batch
				BATCH_TO_DB = atoi(optarg);
				break;
			case 's':	// wi-fi speed
				SEND_OVER_WIFI = atoi(optarg);
				break;	
			case 'w':	// wi-fi speed
				WIFI_PING = atoi(optarg);
				break;
			case 'm':	// metod for data transfer (REST || WS || UDP)
				METHOD_TRANSFER = atoi(optarg);
				break;
			case 'c': // CPU seize time
				CPU_SEIZE_TIME = stod(optarg);
				break;
			default:
				cerr << "Neplatne argumenty!" << endl;
				return -1;
		}
	}

	// simulation time in miliseconds
	SIM_TIME_END = SIM_LENGTH * SECONDS * MILISECONDS;
	
	// set output file and init the simulation
	SetOutput(OUTPUT.c_str());
	Init(0.0, SIM_TIME_END);
	
	// start the whole system
	SystemInitiator* si = new SystemInitiator();
	si->Activate();
	
	Run();

	// memory cleanup
	delete si;
	
	// outputs
	qTDOA.Output();
	qSensmap.Output();
	tag_to_sensmap.Output();
	tag_to_DB.Output();
	tag_to_client.Output();
	CPU.Output();
	Print("Celkovy pocet dat ulozenych v databazi: %lfMB.\n", dbRecords * DB_RECORD_SIZE);
	Print("Celkovy pocet iteraci procesoru %d.\n", iCPU);
	Print("Maximalni cekaci doba dat cekajici na ulozeni do databaze %lfs.\n", max_time_to_batch/SECONDS);
	return 0;
}