/* opt_crm.c - SR99 data aggregation and control
**
** WARNING!! NUM_CONTROLLER_VARS includes db_urms_status_t, urms_datafile_t, db_urms_t, db_urms_status2_t,
** db_urms_status3_t, and db_ramp_data_t.
** For the present purposes (i.e. 4/20/2016) we're using only db_urms_status_t, which contains the data 
** Cheng-Ju needs for data aggregation. We will eventually need all 28*6=168 database variables.
**
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <string.h>

#include "parameters.h"

#define NRANSI

#include "nrutil2.h"
#include "resource.h"
#include "rm_algo.h"
#include "clt_vars.h"
#include <db_include.h>
#include <msgs.h>
#include <ab3418_lib.h>

char str[len_str];

FILE *dbg_f, *dmd_f, *local_rm_f, *cal_opt_f, *pp, *st_file, *st_file_out, *Ln_RM_rt_f, *dbg_st_file_out, *st_file_new;


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

const char *usage = "-i <loop interval> -r (run in replay mode) -d (global disable; does NOT write plan change db variable!!)";

#define NUM_CYCLE_BUFFS  5
//
const char *controller_strings[] = {
        "Capitol_Expy",             //0, OR1
        "Alum_Rock_loop",             //0, OR1
        "Alum_Rock_Diag",             //0, OR1
        "McKee",             //0, OR1
        "Berryessa",             //0, OR1
}; //FR6, FR7, FR8, and FR11 are missing


#define NOCTL   		0
#define CTL     		1
#define MAX_ART_PER_RAMP	7
#define FREE_PATTERN		255
#define CTL_INTRVL_MS		119000

typedef struct {
        char *name;
	int controlling_rm_index;
        unsigned char ctl_permission;
        unsigned char ctl_state;
        unsigned char pattern;
	int db_var;
	char *ipaddr;
} arterial_desc_t;

arterial_desc_t arterial_desc[] = {
        {"Wilbur_Ave", 2, NOCTL, NOCTL, 0, 4000, "10.192.131.9"},		//07:00; 255->255 09:00; 255->255 16:00: 255->255
        {"Florence_Ave", 2, NOCTL, NOCTL, 0, 4100, "10.192.131.8"},		//07:00; 255->255 09:00; 255->004 16:00: 004->255
        {"Alum_Rock_Ave", 2, NOCTL, NOCTL, 0, 4200, "10.192.131.60"},		//07:00; 002->001 09:00; 255->002 16:00: 002->003
        {"Madden_Ave", 3, CTL, NOCTL, 9, 4300, "10.192.131.7"},			//07:00; 255->255 09:00; 255->255 16:00: 255->255
        {"Gay_Ave", 3, CTL, NOCTL, 9, 4400, "10.192.131.6"},			//07:00; 255->255 09:00; 255->255 16:00: 255->255
        {"McKee_Ave", 3, CTL, NOCTL, 9, 4500, "10.192.131.1"},			//07:00; 255->001 09:00; 255->004 16:00: 004->003
        {"Capitol_Square_Ave", 4, CTL, NOCTL, 9, 4600, "10.192.131.10"},	//07:00; 255->255 09:00; 255->004 16:00: 004->255
        {"Gianotta_Ave", 4, CTL, NOCTL, 9, 4700, "10.192.131.11"},		//07:00; 255->255 09:00; 255->255 16:00: 255->255
        {"Rainfield_Ave", 4, CTL, NOCTL, 9, 4800, "10.192.131.12"},		//07:00; 255->255 09:00; 255->255 16:00: 255->255
        {"Mabury_Ave", 4, CTL, NOCTL, 9, 4900, "10.192.131.13"},		//07:00; 255->255 09:00; 255->255 16:00: 255->255
        {"Gilchrist_Ave", 4, CTL, NOCTL, 9, 5000, "10.192.131.14"},		//07:00; 255->255 09:00; 255->255 16:00: 255->003
        {"Penitencia_Creek_Ave", 4, CTL, NOCTL, 9, 5100, "10.192.131.15"},	//07:00; 255->255 09:00; 255->255 16:00: 255->003
        {"Berryessa_Ave", 4, CTL, NOCTL, 9, 5200, "10.192.131.16"},		//07:00; 002->002 09:00; 255->002 16:00: 002->002
        {"Alexander_Ave", 2, NOCTL, NOCTL, 9, 5300, "10.192.131.59"},		//07:00; 255->001 09:00; 255->004 16:00: 002->002
};

#define NUM_ARTERIAL_CONTROLLERS  sizeof(arterial_desc)/sizeof(arterial_desc_t)

typedef struct {
        char *name;
        int arterial_index[7];
	int inter_threshold_flag;
}ramp_meter_desc_t;

ramp_meter_desc_t ramp_meter_desc[] = {
        {"Capitol_Ave_Diag_RM", {-1,-1,-1,-1, -1, -1, -1}, -1},
        {"Alum_Rock_Loop_RM", {-1,-1,-1, -1, -1, -1, -1}, -1},
        {"Alum_Rock_Diag_RM", {0, 1, 2, -1, -1, -1, -1}, -1},
        {"McKee_Diag_RM", {3, 4, 5, -1, -1, -1, -1}, -1},
        {"Berryessa_Diag_RM", {6, 7, 8, 9, 10, 11, 12}, -1}   
                                      };

#define NUM_RAMP_CONTROLLERS    sizeof(ramp_meter_desc)/sizeof(ramp_meter_desc_t)

int set_pattern(int i, ramp_meter_desc_t *ramp_meter_desc[], unsigned char ctl);

int main(int argc, char *argv[])
{
	timestamp_t ts;
	timestamp_t *pts = &ts;
	float time = 0; // time2 = 0,timeSta = 0, tmp=0.0;
	static int init_sw=1;
	int i, j, k, l;
	db_urms_t urms_ctl[NumOnRamp] = {{0}};
	db_urms_status_t controller_data[NUM_CONTROLLERS ];  //See warning at top of file
	db_urms_status2_t controller_data2[NUM_CONTROLLERS ];  //See warning at top of file
	db_urms_status3_t controller_data3[NUM_CONTROLLERS ];  //See warning at top of file

	get_long_status8_resp_mess_typ arterial_controllers[NUM_ARTERIAL_CONTROLLERS]; //arterial controllers
	db_set_pattern_t  db_set_pattern[NUM_ARTERIAL_CONTROLLERS];

	int option;
	int exitsig;
	db_clt_typ *pclt;
	char hostname[MAXHOSTNAMELEN+1];
	posix_timer_typ *ptimer;       /* Timing proxy */
	int interval = 30000;      /// Number of milliseconds between saves
	int cycle_index = -1;      //  Set to -1 at init because it's incremented at the beginning of the infinite for loop, instead of after
	char *domain = DEFAULT_SERVICE; // usually no need to change this
	int xport = COMM_OS_XPORT;      // set correct for OS in sys_os.h
//	int verbose = 0;
	static agg_data_t mainline_out[NUM_CYCLE_BUFFS][SecSize] =  {{{0}},{{0}}};      // data aggregated section by section
	static agg_data_t onramp_out[NUM_CYCLE_BUFFS][NumOnRamp] = {{{0}},{{0}}};      // data aggregated section by section//NumOnRamp=4
	static agg_data_t onramp_queue_out[NUM_CYCLE_BUFFS][NumOnRamp] = {{{0}},{{0}}};      // data aggregated section by section//NumOnRamp=4
	static agg_data_t offramp_out[NUM_CYCLE_BUFFS][NumOnRamp] = {{{0}},{{0}}};  // data aggregated section by section
    
    static agg_data_t mainline_out_f[SecSize] = {{0}};        // save filtered data to this array
	static agg_data_t onramp_out_f[NumOnRamp] = {{0}};        // save filtered data to this array//NumOnRamp=4
	static agg_data_t offramp_out_f[NumOnRamp] = {{0}};    // save filtered data to this array
	static agg_data_t onramp_queue_out_f[NumOnRamp] = {{0}};  // save filtered data queue detector data to this array//NumOnRamp=4
	 
	static agg_data_t controller_mainline_data[NUM_CONTROLLERS ] = {{0}};     // data aggregated controller by controller 
	static agg_data_t controller_onramp_data[NumOnRamp] = {{0}};                 // data aggregated controller by controller
	static agg_data_t controller_onramp_queue_detector_data[NumOnRamp] = {{0}};
	static agg_data_t controller_offramp_data[NumOnRamp] = {{0}};               // data aggregated controller by controller
	static float hm_speed_prev [NUM_CONTROLLERS ] = {1.0};               // this is the register of harmonic mean speed in previous time step
	static float mean_speed_prev [NUM_CONTROLLERS ] = {1.0};             // this is the register of mean speed in previous time step
	static float density_prev [NUM_CONTROLLERS ] = {0};             // this is the register of density in previous time step
	static float float_temp;
	//static float ML_flow_ratio = 0.0; // current most upstream flow to historical most upstream flow
	//static float current_most_upstream_flow = 0.0;
	int global_disable = 0;
	struct timespec curr_timespec;
	struct tm *ltime;
	static int num_controller_vars = NUM_CONTROLLERS ; //See warning at top of file
	struct confidence confidence[num_controller_vars][3]; 
	int ms_now, ms_sav[NUM_ARTERIAL_CONTROLLERS], ms_diff[NUM_ARTERIAL_CONTROLLERS];

	float temp_ary_vol[NUM_CYCLE_BUFFS] = {0};    // temporary array of cyclic buffer
	float temp_ary_speed[NUM_CYCLE_BUFFS] = {0};
	float temp_ary_occ[NUM_CYCLE_BUFFS] = {0};
	float temp_ary_density[NUM_CYCLE_BUFFS] = {0};	
	float temp_ary_OR_vol[NUM_CYCLE_BUFFS] = {0};
	float temp_ary_OR_occ[NUM_CYCLE_BUFFS] = {0};
	float temp_ary_OR_queue_detector_vol[NUM_CYCLE_BUFFS] = {0};
	float temp_ary_OR_queue_detector_occ[NUM_CYCLE_BUFFS] = {0}; 
	float temp_ary_FR_vol[NUM_CYCLE_BUFFS] = {0};
	float temp_ary_FR_occ[NUM_CYCLE_BUFFS] = {0};
	int control_period_disable = 1;
	
		float temp_num_ct = 0.0; // number of controllers per section
		float temp_vol = 0.0;
		float temp_speed = 0.0;
		float temp_occ = 0.0;
		float temp_density = 0.0;
		float temp_mean_speed = 0.0;

	short metering_controller_db_vars[] = {
//		3002,	//Capitol Expressway NOT A CONTROLLED METER!!!!!
		3202,	//Alum Rock Loop
		3402,	//Alum Rock Diag
		3602,	//McKee Diag
		3802,	//Berryessa Diag
	};

	// Initialization for urms_ctl
	// Set lane 4 (nonexistent) action
	// to SKIP and metering rate to 1100 VPH
	// Set regular lanes 2 & 3 to fixed rate and all plans to 0.
	for(i=0; i<NumOnRamp; i++) {//NumOnRamp=4
		urms_ctl[i].lane_1_action = URMS_ACTION_FIXED_RATE;
		urms_ctl[i].lane_1_plan = 1;
		urms_ctl[i].lane_2_action = URMS_ACTION_FIXED_RATE;
		urms_ctl[i].lane_2_plan = 1;
		urms_ctl[i].lane_3_action = URMS_ACTION_FIXED_RATE;
		urms_ctl[i].lane_3_plan = 1;
		urms_ctl[i].lane_4_action = URMS_ACTION_SKIP;
		urms_ctl[i].lane_4_release_rate = 1100;
		urms_ctl[i].lane_4_plan = 1;
	}
	

	while ((option = getopt(argc, argv, "di:rt:T:")) != EOF) {
		switch(option) {
			case 'd':
				global_disable = 1;
				break;
			case 'i':
				interval = atoi(optarg);
				break;
			case 'r':
				pts = &controller_data2[20].ts;
				break;
			default:
				printf("\nUsage: %s %s\n", argv[0], usage);
				exit(EXIT_FAILURE);
				break;
		}
	}
	memset(controller_data, 0, NUM_CONTROLLERS  * (sizeof(db_urms_status_t)));//See warning at top of file
	memset(controller_data2, 0, NUM_CONTROLLERS  * (sizeof(db_urms_status2_t)));//See warning at top of file
	memset(controller_data3, 0, NUM_CONTROLLERS  * (sizeof(db_urms_status3_t)));//See warning at top of file

	// XYLu: adde 6/10/2019
	get_local_name(hostname, MAXHOSTNAMELEN);

	if ( (pclt = db_list_init(argv[0], hostname, domain, xport,
		NULL, 0, NULL, 0)) == NULL) {
		printf("Database initialization error in %s.\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	/* Setup a timer for every 'interval' msec. */
	if ( ((ptimer = timer_init(interval, DB_CHANNEL(pclt) )) == NULL)) {
		printf("Unable to initialize wrfiles timer\n");
		exit(EXIT_FAILURE);
	}

	if(( exitsig = setjmp(exit_env)) != 0) {
		urms_ctl[0].lane_1_action = URMS_ACTION_SKIP;
		urms_ctl[0].lane_2_action = URMS_ACTION_SKIP;
		urms_ctl[0].lane_3_action = URMS_ACTION_SKIP;
		urms_ctl[0].lane_4_action = URMS_ACTION_SKIP;
		for(i=0; i<11; i++)
			db_clt_write(pclt, metering_controller_db_vars[i], sizeof(db_urms_t), &urms_ctl[0]); 
		db_list_done(pclt, NULL, 0, NULL, 0);
		exit(EXIT_SUCCESS);
	} else
		sig_ign(sig_list, sig_hand);

	 memset(mainline_out, 0, NUM_CYCLE_BUFFS*SecSize*(sizeof(agg_data_t)));  
	 memset(onramp_out, 0, NUM_CYCLE_BUFFS*NumOnRamp*(sizeof(agg_data_t))); 
	 memset(onramp_queue_out, 0, NUM_CYCLE_BUFFS*NumOnRamp*(sizeof(agg_data_t))); 
	 memset(offramp_out, 0, NUM_CYCLE_BUFFS*NumOnRamp*(sizeof(agg_data_t))); 
	 memset(mainline_out_f, 0, SecSize*(sizeof(agg_data_t)));   
	 memset(onramp_out_f, 0, NumOnRamp*(sizeof(agg_data_t)));   
	 memset(offramp_out_f, 0, NumOnRamp*(sizeof(agg_data_t))); 
	 memset(onramp_queue_out_f, 0, NumOnRamp*(sizeof(agg_data_t)));  
	 memset(controller_mainline_data, 0, NUM_CONTROLLERS*(sizeof(agg_data_t)));  
	 memset(controller_onramp_data, 0, NumOnRamp*(sizeof(agg_data_t)));          
	 memset(controller_onramp_queue_detector_data, 0, NumOnRamp*(sizeof(agg_data_t))); 
	 memset(controller_offramp_data, 0, NumOnRamp*(sizeof(agg_data_t)));    
	 memset(confidence, 0, num_controller_vars*3*(sizeof(struct confidence))); 

	if (init_sw==1)
	{
		Init();  
		Init_sim_data_io();
		init_sw=0;
	}

	for (i = 0; i < num_controller_vars; i++){   //See warning at top of file
		db_clt_read(pclt, db_controller_list[i].id, db_controller_list[i].size, &controller_data[i]);
		db_clt_read(pclt, db_controller_list[i+15].id, db_controller_list[i+15].size, &controller_data2[i]);
		db_clt_read(pclt, db_controller_list[i+20].id, db_controller_list[i+20].size, &controller_data3[i]);
	}

	for (i = 0; i < NUM_ARTERIAL_CONTROLLERS; i++) {
		db_clt_read(pclt, arterial_desc[i].db_var, sizeof(get_long_status8_resp_mess_typ), &arterial_controllers[i]);
		db_set_pattern[i].pattern = arterial_controllers[i].pattern;
	}

	get_current_timestamp(&ts); // get current time step
	ms_now = TS_TO_MS(&ts);
	for(i=0; i<NUM_ARTERIAL_CONTROLLERS; i++)
		ms_sav[i] = ms_now;

//BEGIN MAIN FOR LOOP HERE
	for(;;)	
	{

	cycle_index++;
	cycle_index = cycle_index % NUM_CYCLE_BUFFS;
	for (i = 0; i < num_controller_vars; i++){  //See warning at top of file
		db_clt_read(pclt, db_controller_list[i].id, db_controller_list[i].size, &controller_data[i]);
		db_clt_read(pclt, db_controller_list[i+15].id, db_controller_list[i+15].size, &controller_data2[i]);
		db_clt_read(pclt, db_controller_list[i+20].id, db_controller_list[i+20].size, &controller_data3[i]);
	}
	for (i = 0; i < NUM_ARTERIAL_CONTROLLERS; i++) {
		db_clt_read(pclt, arterial_desc[i].db_var, sizeof(get_long_status8_resp_mess_typ), &arterial_controllers[i]);
		db_set_pattern[i].pattern = arterial_controllers[i].pattern;
	}

/*#################################################################################################################
###################################################################################################################*/

// Cheng-Ju's code here
    
	get_current_timestamp(&ts); // get current time step
	print_timestamp(dbg_st_file_out, pts); // #1 print out current time step to file
	print_timestamp(st_file_new, pts); // #1 print out current time step to file
 
	for(i=0;i<NUM_CONTROLLERS ;i++){
		printf("\n\n\nopt_crm: ");
		print_timestamp(stdout, pts);
		printf(" IP %s onramp1 passage volume %d demand vol %d offramp volume %d\n", 
			controller_strings[i], 
			controller_data[i].metered_lane_stat[0].passage_vol, 
			controller_data[i].metered_lane_stat[0].demand_vol, 
			controller_data3[i].additional_det[0].volume
		);
		
		// min max function bound the data range and exclude nans.
		controller_mainline_data[i].agg_vol = Mind(12000.0, Maxd( 1.0, flow_aggregation_mainline(&controller_data[i], &confidence[i][0]) ) );
		controller_mainline_data[i].agg_occ = Mind(90.0, Maxd( 1.0, occupancy_aggregation_mainline(&controller_data[i], &confidence[i][0]) ) );
		 
		float_temp = hm_speed_aggregation_mainline(&controller_data[i], hm_speed_prev[i], &confidence[i][0]);
		if(float_temp < 0){
			printf("Error %f in calculating harmonic speed for controller %s\n", float_temp, controller_strings[i]);
			float_temp = hm_speed_prev[i];
		}
		controller_mainline_data[i].agg_speed = Mind(150.0, Maxd( 1.0, float_temp) );
		 
		float_temp = mean_speed_aggregation_mainline(&controller_data[i], mean_speed_prev[i], &confidence[i][0]);
		if(float_temp < 0){
			printf("Error %f in calculating mean speed for controller %s\n", float_temp, controller_strings[i]);
			float_temp = mean_speed_prev[i];
		}
		controller_mainline_data[i].agg_mean_speed = Mind(150.0, Maxd( 1.0, float_temp) );

		if(confidence[i][0].num_total_vals > 0)
			printf("Confidence for controller %s mainline %f total_vals %f good vals %f\n", 
				controller_strings[i], 
				(float)confidence[i][0].num_good_vals/confidence[i][0].num_total_vals, 
				(float)confidence[i][0].num_total_vals, 
				(float)confidence[i][0].num_good_vals
			);
        
		controller_mainline_data[i].agg_density = Mind(125.0,Maxd( 1.0,  density_aggregation_mainline(controller_mainline_data[i].agg_vol, controller_mainline_data[i].agg_speed, density_prev[i]) ) );
		
		hm_speed_prev[i] = controller_mainline_data[i].agg_speed;
		mean_speed_prev[i] = controller_mainline_data[i].agg_mean_speed;
		density_prev[i] = controller_mainline_data[i].agg_density;

		fprintf(dbg_st_file_out,"%f ", controller_mainline_data[i].agg_vol); //2,15,28,41,54
		fprintf(dbg_st_file_out,"%f ", controller_mainline_data[i].agg_occ); //3,16,29,42,55
		fprintf(dbg_st_file_out,"%f ", controller_mainline_data[i].agg_speed); //4,17,30,43,56
		fprintf(dbg_st_file_out,"%f ", controller_mainline_data[i].agg_density); //5,18,31,44,57
		fprintf(dbg_st_file_out,"%f ", controller_mainline_data[i].agg_mean_speed);//6,19,32,45,58
        
		// assign off-ramp data to array
		controller_offramp_data[i].agg_vol =  Mind(6000.0, Maxd( 0, flow_aggregation_offramp(&controller_data3[i], &confidence[i][2]) ) );
		controller_offramp_data[i].agg_occ =  Mind(100.0, Maxd( 0, occupancy_aggregation_offramp(&controller_data3[i], &confidence[i][2]) ) );            
		controller_offramp_data[i].turning_ratio = turning_ratio_offramp(controller_offramp_data[i].agg_vol,controller_mainline_data[i-1].agg_vol);
		if(confidence[i][2].num_total_vals > 0)
			printf("Confidence for controller %s offramp %f total_vals %f good vals %f\n", 
				controller_strings[i], 
				(float)confidence[i][2].num_good_vals/confidence[i][2].num_total_vals, 
				(float)confidence[i][2].num_total_vals, 
				(float)confidence[i][2].num_good_vals
			);
		
		//fprintf(dbg_st_file_out,"FR%d ", i); //controller index 
		fprintf(dbg_st_file_out,"%f ", controller_offramp_data[i].agg_vol); //7,20,33,46,59
		fprintf(dbg_st_file_out,"%f ", controller_offramp_data[i].agg_occ); //8,21,34,47,60
		fprintf(dbg_st_file_out,"%f ", controller_offramp_data[i].turning_ratio);//9,22,35,48,61

		// assign on-ramp data to array
		controller_onramp_data[i].agg_vol = Mind(6000.0, Maxd( 0, flow_aggregation_onramp(&controller_data[i], &confidence[i][1]) ) );
		if(confidence[i][1].num_total_vals > 0)
			printf("Confidence for controller %s onramp flow %f total_vals %f good vals %f\n", 
				controller_strings[i], (float)confidence[i][1].num_good_vals/confidence[i][1].num_total_vals, 
				(float)confidence[i][1].num_total_vals, 
				(float)confidence[i][1].num_good_vals
			);
		controller_onramp_data[i].agg_occ = Mind(100.0, Maxd( 0, occupancy_aggregation_onramp_queue(&controller_data[i], &controller_data2[i], &confidence[i][1]) ) );
		if(i == 3)
			controller_onramp_data[i].agg_occ = (float)((((unsigned short)(controller_data2[i].queue_stat[0][1].occ_msb * 256)) + (unsigned char)controller_data2[i].queue_stat[0][1].occ_lsb) * 0.1);
		printf("####controller_onramp_data[%d].agg_occ %f\n", i, controller_onramp_data[i].agg_occ);

		// data from on-ramp queue detector
printf("controller_data2[%d].queue_stat[0][0].occ_msb %hhu controller_data2[%d].queue_stat[0][0].occ_lsb %hhu\n", i, controller_data2[i].queue_stat[0][0].occ_msb, i, controller_data2[i].queue_stat[0][0].occ_lsb);
printf("controller_data2[%d].queue_stat[0][1].occ_msb %hhu controller_data2[%d].queue_stat[0][1].occ_lsb %hhu\n", i, controller_data2[i].queue_stat[0][1].occ_msb, i, controller_data2[i].queue_stat[0][1].occ_lsb);
		if(confidence[i][1].num_total_vals > 0)
			printf("Confidence for controller %s onramp occupancy (queue) %f total_vals %f good vals %f\n",
				controller_strings[i], (float)confidence[i][1].num_good_vals/confidence[i][1].num_total_vals, 
				(float)confidence[i][1].num_total_vals, 
				(float)confidence[i][1].num_good_vals
			);
 
		fprintf(dbg_st_file_out,"%f ", controller_onramp_data[i].agg_vol); //10,23,36,49,62
		fprintf(dbg_st_file_out,"%f ", controller_onramp_data[i].agg_occ);//11,24,37,50,63
		fprintf(dbg_st_file_out,"%f ", controller_onramp_queue_detector_data[i].agg_vol); //12,25,38,51,64
		fprintf(dbg_st_file_out,"%f ", controller_onramp_queue_detector_data[i].agg_occ);//13,26,39,52,65
		fprintf(dbg_st_file_out,"%f ", (float)(((controller_data2[i].queue_stat[0][0].occ_msb << 8) + controller_data2[i].queue_stat[0][0].occ_lsb) * 0.1) ); //14,27,40,53,66
	}
	fprintf(dbg_st_file_out,"\n");

#define MAX_CONTROLLERS_PER_SECTION	4

// This part aggregate data for each section
// controller index for each mainline section
#undef ORIGINAL_IMPL
#ifdef ORIGINAL_IMPL
int secCTidx [SecSize][4] =  {{0, -1, -1, -1}, // controller in section 1 
                             {1,   2, -1, -1}, // controller in section 2 
                             {3,  -1, -1, -1}, // controller in section 3
                             {4,  -1, -1, -1}, // controller in section 4
//                             {??,  ??, ??, ??}, // controller in section 5
			};
#endif

			int secCTidx [SecSize][4] =  {{0, -1, -1, -1}, // controller in section 1 
                             {0,   1, -1, -1}, // controller in section 2 
                             {1,  2, -1, -1}, // controller in section 3
                             {2,  3, -1, -1}, // controller in section 4
                             {3,  4, -1, -1}, // controller in section 5                             
			};


	
//This part aggregate mainline data for each section
 	for(i=0;i<SecSize;i++){
		// this loop aggregates all controller data in each section
	
		for(j=0;j<MAX_CONTROLLERS_PER_SECTION;j++){
			if(secCTidx[i][j]>-1){
				temp_vol += controller_mainline_data[secCTidx[i][j]].agg_vol;
				temp_speed += controller_mainline_data[secCTidx[i][j]].agg_speed; 
			   	temp_occ += controller_mainline_data[secCTidx[i][j]].agg_occ;
				temp_density += controller_mainline_data[secCTidx[i][j]].agg_density;
				temp_mean_speed += controller_mainline_data[secCTidx[i][j]].agg_mean_speed;
				temp_num_ct ++;
			}
		}
		mainline_out[cycle_index][i].agg_vol = Mind(12000.0, Maxd(temp_vol/temp_num_ct,1));
		//mainline_out[cycle_index][i].agg_speed = Mind(100.0, Maxd(temp_speed/temp_num_ct,1));
		mainline_out[cycle_index][i].agg_speed = controller_mainline_data[i].agg_speed;					// xylu: 6/15/19,  Bypassed using current step data
		mainline_out[cycle_index][i].agg_occ =  Mind(90.0, Maxd(temp_occ/temp_num_ct,1));
		mainline_out[cycle_index][i].agg_density = Mind(250.0, Maxd(temp_density/temp_num_ct,1));


		// Initialize all temp variables
		temp_num_ct = 0.0; 
		temp_vol = 0.0;
		temp_speed = 0.0;
		temp_occ = 0.0;
		temp_density = 0.0;
		temp_mean_speed = 0.0;

	} 

//This part aggregate onramp data for each section
	const int onrampCTidx[NumOnRamp] = {1, 2, 3, 4}; 
	const int offrampCTidx[NumOnRamp] = {1, 2,-1,-1}; // 4 off-ramp is missing, total number of off-ramps is 9
	
	for(i=0;i<NumOnRamp;i++){
		onramp_out[cycle_index][i].agg_vol = Mind(8000.0, Maxd(controller_onramp_data[onrampCTidx[i]].agg_vol,50.0));
		onramp_out[cycle_index][i].agg_occ = Mind(90.0, Maxd(controller_onramp_data[onrampCTidx[i]].agg_occ, 5.0));
		onramp_queue_out[cycle_index][i].agg_vol = Mind(12000.0, Maxd( controller_onramp_queue_detector_data[onrampCTidx[i]].agg_vol ,5.0));
		onramp_queue_out[cycle_index][i].agg_occ = Mind(90.0, Maxd(controller_onramp_queue_detector_data[onrampCTidx[i]].agg_occ,5.0));
	}
//NumOnRamp=4
//This part aggregate onramp data for each section <--- match number of off-ramp by number of on-ramp 		 
	
	for(i=0;i<NumOnRamp;i++){ //NumOnRamp=4
		if (offrampCTidx[i] > -1.0){//<-- impute data here
			offramp_out[cycle_index][i].agg_vol = Mind(8000.0, Maxd(controller_offramp_data[offrampCTidx[i]].agg_vol,50.0));
			offramp_out[cycle_index][i].agg_occ = Mind(90.0, Maxd(controller_offramp_data[offrampCTidx[i]].agg_occ,5.0 ));
		}else{
			offramp_out[cycle_index][i].agg_vol = 0.0;
			offramp_out[cycle_index][i].agg_occ = 0.0;
		}
	} 
   
// replace bad flow data by upstream data
//if flow < 100 do upstream downstrean interpolation flow data

// average the historical data from data buffer
// moving average filter for mainline
   for(i=0; i<SecSize; i++){ //SecSize=4
		for(j=0; j<NUM_CYCLE_BUFFS; j++)
	  {
         temp_ary_vol[j]= mainline_out[j][i].agg_vol;
		 temp_ary_speed[j]= mainline_out[j][i].agg_speed;
		 temp_ary_occ[j]= mainline_out[j][i].agg_occ;
		 temp_ary_density[j]= mainline_out[j][i].agg_density;
	  }
	   mainline_out_f[i].agg_vol = mean_array(temp_ary_vol,NUM_CYCLE_BUFFS);
	   mainline_out_f[i].agg_speed = mean_array(temp_ary_speed,NUM_CYCLE_BUFFS);
       mainline_out_f[i].agg_occ = mean_array(temp_ary_occ,NUM_CYCLE_BUFFS);
	   mainline_out_f[i].agg_density = mean_array(temp_ary_density,NUM_CYCLE_BUFFS);
   }

// moving average filter for on-ramp off-ramp
   for(i=0; i<NumOnRamp; i++)
   {
	  
	  for(j=0; j<NUM_CYCLE_BUFFS; j++)
	  {
		temp_ary_OR_vol[j] = onramp_out[j][i].agg_vol; 
		temp_ary_OR_occ[j] = onramp_out[j][i].agg_occ; 
		temp_ary_OR_queue_detector_vol[j] = onramp_queue_out[j][i].agg_vol;
		temp_ary_OR_queue_detector_occ[j] = onramp_queue_out[j][i].agg_occ;
		temp_ary_FR_vol[j] = offramp_out[j][i].agg_vol;   
		temp_ary_FR_occ[j] = offramp_out[j][i].agg_occ;
		
	  }
	  //onramp_out_f[i].agg_vol = mean_array(temp_ary_OR_vol,NUM_CYCLE_BUFFS);                // xylu: 6/13, causing crash
	  onramp_out_f[i].agg_vol = 1.1*(controller_onramp_data[onrampCTidx[i]].agg_vol);   						// xylu: 6/13, Temporarily by pass the processing  
	  onramp_out_f[i].agg_occ = mean_array(temp_ary_OR_occ,NUM_CYCLE_BUFFS);
	  onramp_queue_out_f[i].agg_vol = mean_array(temp_ary_OR_queue_detector_vol,NUM_CYCLE_BUFFS); 		
	  
	  
	  if (offrampCTidx[i] > -1.0)
	  	{
	  		//offramp_out_f[i].agg_vol = mean_array(temp_ary_FR_vol,NUM_CYCLE_BUFFS);                // XYLu: moving average over-time, mean_arraay(), defined in resource.c	   
	  		//offramp_out_f[i].agg_occ = mean_array(temp_ary_FR_occ,NUM_CYCLE_BUFFS);				 // due ot crashing
	  		offramp_out_f[i].agg_vol = controller_offramp_data[offrampCTidx[i]].agg_vol;     
	  		offramp_out_f[i].agg_occ = controller_offramp_data[offrampCTidx[i]].agg_vol;    
  		}
  	  else
  		{
	  		offramp_out_f[i].agg_vol = 0.0;
	  		offramp_out_f[i].agg_occ = 0.0;
  		}
    }

// Butterworth filter for mainline
    for(i=0; i<SecSize; i++)
    {
	   mainline_out_f[i].agg_vol = butt_2_ML_flow(mainline_out_f[i].agg_vol, i);                       
	   mainline_out_f[i].agg_speed = butt_2_ML_speed(mainline_out_f[i].agg_speed, i);
       mainline_out_f[i].agg_occ = butt_2_ML_occupancy(mainline_out_f[i].agg_occ, i);
       mainline_out_f[i].agg_density=(mainline_out_f[i].agg_vol)/Maxd((mainline_out_f[i].agg_speed)*(lambda[i]), 10.0);  		// xylu: Revised  6/14/2019
       mainline_out_f[i].agg_density=butt_2_ML_density((mainline_out_f[i].agg_vol)/Maxd((mainline_out_f[i].agg_speed)*(lambda[i]), 10.0),i);  		
	   //mainline_out_f[i].agg_density = butt_2_ML_density(mainline_out_f[i].agg_density, i);
    }


/*###################################################################################################################
###################################################################################################################*/

		print_timestamp(st_file_out, pts);//1
		for(i=0;i<SecSize;i++) //SecSize=4
		{
			    detection_s[i]->data[Np-1].flow=Mind(12000.0, Maxd(mainline_out_f[i].agg_vol, 200.0));
			    detection_s[i]->data[Np-1].speed=Mind(100.0, Maxd(mainline_out_f[i].agg_speed, 5.0));
			    detection_s[i]->data[Np-1].occupancy=Mind(100.0, Maxd(mainline_out_f[i].agg_occ, 5.0));
			    detection_s[i]->data[Np-1].density=Mind(1200.0, Maxd(mainline_out_f[i].agg_density, 10.0));
			    
                //fprintf(st_file_out,"Sec %d ", i); 
			    fprintf(st_file_new,"%f ", mainline_out_f[i].agg_vol); //2,6,10,14
                fprintf(st_file_new,"%f ", mainline_out_f[i].agg_speed); 		//3 ,7,11,15
				fprintf(st_file_new,"%f ", mainline_out_f[i].agg_occ); //4,8,12,16
				fprintf(st_file_new,"%f ", mainline_out_f[i].agg_density); //5,9,13,17
				
				fprintf(st_file_out,"%f ", mainline_out_f[i].agg_vol); //2,6,10,14
                fprintf(st_file_out,"%f ", mainline_out_f[i].agg_speed); 		//3 ,7,11,15
				fprintf(st_file_out,"%f ", mainline_out_f[i].agg_occ); //4,8,12,16
				fprintf(st_file_out,"%f ", mainline_out_f[i].agg_density); //5,9,13,17



		} 
        
		for(i=0;i<NumOnRamp;i++) //NumOnRamp=4 
		{	
				detection_onramp[i]->data[Np-1].flow=Mind(6000.0, Maxd(onramp_out_f[i].agg_vol, 100.0));				
				detection_onramp[i]->data[Np-1].occupancy=Mind(100.0, Maxd(onramp_out_f[i].agg_occ, 5.0)); 
				detection_offramp[i]->data[Np-1].flow=Mind(6000.0, Maxd(offramp_out_f[i].agg_vol, 100.0));
				detection_offramp[i]->data[Np-1].occupancy=Mind(100.0, Maxd(offramp_out_f[i].agg_occ, 5.0)); 	
				fprintf(st_file_new,"%f ", onramp_out_f[i].agg_vol);      //18,24,30				
				fprintf(st_file_new,"%f ", onramp_out_f[i].agg_occ);      //19,25,31
				fprintf(st_file_new,"%f ", offramp_out_f[i].agg_vol);     //20,26,32
				fprintf(st_file_new,"%f ", offramp_out_f[i].agg_occ);     //21,27,33
				fprintf(st_file_new,"%f ", onramp_queue_out_f[i].agg_vol);//22,28,34
				fprintf(st_file_new,"%f", onramp_queue_out_f[i].agg_occ);//23,29,35
				fprintf(st_file_new,"\n");
				
				fprintf(st_file_out,"%f ", onramp_out_f[i].agg_vol);      //18,24,30				
				fprintf(st_file_out,"%f ", onramp_out_f[i].agg_occ);      //19,25,31
				fprintf(st_file_out,"%f ", offramp_out_f[i].agg_vol);     //20,26,32
				fprintf(st_file_out,"%f ", offramp_out_f[i].agg_occ);     //21,27,33
				fprintf(st_file_out,"%f ", onramp_queue_out_f[i].agg_vol);//22,28,34
				fprintf(st_file_out,"%f", onramp_queue_out_f[i].agg_occ);//23,29,35
				
				
				max_occ_2_dwn[i]=detection_s[i]->data[Np-1].occupancy;	
				max_occ_all_dwn[i]=detection_s[i]->data[Np-1].occupancy;		
				
				/*for (j=i+1;j<=i+2;j++)
				{					
					if (j < NumOnRamp+1)
						max_occ_2_dwn[i]=Maxd(max_occ_2_dwn[i], (detection_s[j]->data[Np-1].occupancy));
				}*/
				if (i < NumOnRamp -1)
				{
					for (j=i+1;j<i+2;j++)
						max_occ_2_dwn[i]=Maxd(max_occ_2_dwn[i], (detection_s[j]->data[Np-1].occupancy));
				}
				else if (i < NumOnRamp)
				{
					for (j=i+1;j<i+1;j++)
						max_occ_2_dwn[i]=Maxd(max_occ_2_dwn[i], (detection_s[j]->data[Np-1].occupancy));
				}
				else
					max_occ_2_dwn[i]=detection_s[NumOnRamp]->data[Np-1].occupancy;
								
				for (j=i+1;j<NumOnRamp+1;j++)				
					max_occ_all_dwn[i]=Maxd(max_occ_all_dwn[i], (detection_s[j]->data[Np-1].occupancy));						
		}

	
		for (i = 0; i < num_controller_vars; i++) //num_controller_vars=5
			for (j = 0; j < 4; j++)
				fprintf(st_file_out,"%d ", controller_data3[i].metering_rate[j]); //36->50

		
		get_current_timestamp(&ts); // get current time step
		ms_now = TS_TO_MS(&ts);

		for( i = 0; i < NUM_ARTERIAL_CONTROLLERS; i++) //NUM_ARTERIAL_CONTROLLERS=14
		{
			if((arterial_desc[i].ctl_permission == CTL) &&
				(arterial_desc[i].controlling_rm_index >= 0))
			
			{
				if( clock_gettime(CLOCK_REALTIME, &curr_timespec) < 0)
					perror("urms clock_gettime");
				ltime = localtime(&curr_timespec.tv_sec);

                                // Check control periods
                                if( (ltime->tm_wday == 0) ||	//Disable control if it's Sunday, ...
                                    (ltime->tm_wday == 6) ||	//or if it's Saturday, ...
                                    (ltime->tm_hour < 7) ||	//or if it's before 7 AM, ...
                                    (ltime->tm_hour >= 10) ) 	//or if it's after 10 AM
				{	
				    control_period_disable = 1;
				}
				else
				    control_period_disable = 0;

#define FIRST_ONRAMP_INDEX	1

				if(onramp_out_f[arterial_desc[i].controlling_rm_index - FIRST_ONRAMP_INDEX].agg_occ > 40) // The FIRST_ONRAMP_INDEX comes from the calculation for onramp_out_f, 
														//which uses a 0-indexed array with NumOnRamp (==4) members. arterial_desc 
														//includes all of the ramp controllers, even the controllers that were not in the 
														//onramp_out_f calculation.
				{
					ms_diff[i] = ms_now - ms_sav[i];
					if(!global_disable && !control_period_disable && (ms_diff[i] >= CTL_INTRVL_MS)) {//For disabling of control due to startup disable or 
															// outside-of-control-period disable
						arterial_desc[i].ctl_state = CTL; 				//set control to CTL
						db_set_pattern[i].pattern = arterial_desc[i].pattern;
						db_clt_write(pclt, arterial_desc[i].db_var+1, sizeof(db_set_pattern_t), &db_set_pattern[i].pattern); 
						ramp_meter_desc[arterial_desc[i].controlling_rm_index].inter_threshold_flag = 1;
						ms_sav[i] = ms_now;
						print_timestamp(stdout, pts);
						printf("PLAN CHANGE EXECUTED for %s due to occupancy %f, which is over 40%%. Plan changed to %d\n", 
							arterial_desc[i].name,
							onramp_out_f[arterial_desc[i].controlling_rm_index-2].agg_occ,
							db_set_pattern[i].pattern
						);
						printf("PLAN CHANGE:  i=%d arterial_desc[%d].controlling_rm_index %d\n",
							i,
							i,
							arterial_desc[i].controlling_rm_index
						);
					}
				}
				else 
					if(onramp_out_f[arterial_desc[i].controlling_rm_index - FIRST_ONRAMP_INDEX ].agg_occ <= 30) // The FIRST_ONRAMP_INDEX comes from the calculation for onramp_out_f, 
														//which uses a 0-indexed array with NumOnRamp (==4) members. arterial_desc 
														//includes all of the ramp controllers, even the controllers that were not in the 
														//onramp_out_f calculation.
					{
						arterial_desc[i].ctl_state = NOCTL; 				//set control to NOCTL
						ramp_meter_desc[arterial_desc[i].controlling_rm_index].inter_threshold_flag = 0;
					}
				else {
					if(ramp_meter_desc[arterial_desc[i].controlling_rm_index].inter_threshold_flag == 1)
					{
						ms_diff[i] = ms_now - ms_sav[i];
						if(!global_disable && !control_period_disable && (ms_diff[i] > CTL_INTRVL_MS)) {//For disabling of control due to startup disable or 
																//outside-of-control-period disable
							arterial_desc[i].ctl_state = CTL; 				//set control to CTL
							db_set_pattern[i].pattern = arterial_desc[i].pattern;
							db_clt_write(pclt, arterial_desc[i].db_var+1, sizeof(db_set_pattern_t), &db_set_pattern[i].pattern); 
							ms_sav[i] = ms_now;
							print_timestamp(stdout, pts);
							printf("PLAN CHANGE EXECUTED for %s due to occupancy %f, which less than 40%%, but more than 30%%. Plan changed to %d\n", 
								arterial_desc[i].name,
								onramp_out_f[arterial_desc[i].controlling_rm_index-2].agg_occ,
								db_set_pattern[i].pattern
							);
							printf("PLAN CHANGE:  i=%d arterial_desc[%d].controlling_rm_index %d\n",
								i,
								i,
								arterial_desc[i].controlling_rm_index
							);
						}
					}
				}
			}
		
			fprintf(st_file_out,"%s %s %d %.2f %d ", 
				arterial_desc[i].name,							//62,67,72,77,82,87,92,97,102,107,112,117,122,127
				ramp_meter_desc[arterial_desc[i].controlling_rm_index].name, 		//63,68,73,78,83,88,93,98,103,108,113,118,123,128
				arterial_desc[i].ctl_state, 						//64,69,74,79,84,89,94,99,104,109,114,119,124,129
				onramp_out_f[arterial_desc[i].controlling_rm_index-2].agg_occ, 		//65,70,75,80,85,90,95,100,105,110,115,120,125,130
				db_set_pattern[i].pattern); 						//66,71,76,81,86,91,96,101,106,111,116,121,126,131

				}
				
					
		for (i = 0; i < NUM_ARTERIAL_CONTROLLERS; i++)
			print_status(NULL, st_file_out, &arterial_controllers[i], 0);
		for (i = 0; i < NUM_ARTERIAL_CONTROLLERS; i++) { //starts at 1322
		ms_diff[i] = ms_now - ms_sav[i];
		
	
		fprintf(st_file_out, "%d ", global_disable); //1322,1327,1332,1337,1342,1347,1352,1357,1362,1367,1372,1377,1382,1387
		fprintf(st_file_out, "%d ", ms_diff[i]);     //1323,1328,1333,1338,1343,1348,1353,1358,1363,1368,1373,1378,1383,1388
		fprintf(st_file_out, "%d ", control_period_disable); //1324,1329,1334,1339,1344,1349,1354,1359,1364,1369,1374,1379,1384,1389
		fprintf(st_file_out, "%#hhx ", arterial_controllers[i].preemption); //1325,1330,1335,1340,1345,1350,1355,1360,1365,1370,1375,1380,1385,1390
		fprintf(st_file_out, "%#hhx ", (arterial_controllers[i].preemption & 0x0f)); //1326,1331,1336,1341,1346,1351,1356,1361,1366,1371,1376,1381,1386,1391
	

		}

		for (i = 0; i < NUM_RAMP_CONTROLLERS; i++) //starts at 1392
			fprintf(st_file_out, "%d ", (ramp_meter_desc[arterial_desc[i].controlling_rm_index].inter_threshold_flag));

		
		/*************************************************
		   
		      XYLu code start from here
	
		**************************************************/ 
		det_data_4_contr(time);	
		get_meas(time);		
		update_q_R();
		opt_metering();
		
		//fprintf(cal_opt_f,"%lf ", time);   // Output calculated Opt RM rt
		for (i=0;i<NumOnRamp;i++)
		{				
			total_rt[i]=opt_r[i][0];							
			fprintf(cal_opt_f,"%lf ", total_rt[i]);				
		}
		fprintf(cal_opt_f,"\n");
		
/******************************************************************
		             Set CRM Rate
******************************************************************/			


for (k=0;k<NumOnRamp;k++)
	{
		if (N_OnRamp_Ln[k] == 1)
		{
			ln_CRM_rt[k][0]=total_rt[k];
			if (ln_CRM_rt[k][0] > max_Ln_RM_rt[k])
				ln_CRM_rt[k][0] = max_Ln_RM_rt[k];
			if (ln_CRM_rt[k][0] < min_Ln_RM_rt[k])
				ln_CRM_rt[k][0] = min_Ln_RM_rt[k];
		}
		if (N_OnRamp_Ln[k] == 2)
		{
			if (k != 11)			
				ln_CRM_rt[k][0]=total_rt[k]-Onramp_HOV_Util*max_RM_rt;  
			else		// No HOV lane this case
				ln_CRM_rt[k][0]=0.5*total_rt[k];  	
			if (ln_CRM_rt[k][0] > max_Ln_RM_rt[k])
				ln_CRM_rt[k][0] = max_Ln_RM_rt[k];
			if (ln_CRM_rt[k][0] < min_Ln_RM_rt[k])
				ln_CRM_rt[k][0] = min_Ln_RM_rt[k];
			
		}
		if (N_OnRamp_Ln[k] == 3)
		{				
			ln_CRM_rt[k][0]=0.5*(total_rt[k]-Onramp_HOV_Util*max_RM_rt);  
			ln_CRM_rt[k][1]=ln_CRM_rt[k][0];                                   //0.5*(total_rt[k]-Onramp_HOV_Util*max_RM_rt);  
			if (ln_CRM_rt[k][0] > max_Ln_RM_rt[k])
				ln_CRM_rt[k][0] = max_Ln_RM_rt[k];
			if (ln_CRM_rt[k][0] < min_Ln_RM_rt[k])
				ln_CRM_rt[k][0] = min_Ln_RM_rt[k];
			if (ln_CRM_rt[k][1] > max_Ln_RM_rt[k])
				ln_CRM_rt[k][1] = max_Ln_RM_rt[k];
			if (ln_CRM_rt[k][1] < min_Ln_RM_rt[k])
				ln_CRM_rt[k][1] = min_Ln_RM_rt[k];
		}	
	
//#ifdef ORIGINAL_IMP	
			if (max_occ_2_dwn[k] < Occ_Cr)
				ln_CRM_rt[k][0]=0.8*max_Ln_RM_rt[k];
			if (max_occ_all_dwn[k] < Occ_Cr)
				ln_CRM_rt[k][0]=max_Ln_RM_rt[k];
			
			if (onramp_out_f[k].agg_occ > OnRamp_Occ_Cr)
				(ln_CRM_rt[k][0])=(ln_CRM_rt[k][0])*(1.0+ 0.5*(onramp_out_f[k].agg_occ - OnRamp_Occ_Cr));
			if (ln_CRM_rt[k][0]>max_Ln_RM_rt[k])
				ln_CRM_rt[k][0]=max_Ln_RM_rt[k];

//#endif
				
		fprintf(Ln_RM_rt_f,"%10.2f ",ln_CRM_rt[k][0]);			
	}
	fprintf(Ln_RM_rt_f,"\n");	
		
/******************************************************************
		             Set LRRM Rate
******************************************************************/		

   
for(k=0;k<NumOnRamp;k++)
	{	
		for (j=0;j<N_interv-1;j++)
		{
				if (o[k]<=SR99_RM_occ_tbl[0][k])				
					total_LRRM_rt[k]=SR99_RM_rate_tbl[0][k];														
				else if (o[k]>SR99_RM_occ_tbl[j][k] && o[k]<=SR99_RM_occ_tbl[j+1][k])					
					total_LRRM_rt[k]=SR99_RM_rate_tbl[j-1][k];																
				else				
					total_LRRM_rt[k]=SR99_RM_rate_tbl[j+1][k];							
		} // for j-loop
										
		if (total_LRRM_rt[k]>max_RM_rt)
			total_LRRM_rt[k]=max_RM_rt;	
		if (total_LRRM_rt[k]<min_RM_rt)
			total_LRRM_rt[k]=min_RM_rt;	
	
		ln_LRRM_rt[k][0]=total_LRRM_rt[k];

		if (ln_LRRM_rt[k][0] > max_Ln_RM_rt[k])
			ln_LRRM_rt[k][0] = max_Ln_RM_rt[k];
		if (ln_LRRM_rt[k][0] < min_Ln_RM_rt[k])
			ln_LRRM_rt[k][0] = min_Ln_RM_rt[k];
		fprintf(local_rm_f,"%10.2f\t", (float)(total_LRRM_rt[k]));

	}
	fprintf(local_rm_f,"\n");
			

//FOR TEST PURPOSES ONLY###############################
//counter++;
//FOR TEST PURPOSES ONLY###############################

		for (l=0;l<NumOnRamp;l++)               // Lane-wise RM rate
		{
			urms_ctl[l].lane_1_action = URMS_ACTION_FIXED_RATE;
			urms_ctl[l].lane_2_action = URMS_ACTION_FIXED_RATE;
			urms_ctl[l].lane_3_action = URMS_ACTION_FIXED_RATE;
			urms_ctl[l].lane_4_action = URMS_ACTION_SKIP;

			urms_ctl[l].lane_1_release_rate = (unsigned int)ln_CRM_rt[l][0];
			urms_ctl[l].lane_2_release_rate = (unsigned int)ln_CRM_rt[l][0];
			urms_ctl[l].lane_3_release_rate = (unsigned int)ln_CRM_rt[l][0];
			urms_ctl[l].lane_4_release_rate = (unsigned int)ln_CRM_rt[l][0];
			urms_ctl[l].no_control = 0;
//FOR TEST PURPOSES ONLY###############################
//				urms_ctl[l].lane_1_release_rate = 450+i;
//				urms_ctl[l].lane_2_release_rate = 400+i;
//				urms_ctl[l].lane_3_release_rate = 350+i;
//				urms_ctl[l].lane_4_release_rate = 300+i;
//				urms_ctl[l].lane_1_action = URMS_ACTION_SKIP;
//				urms_ctl[l].lane_2_action = URMS_ACTION_SKIP;
//				urms_ctl[l].lane_3_action = URMS_ACTION_SKIP;
//				urms_ctl[l].lane_4_action = URMS_ACTION_SKIP;
//				urms_ctl[l].lane_1_plan = 1;
//				urms_ctl[l].lane_2_plan = 1;
//				urms_ctl[l].lane_3_plan = 1;
//				urms_ctl[l].lane_4_plan = 1;
			db_clt_write(pclt, metering_controller_db_vars[l], sizeof(db_urms_t), &urms_ctl[l]); 
			printf("Controller db var %d lane 1 rate %d action %d plan %d lane 2 rate %d action %d plan %d lane 3 rate %d action %d plan %d lane 4 rate %d action %d plan %d\n",
				metering_controller_db_vars[l],
				urms_ctl[l].lane_1_release_rate,
				urms_ctl[l].lane_1_action,
				urms_ctl[l].lane_1_plan,
				urms_ctl[l].lane_2_release_rate,
				urms_ctl[l].lane_2_action,
				urms_ctl[l].lane_2_plan,
				urms_ctl[l].lane_3_release_rate,
				urms_ctl[l].lane_3_action,
				urms_ctl[l].lane_3_plan,
				urms_ctl[l].lane_4_release_rate,
				urms_ctl[l].lane_4_action,
				urms_ctl[l].lane_4_plan
			);
			fprintf(st_file_out, " %d %d %d %d %d %d %d %d %d %d %d %d ",
				urms_ctl[l].lane_1_release_rate,
				urms_ctl[l].lane_1_action, 	//1398
				urms_ctl[l].lane_1_plan, 	//1399
				urms_ctl[l].lane_2_release_rate,//1400
				urms_ctl[l].lane_2_action,	//1401
				urms_ctl[l].lane_2_plan,	//1402
				urms_ctl[l].lane_3_release_rate,//1403
				urms_ctl[l].lane_3_action,	//1404
				urms_ctl[l].lane_3_plan,	//1405
				urms_ctl[l].lane_4_release_rate,//1406
				urms_ctl[l].lane_4_action,	//1407
				urms_ctl[l].lane_4_plan		//1408
			);
//FOR TEST PURPOSES ONLY###############################
		}
		for (l=0;l<NumOnRamp;l++)               // Lane-wise RM rate
			fprintf(st_file_out, " %f ",
			ln_CRM_rt[l][0]
			);
				
		fprintf(st_file_out,"\n");
		fflush(st_file_out);
		//cycle_index++;
		//cycle_index %= NUM_CYCLE_BUFFS;
	
			TIMER_WAIT(ptimer);	
	} 
	
	Finish_sim_data_io();
	      	
	return 0;
}



int Init_sim_data_io()
{	
	st_file=fopen("In_Data/state_var.txt","r");	
	dbg_f=fopen("Out_Data/dbg_file.txt","w");
	local_rm_f=fopen("Out_Data/local_rm_rate.txt","w");
	cal_opt_f=fopen("Out_Data/cal_opt_RT_rt.txt","w");
	st_file_out=fopen("Out_Data/state_var_out.txt","w");	
	st_file_new=fopen("Out_Data/state_var_new.txt","w");	
	dbg_st_file_out =fopen("Out_Data/dbg_state_var_out.txt","w");	
	Ln_RM_rt_f=fopen("Out_Data/lanewise_rt.txt","w");	
	pp=fopen("Out_Data/coeff.txt","w");
//	dbg_f=fopen("/linux2/big/data/SANJOSE_ACRM/Out_Data/dbg_file.txt","w");
//	local_rm_f=fopen("/linux2/big/data/SANJOSE_ACRM/Out_Data/local_rm_rate.txt","w");
//	cal_opt_f=fopen("/linux2/big/data/SANJOSE_ACRM/Out_Data/cal_opt_RT_rt.txt","w");
//	st_file_out=fopen("/linux2/big/data/SANJOSE_ACRM/Out_Data/state_var_out.txt","w");	
//	dbg_st_file_out =fopen("/linux2/big/data/SANJOSE_ACRM/Out_Data/dbg_state_var_out.txt","w");	
//	Ln_RM_rt_f=fopen("/linux2/big/data/SANJOSE_ACRM/Out_Data/lanewise_rt.txt","w");	
//	pp=fopen("/linux2/big/data/SANJOSE_ACRM/Out_Data/coeff.txt","w");
	return 1;
}

/***************************************************

   Subrutines of XYLu needs updating  from here;   09_15_16
   
****************************************************/

int Init()  // A major function; Called by AAPI.cxx: the top function for intialization of the whole system
{
	int i; 
	
	// Memory set for variables
	/*for(i=0;i<SecSize;i++)
	{
		u[i]=89.0; 		
	}
	for(i=0;i<SecSize;i++)
	{
		v[i]=0.0;
		q[i]=0.0;
		o[i]=0.0;
	}*/

	memset(u,0,SecSize*(sizeof(float))); 
	memset(v,0,SecSize*(sizeof(float))); 
	memset(q,0,SecSize*(sizeof(float))); 
	memset(o,0,SecSize*(sizeof(float))); 
	memset(qc,0,SecSize*(sizeof(float)));
	memset(queue,0,SecSize*(sizeof(float))); 
	memset(s,0,NumOnRamp*(sizeof(float)));
	memset(R,0,NumOnRamp*(sizeof(float)));
	memset(dmd,0,NumOnRamp*(sizeof(float)));
	memset(Q_o,0,NumOnRamp*(sizeof(float)));
	memset(Q_min,0,NumOnRamp*(sizeof(float)));
	memset(ss,0,Np*NumOnRamp*(sizeof(float)));
	memset(dd,0,Np*NumOnRamp*(sizeof(float)));
	memset(pre_w,0,NumOnRamp*(sizeof(float)));
	memset(max_occ_all_dwn,0,NumOnRamp*(sizeof(float)));
	memset(max_occ_2_dwn,0,NumOnRamp*(sizeof(float)));
	memset(q_main,0,SecSize*(sizeof(float)));
	memset(u2,0,SecSize*(sizeof(float))); 
	memset(pre_rho,0,SecSize*(sizeof(float)));
	memset(opt_r,0,SecSize*(sizeof(float)));
	memset(up_rho,0,Np*sizeof(float));
	memset(dyna_min_r, 0, NumOnRamp*(sizeof(float))); 
	memset(dyna_max_r, 0, NumOnRamp*(sizeof(float))); 
	memset(Ramp_rt, 0, NumOnRamp*(sizeof(float))); 
	memset(RM_occ, 0, NumOnRamp*(sizeof(float))); 
	memset(ln_CRM_rt, 0, NumOnRamp*max_onramp_ln*(sizeof(float))); 
	memset(ln_LRRM_rt, 0, NumOnRamp*max_onramp_ln*(sizeof(float))); 
	memset(release_cycle, 0, NumOnRamp*max_onramp_ln*(sizeof(int))); 
	memset(total_rt, 0, NumOnRamp*(sizeof(float))); 
	memset(total_LRRM_rt, 0, NumOnRamp*(sizeof(float))); 
	memset(detection_s_0,0,SecSize*sizeof(detData));
	memset(detection_onramp_0,0,NumOnRamp*sizeof(detData));
	memset(detection_offramp_0,0,NumOnRamp*sizeof(detData));
	memset(a_w,0,NumOnRamp*sizeof(float));
	
	
	 
	
	for(i=0; i<SecSize;i++)
		detection_s[i] = &(detection_s_0[i]);	
	for(i=0; i<NumOnRamp;i++)
	{
		detection_onramp[i] = &(detection_onramp_0[i]);	
		detection_offramp[i] = &(detection_offramp_0[i]);	
	}
	
	for(i=0;i<NumOnRamp;i++)
	{
		dmd[i]=0.0;
		dyna_min_r[i]=300.0*N_OnRamp_Ln[i];
		dyna_max_r[i]=dyna_min_r[i]+rm_band;	
		Ramp_rt[i]=max_RM_rt;   // only used in Set_Coord_ALINEA
		//Q_o[i]=max_RM_rt*N_OnRamp_Ln[i];   // changed on 09_15_16
		
		if(N_OnRamp_Ln[i] == 1)
			Q_o[i]=max_RM_rt;
		else if (N_OnRamp_Ln[i] == 2)
			Q_o[i]=max_RM_rt*(1.0+Onramp_HOV_Util);
		else
			Q_o[i]=max_RM_rt*(N_OnRamp_Ln[i]-1+Onramp_HOV_Util);
		Q_min[i]=min_RM_rt*N_OnRamp_Ln[i];		
		a_w[i]=6.5;
		onrampL[i]=onrampL[i]/1609.0;		
	}	
	
	for(i=0; i<SecSize;i++)
		L[i]=L[i]/1609.0;
	
	u[NumOnRamp]=104;   // Dim Ok	
	ControlOn=0;
	StateOn=0;
	StateOff=0;
	
	for(i=0;i<SecSize;i++)
		qc[i]=Q_ln*lambda[i];
	
	return 1;
}

///////////////////////////////////////////////////////////////////////////////////////////////

int moveData(detData* detection)
{
	int k;
	for(k=0;k<Np-1;k++)
	{
		//detection->data[k]=detection->data[k+1];
		detection->data[k].DataTime=detection->data[k+1].DataTime;
		detection->data[k].density=detection->data[k+1].density;
		detection->data[k].flow=detection->data[k+1].flow;
		detection->data[k].instant_density=detection->data[k+1].instant_density;
		detection->data[k].instant_flow=detection->data[k+1].instant_flow;
		detection->data[k].instant_speed=detection->data[k+1].instant_speed;
		detection->data[k].occupancy=detection->data[k+1].occupancy;
		detection->data[k].speed=detection->data[k+1].speed;
		detection->data[k].vehType=detection->data[k+1].vehType;
	}
	return 1;
}


int update_queue(float time)  // This will not be used since we have onramp length available;
{
	int i,j;
	float w;
//	StructAkiEstadSection onramp_info;

	//for(i=0;i<SecSize;i++)
	for(i=0;i<NumOnRamp;i++)
	{
		w=0;
		for(j=0;j<max_onramp_ln;j++)
		{
			
			/*if(OnRamp_Ln_Id[i][j]>0)
			{
				onramp_info=AKIEstGetParcialStatisticsSection(OnRamp_Ln_Id[i][j],time,0);
				if(onramp_info.report!=0)
					AKIPrintString("read section stat error");
				else
					w=Maxd(w,onramp_info.LongQueueAvg);  // use the max queue of the lane as onramp queue
			}
			*/
		}
		queue[i]=w;
	}

	return 1;
}


int det_data_4_contr(float time) // not used anymore
{
	int i,j;
	
	//update_queue(time);
	
	for(i=0;i<NumOnRamp;i++)
	{
		pre_w[i]=(float)queue[i];
	}

	for(i=0;i<NumOnRamp;i++) // est demnd, odd-ramp flow and queue
	{
		for(j=0;j<Np;j++)
		{				
			dd[i][j]=(float)(detection_onramp[i]->data[Np-1].flow);
			//if (i==0 || i==5 || i==6 || i==7 || i==8)						
			dd[i][j]=(dd[i][j])*(1.0+dmd_change);			
		}
		dmd[i]=(float)(detection_onramp[i]->data[Np-1].flow);
		
		for(j=0;j<Np;j++)	
			ss[i][j]=detection_offramp[i]->data[Np-1].flow; // use current step
			
		pre_w[i]=pre_w[i]+T*(dd[i][0]-opt_r[i][0]);
	}
	

	for(j=0;j<Np;j++)		
		up_rho[j]=(float)((detection_s[0]->data[Np-1].density));		
	for(i=0;i<NumOnRamp;i++)		
		pre_rho[i]=(float)((detection_s[i+1]->data[Np-1].density));  // exclude the most upstream
	
	return 1;
}

int update_q_R()       // update flow for each cell
{
	int i;
	
	for(i=1;i<=NumOnRamp;i++)
	{		
		R[i-1]=get_min(dmd[i-1],get_min(Q_o[i-1],qc[i]-q_main[i-1]));                                     // use filtered onramp flow
		q[i]=q_main[i]+R[i-1]-ss[i-1][Np-1];                       // flow of currect cell          // index of R changed;  01/03/13		
	}
	q[0]=q_main[0];
	
	return 1;
}



int get_meas(float T)
{
	int i;


	for(i=0;i<SecSize;i++)
	{
		
		v[i]=exp_flt*(detection_s[i]->data[Np-1].speed)+(1.0-exp_flt)*v[i];
		o[i]=exp_flt*(detection_s[i]->data[Np-1].occupancy)+(1.0-exp_flt)*o[i];
		q_main[i]=exp_flt*(detection_s[i]->data[Np-1].flow)+(1.0-exp_flt)*q_main[i]; // use upastream section flow for MPC
		u2[i]=(detection_s[i]->data[Np-1].speed)*1609.0/3600.0;
	}
	
	for(i=0;i<NumOnRamp;i++)
	{
		if (detection_offramp[i]->detId > 0)					
			s[i]=exp_flt*(detection_offramp[i]->data[Np-1].flow)+(1-exp_flt)*s[i]; // changed on 03/03/14
			//s[i]=detection_offramp[i]->data[Np-1].flow;
		else
			s[i]=0.0;
	}
	
	return 1;
}


/*******************************************
               OptSolver
********************************************/

int set_coef(float c[MP][NP],float Qm)
{
	

	float w_const[SecSize][Np]={{0.0}}; 					// Used to construct b_u; 
	float p_const[SecSize][Np]={{0.0}};					// Used to construct b_u; 	
	float f[(NumOnRamp)*Np]={0.0};
	
	float b_u[M1]={0.0};
	float b_l[M2]={0.0};   // upper bound of r

	int m,i,j;
	static unsigned short memset_sw=1;
	
	if (memset_sw==1)
	{
	memset(&w_const,0,sizeof(w_const));
	memset(&p_const,0,sizeof(p_const));
	memset(&f,0,sizeof(f));	
	memset(&b_u,0,sizeof(b_u));
	memset(&b_l,0,sizeof(b_l));	
	memset_sw=0;
	}

	


	for(m=0;m<NumOnRamp;m++)
	{
		if (m==0)
		{
			p_const[m][0]=(1-T*u2[0]/L[m])*up_rho[1]+T/(L[m]*lambda[m+1])*opt_r[m][0];
			p_const[m][1]=(1-T*u2[0]/L[m])*p_const[m][0]+(T*u2[m]/L[m])*0;
			w_const[m][0]=pre_w[m]+T*dd[m][0];
			w_const[m][1]=w_const[m][0]+T*dd[m][1];	
		}
		else
		{
			p_const[m][0]=(1-T*u2[m+1]/L[m])*pre_rho[m]+T/(L[m]*lambda[m+1])*opt_r[m][0];
			p_const[m][1]=(1-T*u2[m+1]/L[m])*p_const[m][0]+(T*u2[m+1]/L[m])*p_const[m-1][0]-T/(L[m]*lambda[m+1])*ss[m][Np-1];
			w_const[m][0]=pre_w[m]+T*dd[m][0];
			w_const[m][1]=w_const[m][0]+T*dd[m][Np-1];				
		}
	}
	

//obj fnc
	for(m=0;m<(NumOnRamp);m++)
	{
		if(m==0)
		{			
			f[m]=(float)( (2-T*u2[m+1]/L[m])*T  -2*a_w[m]*T +a_ttd*(2-T*u2[m+1]/L[m])*T*u2[m+1]);
			f[NumOnRamp+m]=(float)(T-T*a_w[m]+a_ttd*T*u2[m+1]);			
		}
		if (m<NumOnRamp-1)
		{
			f[m]=(float)( (2-T*u2[m+1]/L[m])*T + (lambda[m+1]*T*T*u2[m]/(lambda[m]*L[m])) + (lambda[m+1]*T*T*u2[m]*u2[m+1])/(lambda[m]*L[m])-2*a_w[m]*T +a_ttd*(2-T*u2[m+1]/L[m])*T*u2[m+1]);
			f[NumOnRamp+m]=(float)(T-T*a_w[m]+a_ttd*T*u2[m+1]);
		}
		if (m==NumOnRamp-1)
		{
			f[m]=(float)( (lambda[m+1]*T*T*u2[m]/(lambda[m]*L[m])) - 2*a_w[m]*T );
			f[NumOnRamp+m]=(float)(T-T*a_w[m]+a_ttd*T*u2[m+1]);
		}
	}

	// lower and upper bounds
	
	for(m=0;m<NumOnRamp;m++)
	{		
		b_u[m]=Rou_J-p_const[m][0];
		b_u[NumOnRamp+m]=Rou_J-p_const[m][1];
		b_u[2*NumOnRamp+m]=pre_w[m]+dd[m][0];
		b_u[3*NumOnRamp+m]=pre_w[m]+T*dd[m][0]+T*dd[m][1];
		//b_u[4*NumOnRamp+m]=Mins(dd[m][1],Q_o[m]);
		b_u[4*NumOnRamp+m]=Mins(dd[m][1],R[m]);
		b_u[5*NumOnRamp+m]=Mins(Q_o[m],dd[m][0]);
		//b_u[5*NumOnRamp+m]=R[m];
	
		
		b_l[m]=pre_w[m]+T*dd[m][0]-onrampL[m]*Rou_J;
		b_l[NumOnRamp+m]=pre_w[m]+T*dd[m][0]+T*dd[m][1]-onrampL[m]*Rou_J;
	}
	
	for(m=0;m<M1;m++)	
	{
		//if (b_u[m] < 0.0)
		if (b_u[m] < Q_min[m])
			b_u[m]=Maxs(b_u[m],Mins(Maxs(dd[m][1],800),Q_o[m]));
			//b_u[m]=Maxs(b_u[m],500.0);
	}	
			
	for(m=0;m<NumOnRamp;m++)
	{	
		b_l[m]=Maxs(b_l[m],Q_min[m]);
		b_l[NumOnRamp+m]=Maxs(b_l[NumOnRamp+m],Q_min[m]);
		//b_l[m]=Maxs(b_l[m],0.0);
		//b_l[NumOnRamp+m]=Maxs(b_l[NumOnRamp+m],0.0);
	}

	// Assign f to Matrix c
	for(j=0;j<2*NumOnRamp;j++)	
		c[0][j+1]=f[j];			
	
	// Assign Upper &  lower Bounds to Matrix c
	for(i=1;i<=M1;i++)
		c[i][0]=b_u[i-1];		
	for(i=1;i<=M2;i++)
		c[10*NumOnRamp+i][0]=b_l[i-1];		

	// Matrix c
	for(i=1;i<=NumOnRamp;i++)
	{
		for(j=1;j<=NumOnRamp;j++)	
			c[i][j]=T/(L[j]*lambda[j]);
			
		if (i==1)
		{
			for(j=1;j<=NumOnRamp;j++)
			{		
				c[NumOnRamp+i][j]=(1-T*u2[j]/L[j])*T/(L[j]*lambda[j]);
				c[NumOnRamp+i][NumOnRamp+j]=T/(L[j]*lambda[j]);
			}		
		}
		else
		{
			for (j=1;j<=NumOnRamp;j++)
			{
				if (j==1)
				{					
					c[NumOnRamp+i][j]=(1-T*u2[j]/L[j])*T/(L[j]*lambda[j]);
					c[NumOnRamp+i][NumOnRamp+j]=T/(L[j]*lambda[j]);
				}
				else
				{
					c[NumOnRamp+i][j-1]=T*T*u2[j]/(L[j]*L[j-1]*lambda[j-1]);
					c[NumOnRamp+i][j]=(1-T*u2[j]/L[j])*T/(L[j]*lambda[j]);
					c[NumOnRamp+i][NumOnRamp+j]=T/(L[j]*lambda[j]);
				}				
			}
		}
		
		for(j=1;j<=NumOnRamp;j++)	
			c[2*NumOnRamp+i][j]=T;
		
		for(j=1;j<=NumOnRamp;j++)	
		{
			c[3*NumOnRamp+i][j]=T;
			c[3*NumOnRamp+i][NumOnRamp+j]=T;
		}
		
		for(j=1;j<=NumOnRamp;j++)	
			c[4*NumOnRamp+i][j]=1;
	
		for(j=1;j<=NumOnRamp;j++)	
			c[5*NumOnRamp+i][NumOnRamp+j]=1;
	}	// Matrix c: i-loop end	

	// The sign of the coefficient matrix sould be reversed; ecept the coeff of objective function (1st row) and b_u, b_l (1st column)
	for (i=1;i<MP;i++)
	{
		for (m=1;m<NP;m++)
			c[i][m]=-c[i][m];
	}
	
#ifdef COEFF_DATA
	sprintf(str,"up_rho:");
	fprintf(pp,"%s\n",str);	
	for(m=0;m<Np;m++)
	{
		fprintf(pp,"%lf ",up_rho[m]);		
	}
	fprintf(pp,"\n");	

	sprintf(str,"pre_rho:");
	fprintf(pp,"%s\n",str);
	for(m=0;m<SecSize;m++)
	{
		fprintf(pp,"%lf ",pre_rho[m]);		
	}
	fprintf(pp,"\n");
	
	
	sprintf(str,"u2:");
	fprintf(pp,"%s\n",str);
	
	for(m=0;m<SecSize;m++)
	{
		fprintf(pp,"%lf ",u2[m]);		
	}	
	fprintf(pp,"\n");
	

	sprintf(str,"pre_w:");
	fprintf(pp,"%s\n",str);
	for(m=0;m<NumOnRamp;m++)//NumOnRamp=4
	{
		fprintf(pp,"%lf ",pre_w[m]);		
	}
	fprintf(pp,"\n");

	//sprintf(str,"Q_o:");
	/*fprintf(pp,"Q_o=:\n");
	for(m=0;m<NumOnRamp;m++)//NumOnRamp=4
	{
		fprintf(pp,"%lf ",Q_o[m]);		
	}
	fprintf(pp,"\n") */
	

	sprintf(str,"Onramp Length:");
	fprintf(pp,"%s\n",str);
	for(m=0;m<NumOnRamp;m++)//NumOnRamp=4
	{
		fprintf(pp,"%lf ",onrampL[m]);		
	}
	fprintf(pp,"\n");	

	
	sprintf(str,"dd:");
	fprintf(pp,"%s\n",str);
	for(m=0;m<NumOnRamp;m++)//NumOnRamp=4
	{
		for(j=0;j<Np;j++)
		{
			fprintf(pp,"%lf ",dd[m][j]);			
		}
		fprintf(pp,"\n");		
	}
	
	sprintf(str,"ss:");
	fprintf(pp,"%s\n",str);
	for(m=0;m<NumOnRamp;m++)//NumOnRamp=4
	{
		for(j=0;j<Np;j++)
		{
			fprintf(pp,"%lf ",ss[m][j]);			
		}
		fprintf(pp,"\n");		
	}

	sprintf(str,"w_const:");
	fprintf(pp,"%s\n",str);
	for(m=0;m<SecSize;m++)
	{
		for(j=0;j<Np;j++)
		{
			fprintf(pp,"%lf ",w_const[m][j]);			
		}
		fprintf(pp,"\n");		
	}
	
		
	sprintf(str,"p_const:");
	fprintf(pp,"%s\n",str);
	for(m=0;m<SecSize;m++)
	{
		for(j=0;j<Np;j++)
		{
			fprintf(pp,"%lf ",p_const[m][j]);			
		}
		fprintf(pp,"\n");		
	}
	
	
	sprintf(str,"f:");
	fprintf(pp,"%s\n",str);
	for(m=0;m<(NumOnRamp)*Np;m++)//NumOnRamp=4
	{
		fprintf(pp,"%f ",f[m]);		
	}
	fprintf(pp,"\n");	
	

	sprintf(str,"b_u:");
	fprintf(pp,"%s\n",str);
	//for(m=0;m<M1;m++)
	//{
	//	fprintf(pp,"%f ",b_u[m]);		
	//}
	for(i=0;i<10;i++)
	{
		for(m=0;m<NumOnRamp;m++)//NumOnRamp=4
			fprintf(pp,"%f ",b_u[NumOnRamp*i+m]);	
		fprintf(pp,"\n");	
	}
	

	sprintf(str,"b_l:");
	fprintf(pp,"%s\n",str);
	for(m=0;m<M2;m++)
	{
		fprintf(pp,"%f ",b_l[m]);		
	}
	fprintf(pp,"\n");
	


/*sprintf(str,"M1 & M2:");
	fprintf(pp,"%s\n",str);
	fprintf(pp,"%i %i ",M1, M2); //b_l[m]);		
	fprintf(pp,"\n");	
*/
	sprintf(str,"c:");
	fprintf(pp,"%s\n",str);
	for(m=0;m<MP;m++)
	{
		for(j=0;j<NP;j++)
		{
			fprintf(pp,"%lf ",c[m][j]);			
		}
		fprintf(pp,"\n");		
	}
#endif
	
	return 0;
}

int opt_metering(void)
{
	int i,icase,*izrov,*iposv;

	float cc[MP][NP]={{0.0}};
	
	float **a;

	memset(&cc,0,sizeof(cc));
		
	set_coef(cc, Q_ln);   // previous Q_ln=1800
	izrov=ivector(1,N);
	iposv=ivector(1,M);
	a=convert_matrix(&cc[0][0],1,MP,1,NP);
	simplx(a,M,N,M1,M2,M3,&icase,izrov,iposv);
	
	/*for (i=1;i<=M;i++)
		fprintf(dbg_f,"%d ",iposv[i]);	
	fprintf(dbg_f,"\n");
	*/
	/*for (i=1;i<=N;i++)
		fprintf(dbg_f,"%d ",izrov[i]);	
	fprintf(dbg_f,"\n");*/
	
	if (icase == 0)
	{	
	
			for(i=1;i<=NumOnRamp;i++)//NumOnRamp=4
				{	
					opt_r[i-1][0]=a[iposv[i+1]][1];							
					//fprintf(dbg_f,"i=%d, icase=%d iposv=%d, r=%10.2f\n",i,icase, iposv[i],opt_r[i][0]);	
				}	
			for(i=NumOnRamp+1;i<=2*NumOnRamp;i++)	//NumOnRamp=4
				{
					opt_r[i-1-NumOnRamp][1]=a[iposv[i+1]][1];								
					//fprintf(dbg_f,"i=%d, icase=%d iposv=%d, r=%10.2f\n",i,icase, iposv[i],opt_r[i-1-NumOnRamp][1]);					
				}	
	}
	else;
		//fprintf(dbg_f,"i=%d, icase=%d\n",i,icase);		
	
	free_convert_matrix(a,1,MP,1,NP);
	free_ivector(iposv,1,M);
	free_ivector(izrov,1,N);

	return 0;
}
#undef NRANSI

/*******************************************
               OptSolver End
********************************************/


int Finish_sim_data_io()
{
	fflush(dbg_f);
	fclose(dbg_f);
	fflush(local_rm_f);
	fclose(local_rm_f);	
	fflush(cal_opt_f);
	fclose(cal_opt_f);
	fflush(Ln_RM_rt_f);
	fclose(Ln_RM_rt_f);
	//fflush(sec_outfile);
	//fclose(sec_outfile);
	if (use_CRM == 2)
	{
		fflush(pp);
		fclose(pp);
	}
	fflush(st_file);
	fclose(st_file);
	fflush(st_file_out);
	fclose(st_file_out);
	fflush(st_file_new);
	fclose(st_file_new);



	return 1;
}


float Mind(float a,float b)
{
	if(a<=b)
		return a;
	else
		return b;
}
float Maxd(float a,float b)
{
	if(a>=b)
		return a;
	else
		return b;
}
float get_min(float a, float b)
{
	if(a<=b)
		return a;
	else
		return b;
}
float Mins(float a,float b)
{
	if(a<=b)
		return a;
	else
		return b;
}
float Maxs(float a,float b)
{
	if(a>=b)
		return a;
	else
		return b;
}
