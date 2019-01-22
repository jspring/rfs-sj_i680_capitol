/* send_char_to_db_var.c - well what do you think it does?
*/

#include <db_include.h>
#include "ab3418comm.h"
#include "clt_vars.h"

static db_id_t db_vars_list[] =  {
        {0, sizeof(db_set_pattern_t)},
};
int num_db_vars = sizeof(db_vars_list)/sizeof(db_id_t);

const char *controller_strings[] =  {
	"10.192.131.9", 
	"10.192.131.8", 
	"10.192.131.60", 
	"10.192.131.7", 
	"10.192.131.6", 
	"10.192.131.1", 
	"10.192.131.10", 
	"10.192.131.11", 
	"10.192.131.12", 
	"10.192.131.13", 
	"10.192.131.14", 
	"10.192.131.15", 
	"10.192.131.16", 
	"10.192.131.59" 
};

int db_arterial_trig_list[] =  {
	DB_10_192_131_9_VAR+1, 
	DB_10_192_131_8_VAR+1, 
	DB_10_192_131_60_VAR+1, 
	DB_10_192_131_7_VAR+1, 
	DB_10_192_131_6_VAR+1, 
	DB_10_192_131_1_VAR+1, 
	DB_10_192_131_10_VAR+1, 
	DB_10_192_131_11_VAR+1, 
	DB_10_192_131_12_VAR+1, 
	DB_10_192_131_13_VAR+1, 
	DB_10_192_131_14_VAR+1, 
	DB_10_192_131_15_VAR+1, 
	DB_10_192_131_16_VAR+1, 
	DB_10_192_131_59_VAR+1, 
};

int NUM_ARTERIAL_TRIG_VARS = sizeof(db_arterial_trig_list)/sizeof(int);

#define DEBUG

int main(int argc, char *argv[]) {

        char hostname[MAXHOSTNAMELEN];
        char *domain = DEFAULT_SERVICE; /// on Linux sets DB q-file directory
        db_clt_typ *pclt;               /// data server client pointer
        unsigned int xport = COMM_OS_XPORT;      /// value set correctly in sys_os.h
	db_set_pattern_t db_set_pattern;
	int retval = 0;
	int i;
	trig_info_typ trig_info;

	char *str1 = "ssh -p 5571 jspring@localhost \"/home/atsc/ab3418/lnx/ab3418comm -A 192.168.200.120 -o 162 -P \"";
	char *str2 = " -v | grep \"checksum OK\""; 
	char whole_shebang[200] = {0};

#ifdef DEBUG

	str1 = "/home/atsc/ab3418/lnx/ab3418comm -A 10.192.131.150 -o 162 -P ";
#endif	
        get_local_name(hostname, MAXHOSTNAMELEN);
        if ( (pclt = db_list_init(argv[0], hostname, domain, xport, NULL, 0, db_arterial_trig_list, NUM_ARTERIAL_TRIG_VARS)) == NULL) {
            exit(EXIT_FAILURE);
        }

	while(1) {

		retval = clt_ipc_receive(pclt, &trig_info, sizeof(trig_info));
		for(i=0; i<NUM_ARTERIAL_TRIG_VARS; i++) {
		       	if( DB_TRIG_VAR(&trig_info) == db_arterial_trig_list[i]) {
				memset(whole_shebang, 0, sizeof(whole_shebang));
				sprintf(whole_shebang, "%s %hhu -a %s %s \n ", str1, db_set_pattern.pattern, controller_strings[i], str2);
				db_clt_read(pclt, db_arterial_trig_list[i], sizeof(db_set_pattern_t), &db_set_pattern);
				retval = system(whole_shebang);
				if(retval == 0) 
					printf("set_pattern returned OK: db_var %d val %d\n", db_arterial_trig_list[i], db_set_pattern.pattern);
				else
					printf("set_pattern did not return OK: db_var %d val %d\n", db_arterial_trig_list[i], db_set_pattern.pattern);
			}
		}
	}
	return 0;
}
