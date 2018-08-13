#include <db_include.h>
#include <urms.h>

extern db_clt_typ *pclt;
extern db_urms_status_t db_urms_status;

unsigned char get_ramp_flag(void);
unsigned char get_barrier_flag(void);


int send_freeway_data();
int get_meter_measurement(FILE *fp, double *freeway_occ, double *freeway_flow, double *ramp_flow);
//double get_meter_rate(double occ, double q_r, double q_in);
int send_meter_rate(double meter_rate);
