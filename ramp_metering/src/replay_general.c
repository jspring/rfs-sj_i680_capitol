

#include "rm_table.h"
//#include "wrfiles_rm.h"
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

const char *usage = "-d <Database number (Modulo 4!)> -f <filename> -i (loop interval) -v (verbose)\n"; 

int main(int argc, char *argv[]) {
	char strbuf[5000];
	FILE *filestream; 
	char *filename; 
	int use_db = 1;
        int option;
        int exitsig;
        db_clt_typ *pclt;
        char hostname[MAXHOSTNAMELEN+1];
        int interval = 1000;      /// Number of milliseconds between saves
        posix_timer_typ *ptimer;       /* Timing proxy */
        char *domain = DEFAULT_SERVICE; // usually no need to change this
        int xport = COMM_OS_XPORT;      // set correct for OS in sys_os.h
        int verbose = 0;
        int db_urms_status_var = 0;
        int db_urms_datafile_var = 0;
        int db_urms_var = 0;
        int db_urms_status2_var = 0;
        int db_urms_status3_var = 0;
        int db_ramp_var = 0;
        extern db_urms_status_t db_urms_status;
        extern db_urms_status2_t db_urms_status2;
        extern db_urms_status3_t db_urms_status3;
        extern urms_datafile_t urms_datafile;
	extern db_ramp_data_t db_ramp_data;
	extern db_urms_t db_urms;

	unsigned short temp_ushort = 0;
	int i;

        while ((option = getopt(argc, argv, "d:i:f:v")) != EOF) {
                switch(option) {
                case 'd':
                        db_urms_status_var = atoi(optarg);
                        db_urms_datafile_var = db_urms_status_var + 1;
                        db_urms_var = db_urms_status_var + 2;
                        db_urms_status2_var = db_urms_status_var + 3;
                        db_urms_status3_var = db_urms_status_var + 4;
                        db_ramp_var = db_urms_status_var + 5;
                        break;
                case 'i':
                        interval= atoi(optarg);
                        break;
		case 'f':
			filename = strdup(optarg);
			break;
                case 'v':
                        verbose = 1;
                        break;
               default:
                        printf("Usage: %s %s\n", argv[0], usage);
                        exit(EXIT_FAILURE);
                        break;
                }
        }

	if(use_db) {
		get_local_name(hostname, MAXHOSTNAMELEN);
		if ( (pclt = db_list_init(argv[0], hostname, domain, xport, NULL, 0, NULL, 0)) == NULL) {
			printf("Database initialization error in %s.\n", argv[0]);
			exit(EXIT_FAILURE);
		}
		/* Setup a timer for every 'interval' msec. */
		if ((ptimer = timer_init(interval, DB_CHANNEL(pclt) )) == NULL) {
			printf("Unable to initialize wrfiles timer\n");
			exit(EXIT_FAILURE);
		}
	}

	if(( exitsig = setjmp(exit_env)) != 0) {
		db_list_done(pclt, NULL, 0, NULL, 0);
		exit(EXIT_SUCCESS);
	} else
		sig_ign(sig_list, sig_hand);

	memset(strbuf, 0, 5000);
	filestream = fopen(filename, "r");
	while(fgets(strbuf, 5000, filestream) != NULL) {
		get_data_log_line(strbuf, file_spec, num_file_columns);
		if(use_db) {
#define NUM_MAINLINES           8
#define NUM_ONRAMP_LANES        4
#define NUM_OFFRAMP_LANES       4
                        for(i=0; i<NUM_MAINLINES; i++) {
                                temp_ushort = (unsigned short)(urms_datafile.mainline_lead_occ[i] * 10);
                                db_urms_status.mainline_stat[i].lead_occ_msb = (unsigned char)( (temp_ushort >> 8) & 0xFF);
                                db_urms_status.mainline_stat[i].lead_occ_lsb = (unsigned char)(temp_ushort & 0xFF);
		if(verbose) {
			printf("%hhx lead_occ %.3f msb %#hhx lsb %#hhx num_file_columns %d\n", 
				db_urms_status.mainline_stat[i].lead_stat,
				urms_datafile.mainline_lead_occ[i],
				db_urms_status.mainline_stat[i].lead_occ_msb, 
				db_urms_status.mainline_stat[i].lead_occ_lsb, 
				num_file_columns
			);
                        }
			db_clt_write(pclt, db_urms_status_var, sizeof(db_urms_status_t), &db_urms_status);
			db_clt_write(pclt, db_urms_status2_var, sizeof(db_urms_status2_t), &db_urms_status2);
			db_clt_write(pclt, db_urms_status3_var, sizeof(db_urms_status3_t), &db_urms_status3);
			db_clt_write(pclt, db_urms_var, sizeof(db_urms_t), &db_urms);
			db_clt_write(pclt, db_ramp_var, sizeof(db_ramp_data_t), &db_ramp_data);
			db_clt_write(pclt, db_urms_datafile_var, sizeof(urms_datafile_t), &urms_datafile);
			printf("strbuf %s\n", strbuf);
		}
		}
		TIMER_WAIT(ptimer);
	}

	return 0;
}
