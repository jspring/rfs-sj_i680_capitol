//{{NO_DEPENDENCIES}}
// Microsoft Visual C++ generated include file.
// Used by app.rc

#pragma once
#include <urms.h>

// define the array size of look up table here
#define	NUM_5MIN_INTERVALS	288
#define	NUM_ONRAMPS_PLUS_1	17
#define NUM_OFFRAMPS_PLUS_1	13
#define MIN_HARMONIC_SPEED	10.0    // km/hr
#define MAX_HARMONIC_SPEED	120.0   // km/hr 
#define MAX_MEAN_SPEED 120.0        // km/hr
#define MIN_DENSITY		10.0        // veh/km
#define MAX_DENSITY		124.0       // veh/km
#define MIN_FLOW		200.0       // veh/hr/lane
#define MAX_FLOW_PER_LANE	2000.0  // veh/hr/lane
#define MIN_OCCUPANCY 2             // unitless number from 0-100
#define MAX_OCCUPANCY 90.0          // unitless number from 0-100
#define MIN_OR_RAMP_FLOW_PER_LANE 200
#define MAX_OR_RAMP_FLOW_PER_LANE 950
#define MIN_FR_RAMP_FLOW_PER_LANE 50
#define MAX_FR_RAMP_FLOW_PER_LANE 1500 

struct confidence {
        float num_good_vals;
        float num_total_vals;
};

extern float maxd(float a,float b);
extern float mind(float a,float b);
extern float sum_array(float a[], int num_elements);
extern float mean_array(float a[], int num_elements);
extern float var_array(float a[], int num_elements);
extern long int factorial(int x);
extern long int nCr(int n, int r);
//extern int open_file(char controller_data_file[]);
//extern int read_file(char input_file_name[], char output_file_name[]);

//extern float get_split_ratio(float upstream_total_flow, float off_ramp_flow);

//extern float get_queue_estimation(float in_flow, float out_flow, float previous_queue);

//extern float get_density_estimation(float in_flow, float out_flow, float section_length, float previous_density, float speed);

//extern float get_off_ramp_flow_by_flow_balance(float upstream_total_flow, float on_ramp_flow, float downstream_total_flow);

//extern float get_on_ramp_flow_by_flow_balance(float upstream_total_flow, float downstream_total_flow, float off_ramp_flow);

extern float flow_aggregation_mainline(db_urms_status_t *controller_data, struct confidence *confidence);
extern float flow_aggregation_onramp(db_urms_status_t *controller_data, struct confidence *confidence);
extern float flow_aggregation_onramp_queue(db_urms_status_t *controller_data, db_urms_status2_t *controller_data2, struct confidence *confidence);
extern float flow_aggregation_offramp(db_urms_status3_t *controller_data, struct confidence *confidence);

extern float occupancy_aggregation_mainline(db_urms_status_t *controller_data, struct confidence *confidence);
extern float occupancy_aggregation_onramp(db_urms_status_t *controller_data, db_urms_status2_t *controller_data2, struct confidence *confidence);
extern float occupancy_aggregation_onramp_queue(db_urms_status_t *controller_data, db_urms_status2_t *controller_data2, struct confidence *confidence);
extern float occupancy_aggregation_offramp(db_urms_status3_t *controller_data, struct confidence *confidence);



extern float queue_onramp(db_urms_status_t *controller_data, db_urms_status2_t *controller_data2, struct confidence *confidence);

extern float hm_speed_aggregation_mainline(db_urms_status_t *controller_data, float hm_speed_prev,struct confidence *confidence);   // harmonic mean speed 
extern float mean_speed_aggregation_mainline(db_urms_status_t *controller_dat, float mean_speed_prev, struct confidence *confidence);  // mean speed

extern float density_aggregation_mainline(float flow, float hm_speed, float density_prev);
//extern float density_aggregation_mainline(db_urms_status_t *controller_data, struct confidence *confidence);

extern float turning_ratio_offramp(float FR_flow, float ML_flow);

//extern float interp_OR_HIS_FLOW(int OR_idx, float *OR_HIS_FLOW_DAT);

extern float interp_OR_HIS_FLOW(int OR_idx, float OR_flow_prev, const float OR_HIS_FLOW_DAT[NUM_5MIN_INTERVALS][NUM_ONRAMPS_PLUS_1], timestamp_t *ts);
extern float interp_OR_HIS_OCC(int OR_idx, float OR_occupancy_prev, const float OR_HIS_OCC_DAT[NUM_5MIN_INTERVALS][NUM_ONRAMPS_PLUS_1], timestamp_t *ts);

extern float interp_FR_HIS_FLOW(int FR_idx, float FR_flow_prev, const float FR_HIS_FLOW_DAT[NUM_5MIN_INTERVALS][NUM_OFFRAMPS_PLUS_1], timestamp_t *ts);
extern float interp_FR_HIS_OCC(int FR_idx, float FR_occupancy_prev, const float FR_HIS_OCC_DAT[NUM_5MIN_INTERVALS][NUM_OFFRAMPS_PLUS_1], timestamp_t *ts);

extern float ratio_ML_HIS_FLOW(float current_most_upstream_flow, const float MOST_UPSTREAM_MAINLINE_FLOW_DATA[NUM_5MIN_INTERVALS][2], timestamp_t *ts);

extern float butt_2_ML_flow(float flow, int ML_idx);
extern float butt_2_ML_speed(float speed, int ML_idx);
extern float butt_2_ML_occupancy(float occ, int ML_idx);
extern float butt_2_ML_density(float density, int ML_idx);
