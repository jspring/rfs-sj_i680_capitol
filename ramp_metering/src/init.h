#ifndef _INIT_H_
#define _INIT_H_

#include "temp_global.h"
#include "variables2.h"
#include "variables.h"
//these three functions need to disc
float get_init_ramp_queue();

float get_current_meter_rate();

int get_current_max_green();

int init_signal(struct signal_variables* psignal_data);

int init_realtimedata(struct signal_variables *psignal_data);

int init_ramp_data(struct ramp_variables* pramp_data);

#endif