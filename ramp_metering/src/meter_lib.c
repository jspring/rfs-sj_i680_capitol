#include "meter_lib.h"
#include <ab3418_lib.h>

#define LAMBDA_IN 2
#define LAMBDA_OUT 3
#define ALPHA 1.0
#define OCC_CRITICAL 25.0
#define K_R 70.0
#define ALINEA_LOWER_BOUND 1200.0
#define ALINEA_UPPER_BOUND 1700.0

db_clt_typ *pclt;
db_urms_status_t db_urms_status;
phase_status_t phase_status;

int send_freeway_data()
{
	return 0;
}

int get_meter_measurement(FILE *fp, double *freeway_occ, double *freeway_flow, double *ramp_flow)
{
	int flag;
	flag = fscanf(fp, "%lf %lf %lf", freeway_occ, freeway_flow, ramp_flow);
	if(flag!=3)
		return 1;
	else
		return 0;
}

/*
double get_meter_rate(double occ, double q_r, double q_in)
{
	//occ is mainline upstream occ, q_r is ramp flow(current meter rate), q_in is mainline upstream measured flow
	double occ_out;
	double new_rate;

	occ_out=ALPHA*occ*(1+q_r/q_in)*LAMBDA_IN/LAMBDA_OUT;

	new_rate=q_r+K_R*(OCC_CRITICAL-occ_out);
	if(new_rate<ALINEA_LOWER_BOUND)
		new_rate=ALINEA_LOWER_BOUND;
	if(new_rate>ALINEA_UPPER_BOUND)
		new_rate=ALINEA_UPPER_BOUND;
	new_rate=(double)(int)new_rate;
	return new_rate;
}
*/
int send_meter_rate(double meter_rate)
{
	printf("%5.2lf\n",meter_rate);
	return 0;
}
unsigned char get_ramp_flag(void) {

        unsigned char ramp_flag = 0;

        db_clt_read(pclt, DB_URMS_STATUS_VAR, sizeof(db_urms_status_t), &db_urms_status);
        ramp_flag = db_urms_status.computation_finished;
        if(ramp_flag)
                printf("get_ramp_flag: got ramp flag; statistics display on ramp meter controller should have just changed\n");
        return ramp_flag;
}

unsigned char get_barrier_flag(void) {

        unsigned char barrier_flag = 0;

        db_clt_read(pclt, DB_PHASE_STATUS_VAR, sizeof(phase_status_t), &phase_status);
        barrier_flag = phase_status.barrier_flag;
        if(barrier_flag)
                printf("get_barrier_flag: got arterial barrier flag %hhu; phases 6 and 2 should be yellow now\n", barrier_flag);
        return barrier_flag;
}
