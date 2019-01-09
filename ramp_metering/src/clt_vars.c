/**\file
 * clt_vars.c	Creates all database client variables for ramp metering project    File to save the data from the arterial coordination
 */

#include "clt_vars.h"

static int sig_list[] = 
{
	SIGINT,
	SIGQUIT,
	SIGTERM,
	SIGALRM,
	ERROR,
};

static jmp_buf exit_env;

static void sig_hand(int code)
{
	if (code == SIGALRM)
		return;
	else
		longjmp(exit_env, code);
}

int main(int argc, char *argv[])
{
	int option;
	int exitsig;
	db_clt_typ *pclt;
	char hostname[MAXHOSTNAMELEN+1];

	int interval = 50;	/// Number of milliseconds between saves 
	posix_timer_typ *ptimer;       /* Timing proxy */
	char *domain = DEFAULT_SERVICE; // usually no need to change this 
	int xport = COMM_OS_XPORT;	// set correct for OS in sys_os.h 
	int verbose = 0;
	char zero_array[200];
	int i;

	memset(zero_array, 0, sizeof(zero_array));

	/* Read and interpret any user switches. */
	while ((option = getopt(argc, argv, "i:v")) != EOF) {
		switch(option) {
	        case 'i':
			interval = atoi(optarg); 
			break;
	        case 'v':
			verbose = 1; 
			break;
	        default:
			printf("Usage: %s\n", argv[0]); 
			printf("    -d <trip directory> \n");
			printf("    -m <file time in minutes> \n");
			printf("    -l <no. lines to save to file> \n");
			printf("    -t <loop time in ms> \n");
			exit(EXIT_FAILURE);
	        }
	}

	/* Log in to the database (shared global memory).  Default to the
	 * the current host. Assumes other processes that create variables
	 * are already running, since list is NULL.
	 */
	get_local_name(hostname, MAXHOSTNAMELEN);
	if ((pclt = db_list_init(argv[0], hostname, domain, xport, 
			db_controller_list, NUM_CONTROLLER_VARS, NULL, 0)) == NULL) {
		printf("Database initialization error in %s.\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	for (i = 0; i < NUM_CONTROLLER_VARS; i++){
		db_clt_write(pclt,
		db_controller_list[i].id,
		db_controller_list[i].size,
		zero_array);
	}
        
	if ((pclt = db_list_init(argv[0], hostname, domain, xport, 
			db_arterial_list, NUM_ARTERIAL_VARS, NULL, 0)) == NULL) {
		printf("Database initialization error in %s.\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	for (i = 0; i < NUM_ARTERIAL_VARS; i++){
		db_clt_write(pclt,
		db_arterial_list[i].id,
		db_arterial_list[i].size,
		zero_array);
	}
        
	/* Setup a timer for every 'interval' msec. */
	if ((ptimer = timer_init(interval, DB_CHANNEL(pclt) )) == NULL) {
		printf("Unable to initialize wrfiles timer\n");
		exit(EXIT_FAILURE);
	}

	if(( exitsig = setjmp(exit_env)) != 0) {
		db_list_done(pclt, db_controller_list, NUM_CONTROLLER_VARS, NULL, 0);
		exit(EXIT_SUCCESS);
	} else
		sig_ign(sig_list, sig_hand);

	for(;;) {
		TIMER_WAIT(ptimer);
	}
}
