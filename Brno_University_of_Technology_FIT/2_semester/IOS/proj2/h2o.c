/*
 * File:        h2o.c
 * Date:        2015/5/1
 * Author:      Jan Ondruch, xondru14@stud.fit.vutbr.cz
 * Project:     Building H20
 * Description: Implemetation of a well-known process synchronization problem Building H20
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <semaphore.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <stdbool.h>

#define HELP 5
#define SYSCALL_ERR 2

#define TIME_MS_MAX_VAL 5001

typedef struct {   // stores process information
	int mode;
    int N;
    unsigned int GH;
    unsigned int GO;
    unsigned int B;
    FILE *fd;
} Tparams;

/**
  * Shared memory for shared variables and semaphores
  * @param counter is a Total Action Count
  * @param oxygen is a number of Produced oxygens 
  * @param oxy_proc_cnt is a count of Total Oxygens Produced so far
  * @param sem_counter is a mutex for Total Action Count
  * @param sem_proc_h is a sem_post to Signal to Create New Hydrogen
  * @param sem_creat unlocks and locks Oxy and Hydro Threads
  */
typedef struct {
	int counter, oxygen, oxy_proc_cnt, hydrogen, hydro_proc_cnt, bar, bonded_cnt, fin_hydro;
	sem_t sem_write, sem_counter, sem_pc, sem_creat, sem_proc_o, sem_proc_h, sem_bond, sem_kill_child, sem_finish_bond, sem_block, sem_fin_hydro;
	int *processes_h, *processes_o;	// number of created processes, O & H 
} Tmemory;

Tmemory *memory = NULL;
int shm_key = NULL;

/**
 * function prototypes
 */

int init_mem(Tparams *params);
int deinit_mem(void);
void print_out(FILE *fd, const char *name, int pc, const char *action);
void oxyQueue(const Tparams *params, int o_cnt);
void hydroQueue(const Tparams *params, int h_cnt);
int counter(void);
void bond(const char *name, int pc, Tparams params);
int inc_pc(int pc);
void terminate(pid_t pid_O);
int parse_args(int argc, const char *argv[], Tparams *params);

/**
 * shm initialization
 */

int init_mem(Tparams *params) {

	if ((shm_key = shmget(IPC_PRIVATE, sizeof(Tmemory), IPC_EXCL | IPC_CREAT | 0666)) == -1) {
		fprintf(stderr, "Error while initializing shared memory.\n");
		return SYSCALL_ERR;
	}

	if ((memory = (Tmemory *) shmat(shm_key, NULL, 0)) == NULL) {
    	fprintf(stderr, "Error while initializing shared memory.\n");
    	return SYSCALL_ERR;
	}


	if (sem_init(&memory->sem_write, 1, 1) == -1 ||	// unlock -> do.. -> lock
		sem_init(&memory->sem_counter, 1, 1) == -1 || // unlock -> do.. -> lock
		sem_init(&memory->sem_bond, 1, 1) == -1 ||	  // unlock -> do.. -> lock
		sem_init(&memory->sem_pc, 1, 1) == -1 || 	  // unlock -> do.. -> lock
		sem_init(&memory->sem_block, 1, 0) == -1 ||
		sem_init(&memory->sem_kill_child, 1, 0) == -1 || 	// locked initially
		sem_init(&memory->sem_finish_bond, 1, 0) == -1 ||	// locked initially
		sem_init(&memory->sem_fin_hydro, 1, 0) == -1 || // locked initially
		sem_init(&memory->sem_creat, 1, 1) == -1 ||   // set on 1 initially
		sem_init(&memory->sem_proc_o, 1, 0) == -1 ||  // locked initially
		sem_init(&memory->sem_proc_h, 1, 0) == -1 	  // locked initially
		) {
		fprintf(stderr, "Error while initializing semaphores.\n");
		return SYSCALL_ERR;
	}

	memory->processes_o = malloc(sizeof(int) * params->N);
	memory->processes_h = malloc(sizeof(int) * ((params->N) * 2));
	if ((memory->processes_o == NULL) || (memory->processes_h == NULL)) {
		fprintf(stderr, "Error while initializing memory (malloc).\n");
		return SYSCALL_ERR;
	}

	return EXIT_SUCCESS;
}

/**
 * shm deinitialization
 */

int deinit_mem(void) {
	int err = EXIT_SUCCESS;

	free(memory->processes_o);
	free(memory->processes_h);

	if (sem_destroy(&memory->sem_write) == -1 ||
		sem_destroy(&memory->sem_counter) == -1 ||
		sem_destroy(&memory->sem_bond) == -1 ||
		sem_destroy(&memory->sem_pc) == -1 ||
		sem_destroy(&memory->sem_block) == -1 ||
		sem_destroy(&memory->sem_kill_child) == -1 ||
		sem_destroy(&memory->sem_finish_bond) == -1 ||
		sem_destroy(&memory->sem_fin_hydro) == -1 ||
		sem_destroy(&memory->sem_creat) == -1 ||
		sem_destroy(&memory->sem_proc_o) == -1 ||
		sem_destroy(&memory->sem_proc_h) == -1
		) {
		fprintf(stderr, "Error while deleting semaphores.\n");
		err = SYSCALL_ERR;
	}

	if (shmctl(shm_key, IPC_RMID, NULL) == -1) {
    	fprintf(stderr, "Error while deleting shared memory.\n");
    	err = SYSCALL_ERR;
  	}

  	return err;
}

/** 
  * print process actions to file
  */

void print_out(FILE *fd, const char *name, int pc, const char *action) {

	sem_wait(&memory->sem_write);	//lock

	int count = counter();

	// A: NAME I: action
	fprintf(fd, "%d\t : %s %d\t  : %s\n", count, name, pc, action);
	
	fflush(fd);

	sem_post(&memory->sem_write);	//unlock
}


/** 
  * main oxygen process
  */

void oxyQueue(const Tparams *params, int o_cnt) {
	
	sem_wait(&memory->sem_creat);	// lock, initially set on 1 I guess

	memory->oxygen = inc_pc(memory->oxygen);	// rename process??
	memory->oxy_proc_cnt = inc_pc(memory->oxy_proc_cnt);
	o_cnt = memory->oxy_proc_cnt;

	print_out(params->fd, "O", memory->oxy_proc_cnt, "started");

	if (memory->hydrogen >= 2) {
		print_out(params->fd, "O", memory->oxy_proc_cnt, "ready");

		//signal twice
		sem_post(&memory->sem_proc_h);
		sem_post(&memory->sem_proc_h);

		memory->hydrogen -= 2;

		sem_post(&memory->sem_proc_o);

		memory->oxygen -= 1;
	}
	else {
		print_out(params->fd, "O", memory->oxy_proc_cnt, "waiting");
		sem_post(&memory->sem_creat);	// unlock
	}

	sem_wait(&memory->sem_proc_o);

	bond("O", o_cnt, *params);

	sem_wait(&memory->sem_finish_bond);	
	
	
	print_out(params->fd, "O", o_cnt, "bonded");
	
	(memory->bonded_cnt)++;

	sem_wait(&memory->sem_block);
	
	sem_wait(&memory->sem_fin_hydro);
	sem_wait(&memory->sem_fin_hydro);

	memory->fin_hydro = 0;
	sem_post(&memory->sem_creat);
	if (memory->bonded_cnt == (params->N)*3) {
		sem_post(&memory->sem_kill_child);	// start killing children
	}
	sem_wait(&memory->sem_kill_child);

	print_out(params->fd, "O", o_cnt, "finished");
	sem_post(&memory->sem_kill_child);
	_exit(EXIT_SUCCESS);
}

/** 
  * main hydrogen process
  */

void hydroQueue(const Tparams *params, int h_cnt) {

	sem_wait(&memory->sem_creat);	// lock the mutex

	memory->hydrogen = inc_pc(memory->hydrogen);	// rename process??
	memory->hydro_proc_cnt = inc_pc(memory->hydro_proc_cnt);
	h_cnt = memory->hydro_proc_cnt;

	print_out(params->fd, "H", memory->hydro_proc_cnt, "started");

	if ((memory->hydrogen >= 2) && (memory->oxygen >= 1)) {
		print_out(params->fd, "H", memory->hydro_proc_cnt, "ready");
		
		//signal twice 2 hydrogens to proceed
		sem_post(&memory->sem_proc_h);
		sem_post(&memory->sem_proc_h);

		memory->hydrogen -= 2;
		sem_post(&memory->sem_proc_o);
		memory->oxygen -= 1;
	}
	else {
		print_out(params->fd, "H", memory->hydro_proc_cnt, "waiting");
		sem_post(&memory->sem_creat);	// unlock
	}

	sem_wait(&memory->sem_proc_h);	// waiting for 2 signals
	
	bond("H", h_cnt, *params);

	sem_wait(&memory->sem_finish_bond);	


	print_out(params->fd, "H", h_cnt, "bonded");

	(memory->bonded_cnt)++;

	sem_wait(&memory->sem_block);
	(memory->fin_hydro)++;
	sem_post(&memory->sem_fin_hydro);
	

	if (memory->bonded_cnt == (params->N)*3) {
		sem_post(&memory->sem_kill_child);	// start killing children
	}

	sem_wait(&memory->sem_kill_child);	

	print_out(params->fd, "H", h_cnt, "finished");
	sem_post(&memory->sem_kill_child);
	_exit(EXIT_SUCCESS);
}

/**
  * creates a molecule using 2H and 1O
  */
 
void bond(const char *name, int pc, Tparams params) {

	sem_wait(&memory->sem_bond);	// lock
	print_out(params.fd, name, pc, "begin bonding");
	usleep((random() % (params.B + 1)) * 1000);
	
	(memory->bar++);

	if (memory->bar == 3) {
		sem_post(&memory->sem_finish_bond);
		sem_post(&memory->sem_finish_bond);
		sem_post(&memory->sem_finish_bond);
		memory->bar = 0;
		sem_post(&memory->sem_block);
		sem_post(&memory->sem_block);
		sem_post(&memory->sem_block);
	}

	sem_post(&memory->sem_bond);	// unlock
}

/**
  * oxy and hydro proc counter
  * @params pc is Process Count
  */
 
int inc_pc(int pc) {
	sem_wait(&memory->sem_pc);	//lock, initially set to 1
	pc++;
	sem_post(&memory->sem_pc);	//unlock

	return pc;
}

/**
  * total action counter
  */

int counter(void) {
	sem_wait(&memory->sem_counter);	//lock, initially set to 1

	int count = memory->counter;

	(memory->counter)++;
	sem_post(&memory->sem_counter);	//unlock

	return count;
}

/**
  * terminates program and clears memory
  */

void terminate(pid_t pid_O) {
	deinit_mem();

	kill(pid_O, SIGTERM);
	kill(getppid(), SIGTERM);

	exit(SYSCALL_ERR);
}


/**
  * argument parsing
  */

int parse_args(int argc, const char *argv[], Tparams *params) {

    if (argc == 2 && (!strcmp("--help", argv[1]))) {
        printf("usage: ... \n");     // help msg is printed out
        params->mode = HELP;
        return EXIT_SUCCESS;
    }

	else if (argc == 5) {

		for (int i = 1; i < argc; i++) {
			if (!isdigit(*argv[i])) {
				return EXIT_FAILURE;
			}
		}

		if (atoi(argv[1]) > 0) {
			params->N = atoi(argv[1]);
		}
		else {
			return EXIT_FAILURE;
		}

		if (((atoi(argv[2]) >= 0) && (atoi(argv[2]) < TIME_MS_MAX_VAL)) && ((atoi(argv[3]) >= 0) && (atoi(argv[3]) < TIME_MS_MAX_VAL)) && ((atoi(argv[4])) >= 0 && (atoi(argv[4]) < TIME_MS_MAX_VAL))) {
			params->GH = atoi(argv[2]);
			params->GO = atoi(argv[3]);
			params->B = atoi(argv[4]);
		}
		else {
			return EXIT_FAILURE;
		}
	}

	else {
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}


int main(int argc, char const *argv[]) {

	Tparams params;

	pid_t pid_proc, pid_O, pid_H;

	if (!parse_args(argc, argv, &params)) {
		if (params.mode == HELP) {
			// help message
			return EXIT_SUCCESS;
		}
		else {
			// main programm
			if ((init_mem(&params)) == SYSCALL_ERR) {
				return SYSCALL_ERR;
			}

			if ((params.fd = fopen("./h2o.out", "w+")) == NULL) {
				fprintf(stderr, "Error while opening h2o.c\n");
				deinit_mem();
				return SYSCALL_ERR;
			}
	 
			setbuf(params.fd, NULL);
			srand(time(0));

			// initialize shm values
			memory->counter = 1;
			memory->oxy_proc_cnt = 0;
			memory->hydro_proc_cnt = 0;
			memory->bar = 0;
			memory->bonded_cnt = 0;
			memory->fin_hydro = 0;

			pid_proc = fork();
			if (pid_proc == 0) {
				// child process, creating H				

				for (int i = 0; i < (params.N)*2; i++) {
					pid_H = fork();
					if (pid_H == 0) {
						// create H process

						usleep((random() % (params.GH + 1)) * 1000);
						hydroQueue(&params, i+1);
					}
					else if (pid_H > 0) {
						// save PID of the process
						memory->processes_h[i] = pid_H;
					}
					else {
						// syserr
						fprintf(stderr, "Error while creating H process.\n");
						fclose(params.fd);
						terminate(pid_proc);
					}
				}
			}
			else if (pid_proc > 0) {
				// parent process, creating O
				
				for (int j = 0; j < params.N; j++) {
					pid_O = fork();
					if (pid_O == 0) {
						// create O process

						usleep((random() % (params.GO + 1)) * 1000);
						oxyQueue(&params, j+1);
					}
					else if (pid_O > 0) {
						memory->processes_o[j] = pid_O;
					}
					else {
						// syserr
						fprintf(stderr, "Error while creating O process.\n");
						fclose(params.fd);
						terminate(pid_proc);
					}
				}
			}
			else {
				// syserr
				fprintf(stderr, "Error while creating process.\n");
				fclose(params.fd);
				terminate(pid_proc);
			}
		}
	}
	else {
		fprintf(stderr, "Wrong arguments passed\n");
		return EXIT_FAILURE;
	}
	
	// waiting for childs to finish
	for (int i = 0; i < params.N; i++) {
		waitpid(memory->processes_o[i], NULL, 0);
	}
	for (int i = 0; i < (params.N)*2; i++) {
		waitpid(memory->processes_h[i], NULL, 0);
	}


	// closing file and dealocating resources
	fclose(params.fd);
	if (deinit_mem() == SYSCALL_ERR) {
		return SYSCALL_ERR;
	}

	return EXIT_SUCCESS;
}