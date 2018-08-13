#ifndef		_VARIABLES2_H_
#define		_VARIABLES2_H_

#ifndef _VARIABLES2_H_DECLS
#define _DECL extern
#else
#define _DECL
#endif

typedef unsigned int boolean;
#define true 1
#define false 0

#undef USE_CONFIG_FILE
#ifndef USE_CONFIG_FILE 

//intersection parameters
#define MAX_GREEN_UPPERBOUND 30
#define MAX_GREEN_LOWERBOUND 15
#define RAMP_LIMIT 76.0 
#define LT_OCC_THRESHOLD 0.15 
#define MAX_GREEN_CHANGE 2 
#define RT_RELEASE_OCC_THRESHOLD 0.35 
#define MAX_STEPS 100 //max steps (we think) a vehicle needs to pass a detector
#define THRESHOLD_OCC_HEALTH 0.05 //threshold, if value changes are less then this ( 0.1 means 10% ) percent of values in previous cycles, and this keeps for many cycles, we think detector data has problem
#define THRESHOLD_VOL_HEALTH 0.1
#define CYCLE_TO_ACTIVATE_OVERWRITE 3	//within cycle_to_release, how many cycles RT_occ needs to exceed threshold for activating overwrite release
#define CYCLE_TO_RELEASE 3	//how many cycles before a right-turn release happens
#define REGULAR_CYCLE 1
#define OVERWRITE_CYCLE	1	//after regular release cycles, how many cycles more a overwrite release should have
#define QUEUE_OCC_THRESHOLD_TO_ACTIVATE_INTERSECTION 30
#define LEFT_OCC_THRESHOLD_TO_ACTIVATE_INTERSECTION 0.3
#define RIGHT_OCC_THRESHOLD_TO_ACTIVATE_INTERSECTION 0.3
#define METHOD_FOR_RAMP_QUEUE 3	//the way to get ramp queue flag, 1: via queue estimation from in/out flow 2: queue detector 2 occupancy 3: left and right turn occupancy

//ramp parameters
#define SHOCH_WAVE_SPEED -15.0
#define VEHICLE_EFFECTIVE_LENGTH 0.01
#define ALINEA_GAMMA 1.0
#define ALPHA 1.0
#define ALINEA_LOWER_BOUND 600.0
#define ALINEA_UPPER_BOUND 900.0
#define QUEUE_RESET false
#define QUEUE_RESET_INTERVAL 5*60
#define RAMP_EXIT_FLOW_ADJUST_FACTOR 0.92
#define RAMP_METER_CHANGE_THRESHOLD 0.1
#define PLANTIME1 (17.0f*3600.0f+15.0f*60.0f)
#define PLANTIME2 (17.0f*3600.0f+45.0f*60.0f)
#define METER_LOWER_BOUND1 720.0f
#define METER_LOWER_BOUND2 660.0f
#define METER_LOWER_BOUND3 600.0f


//ramp parameters, ALINEA algorithm
#define OCC_CRITICAL 8.0
#define IN_LANES 2.0
#define OUT_LANES 2.0
#define ALINEA_KR 40.0

#else

//intersection parameters
extern int MAX_GREEN_UPPERBOUND;
extern int MAX_GREEN_LOWERBOUND;
extern float RAMP_LIMIT;
extern float LT_OCC_THRESHOLD;
extern int MAX_GREEN_CHANGE;
extern float RT_RELEASE_OCC_THRESHOLD;
extern float MAX_STEPS; //max steps (we think) a vehicle needs to pass a detector
extern float THRESHOLD_OCC_HEALTH; //threshold, if value changes are less then this ( 0.1 means 10% ) percent of values in previous cycles, and this keeps for many cycles, we think detector data has problem
extern float THRESHOLD_VOL_HEALTH;
extern int CYCLE_TO_ACTIVATE_OVERWRITE; //within cycle_to_release, how many cycles RT_occ needs to exceed threshold for activating overwrite release;
extern int CYCLE_TO_RELEASE; //how many cycles before a right-turn release happens;
extern int REGULAR_CYCLE;
extern int OVERWRITE_CYCLE; //after regular release cycles, how many cycles more a overwrite release should have;
extern float QUEUE_OCC_THRESHOLD_TO_ACTIVATE_INTERSECTION;
extern float LEFT_OCC_THRESHOLD_TO_ACTIVATE_INTERSECTION;
extern float RIGHT_OCC_THRESHOLD_TO_ACTIVATE_INTERSECTION;
extern int METHOD_FOR_RAMP_QUEUE; //the way to get ramp queue flag, 1: via queue estimation from in/out flow 2: queue detector 2 occupancy 3: left and right turn occupancy;

//ramp parameters;
extern float SHOCH_WAVE_SPEED;
extern float VEHICLE_EFFECTIVE_LENGTH;
extern float ALINEA_GAMMA;
extern float ALPHA;
extern float ALINEA_LOWER_BOUND;
extern float ALINEA_UPPER_BOUND;
extern int QUEUE_RESET;
extern float QUEUE_RESET_INTERVAL;
extern float RAMP_EXIT_FLOW_ADJUST_FACTOR;
extern float RAMP_METER_CHANGE_THRESHOLD;
extern float PLANTIME1;
extern float PLANTIME2;
extern float METER_LOWER_BOUND1;
extern float METER_LOWER_BOUND2;
extern float METER_LOWER_BOUND3;


//ramp parameters, ALINEA algorithm;
extern float OCC_CRITICAL;
extern float IN_LANES;
extern float OUT_LANES;
extern float ALINEA_KR;

#endif

#define RAMP_QUEUE_THRESHOLD RAMP_LIMIT/2
#define RAMP_QUEUE_DELTA 10.0
//
//#define RTQUEUE_THRESHOLD 20.0
#define RT_EXCEED_THRESHOLD 3

#define MAXROW 6000
#define MAXCOL 10

#define PHASE5SIGNAL 0
#define PHASE5APPROACH1 1
#define PHASE5APPROACH2 2
#define PHASE5STOPBAR 3
#define PHASE8SIGNAL 4
#define PHASE8APPROACH 5
#define PHASE8STOPBAR 6
#define PHASE6SIGNAL 7
#define PHASE7SIGNAL 8
#define PHASE3SIGNAL 9



#define RAMP_DATA_INTERVAL 30.0	//ramp data update interval
#define MAINLINE_DETECTOR_WORKING 2 //mainline detector status for "working"
#define MAINLINE_DETECTOR_DISABLE 1 //mainline detector status for "disable"
#define NUMBER_METERED_LANE 3	//number of metered lane, including HOV
#define NUMBER_MAINLINE_LANE 3	//number of mainline lane, including HOV
#define NUMBER_RAMP_DATA 6 //maximum pieces of ramp data kept
#define EPS 1e-3


_DECL struct signal_variables
{
	float LT_occ, old_LT_occ, RT_occ;
	float ramp_queue, old_ramp_queue;
	int RT_exceed_num;
	int new_max_green;
	int old_max_green;
	int last_sent_max_green;
	int realtime_data[MAXROW][MAXCOL];
	int data_row;
	boolean regular_release,overwrite_release;
	int regular_remain_cycle;
	int overwrite_remain_cycle;
	int cycle_passed;

	float prev_cycle_terminate_time;
	float current_cycle_terminate_time;
	float prev_queue_reset_time;

} signal_data;

_DECL struct ramp_variables
{
	int mainline_lead_status[NUMBER_RAMP_DATA][NUMBER_MAINLINE_LANE];
	int mainline_trail_status[NUMBER_RAMP_DATA][NUMBER_MAINLINE_LANE];
	int mainline_lead_vol[NUMBER_RAMP_DATA][NUMBER_MAINLINE_LANE];
	int mainline_trail_vol[NUMBER_RAMP_DATA][NUMBER_MAINLINE_LANE];
	float mainline_lead_occ[NUMBER_RAMP_DATA][NUMBER_MAINLINE_LANE];
	float mainline_trail_occ[NUMBER_RAMP_DATA][NUMBER_MAINLINE_LANE];
	int passage_vol[NUMBER_RAMP_DATA][NUMBER_METERED_LANE];
	int demand_vol[NUMBER_RAMP_DATA][NUMBER_METERED_LANE];
	int queue_vol[NUMBER_RAMP_DATA][NUMBER_METERED_LANE];
	float queue_occ[NUMBER_RAMP_DATA][NUMBER_METERED_LANE];
	float meter_rate[NUMBER_RAMP_DATA][NUMBER_METERED_LANE];
	float data_time[NUMBER_RAMP_DATA];
	float prev_update_data;

	float mainline_avg_occupancy[NUMBER_METERED_LANE];
	int mainline_avg_volume[NUMBER_METERED_LANE];
	float prev_occ_out;
	float new_meter_rate;


} ramp_data;



#endif
