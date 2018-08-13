
#include "init.h"

extern float get_update_time_ramp(struct ramp_variables *pramp_data);

//these three functions need to disc
float get_init_ramp_queue()
{
	return 0;
}

float get_current_meter_rate()
{
	return 900.0;
}

int get_current_max_green()
{
	return 25;
}

int init_signal(struct signal_variables* psignal_data)
{

//	psignal_data->new_max_green=get_current_max_green(); This call has been replaced by a previous 
//	database call that gets the phase 3 max_green1 value

	psignal_data->last_sent_max_green=psignal_data->new_max_green;
	psignal_data->old_max_green = psignal_data->new_max_green;


	psignal_data->old_ramp_queue=get_init_ramp_queue();
	psignal_data->regular_release=false;
	psignal_data->regular_remain_cycle=0;
	psignal_data->overwrite_release=false;
	psignal_data->overwrite_remain_cycle=0;
	psignal_data->cycle_passed=0;

	psignal_data->prev_queue_reset_time = 0.0;	//this initial has problem

	return 0;
}

int init_realtimedata(struct signal_variables *psignal_data)
{
	int i,j;
	for(i=0;i<MAXROW;i++)
		for(j=0;j<MAXCOL;j++)
			psignal_data->realtime_data[i][j]=0;
	psignal_data->data_row=-1;
	psignal_data->prev_cycle_terminate_time = 0.0;
	psignal_data->current_cycle_terminate_time = 0.0;
	return 0;
}

int init_ramp_data(struct ramp_variables* pramp_data)
{
	int i,j;
	for(i=0;i<NUMBER_RAMP_DATA;i++)
		for(j=0;j<NUMBER_MAINLINE_LANE;j++)
			pramp_data->mainline_lead_status[i][j] = 0;
	for(i=0;i<NUMBER_RAMP_DATA;i++)
		for(j=0;j<NUMBER_MAINLINE_LANE;j++)
			pramp_data->mainline_trail_status[i][j] = 0;
	for(i=0;i<NUMBER_RAMP_DATA;i++)
		for(j=0;j<NUMBER_MAINLINE_LANE;j++)
			pramp_data->mainline_lead_vol[i][j] = 0;
	for(i=0;i<NUMBER_RAMP_DATA;i++)
		for(j=0;j<NUMBER_MAINLINE_LANE;j++)
			pramp_data->mainline_trail_vol[i][j] = 0;
	for(i=0;i<NUMBER_RAMP_DATA;i++)
		for(j=0;j<NUMBER_MAINLINE_LANE;j++)
			pramp_data->mainline_lead_occ[i][j] = 0.0;
	for(i=0;i<NUMBER_RAMP_DATA;i++)
		for(j=0;j<NUMBER_MAINLINE_LANE;j++)
			pramp_data->mainline_trail_occ[i][j] = 0.0;

	for(i=0;i<NUMBER_RAMP_DATA;i++)
		for(j=0;j<NUMBER_METERED_LANE;j++)
			pramp_data->passage_vol[i][j] = 0;
	for(i=0;i<NUMBER_RAMP_DATA;i++)
		for(j=0;j<NUMBER_METERED_LANE;j++)
			pramp_data->demand_vol[i][j] = 0;
	for(i=0;i<NUMBER_RAMP_DATA;i++)
		for(j=0;j<NUMBER_METERED_LANE;j++)
			pramp_data->queue_vol[i][j] = 0;
	for(i=0;i<NUMBER_RAMP_DATA;i++)
		for(j=0;j<NUMBER_METERED_LANE;j++)
			pramp_data->queue_occ[i][j] = 0.0;
	for(i=0;i<NUMBER_RAMP_DATA;i++)
		for(j=0;j<NUMBER_METERED_LANE;j++)
			pramp_data->meter_rate[i][j] = 0.0;

	for(j=0;j<NUMBER_METERED_LANE;j++)
		pramp_data->mainline_avg_occupancy[j] = 0.0;
	for(j=0;j<NUMBER_METERED_LANE;j++)
		pramp_data->mainline_avg_volume[j] = 0;

	for(i=0;i<NUMBER_RAMP_DATA;i++)
		pramp_data->data_time[i] = 0.0;

	get_update_time_ramp(pramp_data);
	pramp_data->prev_occ_out = 5.0;	//this initial has problem


	return 0;
}
