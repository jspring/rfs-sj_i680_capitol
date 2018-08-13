#define _VARIABLES2_H_DECLS
#include "variables2.h"

#ifdef USE_CONFIG_FILE
//intersection parameters
int MAX_GREEN_UPPERBOUND;
int MAX_GREEN_LOWERBOUND;
float RAMP_LIMIT;
float LT_OCC_THRESHOLD;
int MAX_GREEN_CHANGE;
float RT_RELEASE_OCC_THRESHOLD;
float MAX_STEPS; //max steps (we think) a vehicle needs to pass a detector
float THRESHOLD_OCC_HEALTH; //threshold, if value changes are less then this ( 0.1 means 10% ) percent of values in previous cycles, and this keeps for many cycles, we think detector data has problem
float THRESHOLD_VOL_HEALTH;
int CYCLE_TO_ACTIVATE_OVERWRITE; //within cycle_to_release, how many cycles RT_occ needs to exceed threshold for activating overwrite release;
int CYCLE_TO_RELEASE; //how many cycles before a right-turn release happens;
int REGULAR_CYCLE;
int OVERWRITE_CYCLE; //after regular release cycles, how many cycles more a overwrite release should have;
float QUEUE_OCC_THRESHOLD_TO_ACTIVATE_INTERSECTION;
float LEFT_OCC_THRESHOLD_TO_ACTIVATE_INTERSECTION;
float RIGHT_OCC_THRESHOLD_TO_ACTIVATE_INTERSECTION;
int METHOD_FOR_RAMP_QUEUE; //the way to get ramp queue flag, 1: via queue estimation from in/out flow 2: queue detector 2 occupancy 3: left and right turn occupancy;

//ramp parameters;
float SHOCH_WAVE_SPEED;
float VEHICLE_EFFECTIVE_LENGTH;
float ALINEA_GAMMA;
float ALPHA;
float ALINEA_LOWER_BOUND;
float ALINEA_UPPER_BOUND;
int QUEUE_RESET;
float QUEUE_RESET_INTERVAL;
float RAMP_EXIT_FLOW_ADJUST_FACTOR;
float RAMP_METER_CHANGE_THRESHOLD;

//ramp parameters, ALINEA algorithm;
float OCC_CRITICAL;
float IN_LANES;
float OUT_LANES;
float ALINEA_KR;
#endif
