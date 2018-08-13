/* ac_rm_algo.h - structs for inputting controller status data to the
	control algorithm, and outputting changed green times, via the
	database
*/

#pragma once
#include <db_include.h>
#include "variables2.h"

#define DB_ALGO_INPUT_TYPE	2001
#define	DB_ALGO_INPUT_VAR	DB_ALGO_INPUT_TYPE
#define DB_ALGO_OUTPUT_TYPE	2002
#define	DB_ALGO_OUTPUT_VAR	DB_ALGO_OUTPUT_TYPE
#define DB_RAMP_DATA_TYPE	5700
#define DB_RAMP_DATA_VAR	DB_RAMP_DATA_TYPE
#define DB_AGG_DATA_TYPE	5701
#define DB_AGG_DATA_VAR		DB_AGG_DATA_TYPE

typedef struct {
	int num_phases; // Number of phases used in calculation
	float cycle_time;
	float demand;
	float queue;
	float saturation_flow;
	float gmin;
	float beta;
	float weight;
	float ra;
} algo_input_t;

typedef struct {
	float g_opt;
} algo_output_t;

typedef struct
{
        float mainline_lead_occ[NUMBER_MAINLINE_LANE];
        float mainline_trail_occ[NUMBER_MAINLINE_LANE];
        float queue_occ[NUMBER_METERED_LANE];
        float meter_rate[NUMBER_METERED_LANE];
        float data_time;
        float prev_update_data;

        float mainline_avg_occupancy[NUMBER_METERED_LANE];
        int mainline_avg_volume[NUMBER_METERED_LANE];
        float prev_occ_out;
        float new_meter_rate;
        unsigned char mainline_lead_status[NUMBER_MAINLINE_LANE];
        unsigned char mainline_trail_status[NUMBER_MAINLINE_LANE];
        unsigned char mainline_lead_vol[NUMBER_MAINLINE_LANE];
        unsigned char mainline_trail_vol[NUMBER_MAINLINE_LANE];
        unsigned char passage_vol[NUMBER_METERED_LANE];
        unsigned char demand_vol[NUMBER_METERED_LANE];
        unsigned char queue_vol[NUMBER_METERED_LANE];

} IS_PACKED db_ramp_data_t;

typedef struct {
	    float agg_mean_speed;  // mean speed
        float agg_speed;       // harmonic mean speed 
        float agg_occ;
        float agg_vol;
        char  agg_stat;
        float agg_queue_length;
        float agg_turning_ratio;
        float agg_density;
	    float turning_ratio;
	    int   flag;
} IS_PACKED agg_data_t;