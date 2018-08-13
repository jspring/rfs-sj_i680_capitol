#include <db_include.h>
#include <path_gps_lib.h>
#include "data_log.h"
#include <urms.h>
//#include "wrfiles_rm.h"
#include "variables2.h"
#include "meter_lib.h"
#include "rm_algo.h"

//Ramp meter inputs
db_urms_status_t db_urms_status;
db_urms_status2_t db_urms_status2;
db_urms_status3_t db_urms_status3;
urms_datafile_t urms_datafile;
db_ramp_data_t db_ramp_data;
db_urms_t db_urms;

/**
 *      This array is used to specify how to read in the DB vars.
 */
db_var_spec_t db_vars_ac_rm[] =
{
        {1, sizeof(db_urms_status_t), &db_urms_status},
        {2, sizeof(urms_datafile_t), &urms_datafile},
        {3, sizeof(db_urms_t), &db_urms},
        {4, sizeof(db_urms_status2_t), &db_urms_status2},
        {5, sizeof(db_urms_status3_t), &db_urms_status3},
        {6, sizeof(db_ramp_data_t), &db_ramp_data},
};
int num_ac_rm_vars = (sizeof(db_vars_ac_rm)/sizeof(db_var_spec_t));

/** This array is used to specify the output format of the data file.
 */
data_log_column_spec_t file_spec[] = 
{
        {"HH:MM:SS.SSS ", &db_urms_status2.ts, BASE_TIMESTAMP, REPLAY_TIME},		//###1
//get_long_status8_resp_mess_typ
        {"%hhu ",   &db_urms_status.mainline_stat[0].lead_stat, BASE_CHAR, REPLAY_USE},	//###2
        {"%hhu ",   &db_urms_status.mainline_stat[0].lead_vol, BASE_CHAR, REPLAY_USE},	//###3
        {"%.3f ",   &urms_datafile.mainline_lead_occ[0], BASE_FLOAT, REPLAY_USE},	//###4
        {"%hhu ",   &db_urms_status.mainline_stat[0].trail_stat, BASE_CHAR, REPLAY_USE},//###5
        {"%hhu ",   &db_urms_status.mainline_stat[0].trail_vol, BASE_CHAR, REPLAY_USE},	//###6
        {"%.3f ",   &urms_datafile.mainline_trail_occ[0], BASE_FLOAT, REPLAY_USE},	//###7
        {"%hhu ",   &db_urms_status.mainline_stat[0].speed, BASE_CHAR, REPLAY_USE},	//###8

        {"%hhu ",   &db_urms_status.mainline_stat[1].lead_stat, BASE_CHAR, REPLAY_USE},	//###9
        {"%hhu ",   &db_urms_status.mainline_stat[1].lead_vol, BASE_CHAR, REPLAY_USE},	//###10
        {"%.3f ",   &urms_datafile.mainline_lead_occ[1], BASE_FLOAT, REPLAY_USE},	//###11
        {"%hhu ",   &db_urms_status.mainline_stat[1].trail_stat, BASE_CHAR, REPLAY_USE},//###12
        {"%hhu ",   &db_urms_status.mainline_stat[1].trail_vol, BASE_CHAR, REPLAY_USE},	//###13
        {"%.3f ",   &urms_datafile.mainline_trail_occ[1], BASE_FLOAT, REPLAY_USE},	//###14
        {"%hhu ",   &db_urms_status.mainline_stat[1].speed, BASE_CHAR, REPLAY_USE},	//###15

        {"%hhu ",   &db_urms_status.mainline_stat[2].lead_stat, BASE_CHAR, REPLAY_USE},	//###16
        {"%hhu ",   &db_urms_status.mainline_stat[2].lead_vol, BASE_CHAR, REPLAY_USE},	//###17
        {"%.3f ",   &urms_datafile.mainline_lead_occ[2], BASE_FLOAT, REPLAY_USE},	//###18
        {"%hhu ",   &db_urms_status.mainline_stat[2].trail_stat, BASE_CHAR, REPLAY_USE},//###19
        {"%hhu ",   &db_urms_status.mainline_stat[2].trail_vol, BASE_CHAR, REPLAY_USE},	//###20
        {"%.3f ",   &urms_datafile.mainline_trail_occ[2], BASE_FLOAT, REPLAY_USE},	//###21
        {"%hhu ",   &db_urms_status.mainline_stat[2].speed, BASE_CHAR, REPLAY_USE},	//###22

        {"%hhu ",   &db_urms_status.mainline_stat[3].lead_stat, BASE_CHAR, REPLAY_USE},	//###23
        {"%hhu ",   &db_urms_status.mainline_stat[3].lead_vol, BASE_CHAR, REPLAY_USE},	//###24
        {"%.3f ",   &urms_datafile.mainline_lead_occ[3], BASE_FLOAT, REPLAY_USE},	//###25
        {"%hhu ",   &db_urms_status.mainline_stat[3].trail_stat, BASE_CHAR, REPLAY_USE},//###26
        {"%hhu ",   &db_urms_status.mainline_stat[3].trail_vol, BASE_CHAR, REPLAY_USE},	//###27
        {"%.3f ",   &urms_datafile.mainline_trail_occ[3], BASE_FLOAT, REPLAY_USE},	//###28
        {"%hhu ",   &db_urms_status.mainline_stat[3].speed, BASE_CHAR, REPLAY_USE},	//###29

        {"%hhu ",   &db_urms_status.mainline_stat[4].lead_stat, BASE_CHAR, REPLAY_USE},	//###30
        {"%hhu ",   &db_urms_status.mainline_stat[4].lead_vol, BASE_CHAR, REPLAY_USE},	//###31
        {"%.3f ",   &urms_datafile.mainline_lead_occ[4], BASE_FLOAT, REPLAY_USE},	//###32
        {"%hhu ",   &db_urms_status.mainline_stat[4].trail_stat, BASE_CHAR, REPLAY_USE},//###33
        {"%hhu ",   &db_urms_status.mainline_stat[4].trail_vol, BASE_CHAR, REPLAY_USE},	//###34
        {"%.3f ",   &urms_datafile.mainline_trail_occ[4], BASE_FLOAT, REPLAY_USE},	//###35
        {"%hhu ",   &db_urms_status.mainline_stat[4].speed, BASE_CHAR, REPLAY_USE},	//###36

        {"%hhu ",   &db_urms_status.mainline_stat[5].lead_stat, BASE_CHAR, REPLAY_USE},	//###37
        {"%hhu ",   &db_urms_status.mainline_stat[5].lead_vol, BASE_CHAR, REPLAY_USE},	//###38
        {"%.3f ",   &urms_datafile.mainline_lead_occ[5], BASE_FLOAT, REPLAY_USE},	//###39
        {"%hhu ",   &db_urms_status.mainline_stat[5].trail_stat, BASE_CHAR, REPLAY_USE},//###40
        {"%hhu ",   &db_urms_status.mainline_stat[5].trail_vol, BASE_CHAR, REPLAY_USE},	//###41
        {"%.3f ",   &urms_datafile.mainline_trail_occ[5], BASE_FLOAT, REPLAY_USE},	//###42
        {"%hhu ",   &db_urms_status.mainline_stat[5].speed, BASE_CHAR, REPLAY_USE},	//###43

        {"%hhu ",   &db_urms_status.metered_lane_stat[0].demand_vol, BASE_CHAR, REPLAY_USE},	//###44
        {"%hhu ",   &db_urms_status.metered_lane_stat[0].passage_vol, BASE_CHAR, REPLAY_USE},	//###45
        {"%.3f ",   &urms_datafile.queue_occ[0], BASE_FLOAT, REPLAY_USE},			//###46
        {"%hhu ",   &db_urms_status2.queue_stat[0][0].vol, BASE_CHAR, REPLAY_USE},		//###47
        {"%d ",     &db_urms_status3.metering_rate[0], BASE_INT, REPLAY_USE},			//###48 Tried using BASE_SHORT but it didn't work!
        {"%hhu ",   &db_urms_status3.cmd_src[0], BASE_CHAR, REPLAY_USE},			//###49
        {"%hhu ",   &db_urms_status3.action[0], BASE_CHAR, REPLAY_USE},				//###50
        {"%hhu ",   &db_urms_status3.plan[0], BASE_CHAR, REPLAY_USE},				//###51

        {"%hhu ",   &db_urms_status.metered_lane_stat[1].demand_vol, BASE_CHAR, REPLAY_USE},	//###52
        {"%hhu ",   &db_urms_status.metered_lane_stat[1].passage_vol, BASE_CHAR, REPLAY_USE},	//###53
        {"%.3f ",   &urms_datafile.queue_occ[1], BASE_FLOAT, REPLAY_USE},			//###54
        {"%hhu ",   &db_urms_status2.queue_stat[1][0].vol, BASE_CHAR, REPLAY_USE},		//###55
        {"%d ",     &db_urms_status3.metering_rate[1], BASE_INT, REPLAY_USE},			//###56 Tried using BASE_SHORT but it didn't work!
        {"%hhu ",   &db_urms_status3.cmd_src[1], BASE_CHAR, REPLAY_USE},			//###57
        {"%hhu ",   &db_urms_status3.action[1], BASE_CHAR, REPLAY_USE},				//###58
        {"%hhu ",   &db_urms_status3.plan[1], BASE_CHAR, REPLAY_USE},				//###59

        {"%hhu ",   &db_urms_status.metered_lane_stat[2].demand_vol, BASE_CHAR, REPLAY_USE},	//###60
        {"%hhu ",   &db_urms_status.metered_lane_stat[2].passage_vol, BASE_CHAR, REPLAY_USE},	//###61
        {"%.3f ",   &urms_datafile.queue_occ[2], BASE_FLOAT, REPLAY_USE},			//###62
        {"%hhu ",   &db_urms_status2.queue_stat[2][0].vol, BASE_CHAR, REPLAY_USE},		//###63
        {"%d ",     &db_urms_status3.metering_rate[2], BASE_INT, REPLAY_USE},			//###64
        {"%hhu ",   &db_urms_status3.cmd_src[2], BASE_CHAR, REPLAY_USE},			//###65
        {"%hhu ",   &db_urms_status3.action[2], BASE_CHAR, REPLAY_USE},				//###66
        {"%hhu ",   &db_urms_status3.plan[2], BASE_CHAR, REPLAY_USE},				//###67

        {"%hhu ",   &db_urms_status3.additional_det[0].volume, BASE_CHAR, REPLAY_USE},	//###68
        {"%hhu ",   &db_urms_status3.additional_det[0].occ_msb, BASE_CHAR, REPLAY_USE},	//###69
        {"%hhu ",   &db_urms_status3.additional_det[0].occ_lsb, BASE_CHAR, REPLAY_USE},	//###70
        {"%hhu ",   &db_urms_status3.additional_det[0].stat, BASE_CHAR, REPLAY_USE},	//###71

        {"%hhu ",   &db_urms_status3.additional_det[1].volume, BASE_CHAR, REPLAY_USE},	//###72
        {"%hhu ",   &db_urms_status3.additional_det[1].occ_msb, BASE_CHAR, REPLAY_USE},	//###73
        {"%hhu ",   &db_urms_status3.additional_det[1].occ_lsb, BASE_CHAR, REPLAY_USE},	//###74
        {"%hhu ",   &db_urms_status3.additional_det[1].stat, BASE_CHAR, REPLAY_USE},	//###75


        {"%hhu ",   &db_urms_status3.rm2rmc_ctr, BASE_CHAR, REPLAY_USE},		//###76
	{"%hhu ",   &db_urms_status3.is_metering, BASE_CHAR, REPLAY_USE},		//###77
	{"%hhu ",   &db_urms_status3.computation_finished, BASE_CHAR, REPLAY_USE}, 	//###78
	{"%hx ",    &db_urms_status.checksum, BASE_CHAR, REPLAY_USE}, 			//###79
        {"%d ",     &db_urms_status3.metering_rate[2], BASE_INT, REPLAY_USE},		//###80 Tried using BASE_SHORT but it didn't work!
	{"%.1f ",   &db_ramp_data.new_meter_rate, BASE_FLOAT, REPLAY_USE}, 		//###81
	{"%.1f ",   &db_ramp_data.data_time, BASE_FLOAT, REPLAY_USE}, 			//###82
	{"%.1f ",   &db_ramp_data.prev_update_data, BASE_FLOAT, REPLAY_USE}, 		//###83
	{"%.3f ",   &db_ramp_data.prev_occ_out, BASE_FLOAT, REPLAY_USE}, 		//###84
	{"%hhu ",   &db_ramp_data.passage_vol[2], BASE_CHAR, REPLAY_USE}, 		//###85
	{"%hhu ",   &db_urms_status3.plan_base_lvl[0], BASE_CHAR, REPLAY_USE},		//###86
	{"%hhu ",   &db_urms_status3.plan_base_lvl[1], BASE_CHAR, REPLAY_USE},		//###87
	{"%hhu ",   &db_urms_status3.plan_base_lvl[2], BASE_CHAR, REPLAY_USE},		//###88
	{"%hhu ",   &db_urms_status.hour, BASE_CHAR, REPLAY_USE},			//###89
	{"%hhu ",   &db_urms_status.minute, BASE_CHAR, REPLAY_USE},			//###90
	{"%hhu ",   &db_urms_status.second, BASE_CHAR, REPLAY_USE},			//###91
	{"%hhu ",   &db_urms_status.num_main, BASE_CHAR, REPLAY_USE},			//###92
	{"%hhu ",   &db_urms_status.num_meter, BASE_CHAR, REPLAY_USE},			//###93
	{"%hhu ",   &db_urms_status3.num_addl_det, BASE_CHAR, REPLAY_USE},		//###94
	{"%hhu ",   &db_urms_status3.num_opp, BASE_CHAR, REPLAY_USE},			//###95

        {"%hhu ",   &db_urms_status3.additional_det[2].volume, BASE_CHAR, REPLAY_USE},	//###96
        {"%hhu ",   &db_urms_status3.additional_det[2].occ_msb, BASE_CHAR, REPLAY_USE},	//###97
        {"%hhu ",   &db_urms_status3.additional_det[2].occ_lsb, BASE_CHAR, REPLAY_USE},	//###98
        {"%hhu ",   &db_urms_status3.additional_det[2].stat, BASE_CHAR, REPLAY_USE},	//###99

        {"%hhu ",   &db_urms_status3.additional_det[3].volume, BASE_CHAR, REPLAY_USE},	//###100
        {"%hhu ",   &db_urms_status3.additional_det[3].occ_msb, BASE_CHAR, REPLAY_USE},	//###101
        {"%hhu ",   &db_urms_status3.additional_det[3].occ_lsb, BASE_CHAR, REPLAY_USE},	//###102
        {"%hhu ",   &db_urms_status3.additional_det[3].stat, BASE_CHAR, REPLAY_USE},	//###103

        {"%hhu ",   &db_urms_status3.additional_det[4].volume, BASE_CHAR, REPLAY_USE},	//###104
        {"%hhu ",   &db_urms_status3.additional_det[4].occ_msb, BASE_CHAR, REPLAY_USE},	//###105
        {"%hhu ",   &db_urms_status3.additional_det[4].occ_lsb, BASE_CHAR, REPLAY_USE},	//###106
        {"%hhu ",   &db_urms_status3.additional_det[4].stat, BASE_CHAR, REPLAY_USE},	//###107

        {"%hhu ",   &db_urms_status3.additional_det[5].volume, BASE_CHAR, REPLAY_USE},	//###108
        {"%hhu ",   &db_urms_status3.additional_det[5].occ_msb, BASE_CHAR, REPLAY_USE},	//###109
        {"%hhu ",   &db_urms_status3.additional_det[5].occ_lsb, BASE_CHAR, REPLAY_USE},	//###110
        {"%hhu ",   &db_urms_status3.additional_det[5].stat, BASE_CHAR, REPLAY_USE},	//###111

        {"%hhu ",   &db_urms_status3.additional_det[6].volume, BASE_CHAR, REPLAY_USE},	//###112
        {"%hhu ",   &db_urms_status3.additional_det[6].occ_msb, BASE_CHAR, REPLAY_USE},	//###113
        {"%hhu ",   &db_urms_status3.additional_det[6].occ_lsb, BASE_CHAR, REPLAY_USE},	//###114
        {"%hhu ",   &db_urms_status3.additional_det[6].stat, BASE_CHAR, REPLAY_USE},	//###115

        {"%hhu ",   &db_urms_status3.additional_det[7].volume, BASE_CHAR, REPLAY_USE},	//###116
        {"%hhu ",   &db_urms_status3.additional_det[7].occ_msb, BASE_CHAR, REPLAY_USE},	//###117
        {"%hhu ",   &db_urms_status3.additional_det[7].occ_lsb, BASE_CHAR, REPLAY_USE},	//###118
        {"%hhu ",   &db_urms_status3.additional_det[7].stat, BASE_CHAR, REPLAY_USE},	//###119

        {"%hhu ",   &db_urms_status3.additional_det[8].volume, BASE_CHAR, REPLAY_USE},	//###120
        {"%hhu ",   &db_urms_status3.additional_det[8].occ_msb, BASE_CHAR, REPLAY_USE},	//###121
        {"%hhu ",   &db_urms_status3.additional_det[8].occ_lsb, BASE_CHAR, REPLAY_USE},	//###122
        {"%hhu ",   &db_urms_status3.additional_det[8].stat, BASE_CHAR, REPLAY_USE},	//###123

        {"%hhu ",   &db_urms_status3.additional_det[9].volume, BASE_CHAR, REPLAY_USE},	//###124
        {"%hhu ",   &db_urms_status3.additional_det[9].occ_msb, BASE_CHAR, REPLAY_USE},	//###125
        {"%hhu ",   &db_urms_status3.additional_det[9].occ_lsb, BASE_CHAR, REPLAY_USE},	//###126
        {"%hhu ",   &db_urms_status3.additional_det[9].stat, BASE_CHAR, REPLAY_USE},	//###127

        {"%hhu ",   &db_urms_status3.additional_det[10].volume, BASE_CHAR, REPLAY_USE},	//###128
        {"%hhu ",   &db_urms_status3.additional_det[10].occ_msb, BASE_CHAR, REPLAY_USE},//###129
        {"%hhu ",   &db_urms_status3.additional_det[10].occ_lsb, BASE_CHAR, REPLAY_USE},//###130
        {"%hhu ",   &db_urms_status3.additional_det[10].stat, BASE_CHAR, REPLAY_USE},	//###131

        {"%hhu ",   &db_urms_status3.additional_det[11].volume, BASE_CHAR, REPLAY_USE},	//###132
        {"%hhu ",   &db_urms_status3.additional_det[11].occ_msb, BASE_CHAR, REPLAY_USE},//###133
        {"%hhu ",   &db_urms_status3.additional_det[11].occ_lsb, BASE_CHAR, REPLAY_USE},//###134
        {"%hhu ",   &db_urms_status3.additional_det[11].stat, BASE_CHAR, REPLAY_USE},	//###135

        {"%hhu ",   &db_urms_status3.additional_det[12].volume, BASE_CHAR, REPLAY_USE},	//###136
        {"%hhu ",   &db_urms_status3.additional_det[12].occ_msb, BASE_CHAR, REPLAY_USE},//###137
        {"%hhu ",   &db_urms_status3.additional_det[12].occ_lsb, BASE_CHAR, REPLAY_USE},//###138
        {"%hhu ",   &db_urms_status3.additional_det[12].stat, BASE_CHAR, REPLAY_USE},	//###139

        {"%hhu ",   &db_urms_status3.additional_det[13].volume, BASE_CHAR, REPLAY_USE},	//###140
        {"%hhu ",   &db_urms_status3.additional_det[13].occ_msb, BASE_CHAR, REPLAY_USE},//###141
        {"%hhu ",   &db_urms_status3.additional_det[13].occ_lsb, BASE_CHAR, REPLAY_USE},//###142
        {"%hhu ",   &db_urms_status3.additional_det[13].stat, BASE_CHAR, REPLAY_USE},	//###143

        {"%hhu ",   &db_urms_status3.additional_det[14].volume, BASE_CHAR, REPLAY_USE},	//###144
        {"%hhu ",   &db_urms_status3.additional_det[14].occ_msb, BASE_CHAR, REPLAY_USE},//###145
        {"%hhu ",   &db_urms_status3.additional_det[14].occ_lsb, BASE_CHAR, REPLAY_USE},//###146
        {"%hhu ",   &db_urms_status3.additional_det[14].stat, BASE_CHAR, REPLAY_USE},	//###147

        {"%hhu ",   &db_urms_status3.additional_det[15].volume, BASE_CHAR, REPLAY_USE},	//###148
        {"%hhu ",   &db_urms_status3.additional_det[15].occ_msb, BASE_CHAR, REPLAY_USE},//###149
        {"%hhu ",   &db_urms_status3.additional_det[15].occ_lsb, BASE_CHAR, REPLAY_USE},//###150
        {"%hhu ",   &db_urms_status3.additional_det[15].stat, BASE_CHAR, REPLAY_USE},	//###151

        {"%hhu ",   &db_urms_status2.queue_stat[0][0].vol, BASE_CHAR, REPLAY_USE},	//###152
        {"%hhu ",   &db_urms_status2.queue_stat[0][0].occ_msb, BASE_CHAR, REPLAY_USE},	//###153
        {"%hhu ",   &db_urms_status2.queue_stat[0][0].occ_lsb, BASE_CHAR, REPLAY_USE},	//###154
        {"%hhu ",   &db_urms_status2.queue_stat[0][0].stat, BASE_CHAR, REPLAY_USE},	//###155
        {"%hhu ",   &db_urms_status2.queue_stat[0][0].flag, BASE_CHAR, REPLAY_USE},	//###156

        {"%hhu ",   &db_urms_status2.queue_stat[0][1].vol, BASE_CHAR, REPLAY_USE},	//###157
        {"%hhu ",   &db_urms_status2.queue_stat[0][1].occ_msb, BASE_CHAR, REPLAY_USE},	//###158
        {"%hhu ",   &db_urms_status2.queue_stat[0][1].occ_lsb, BASE_CHAR, REPLAY_USE},	//###159
        {"%hhu ",   &db_urms_status2.queue_stat[0][1].stat, BASE_CHAR, REPLAY_USE},	//###160
        {"%hhu ",   &db_urms_status2.queue_stat[0][1].flag, BASE_CHAR, REPLAY_USE},	//###161

        {"%hhu ",   &db_urms_status2.queue_stat[0][2].vol, BASE_CHAR, REPLAY_USE},	//###162
        {"%hhu ",   &db_urms_status2.queue_stat[0][2].occ_msb, BASE_CHAR, REPLAY_USE},	//###163
        {"%hhu ",   &db_urms_status2.queue_stat[0][2].occ_lsb, BASE_CHAR, REPLAY_USE},	//###164
        {"%hhu ",   &db_urms_status2.queue_stat[0][2].stat, BASE_CHAR, REPLAY_USE},	//###165
        {"%hhu ",   &db_urms_status2.queue_stat[0][2].flag, BASE_CHAR, REPLAY_USE},	//###166

        {"%hhu ",   &db_urms_status2.queue_stat[0][3].vol, BASE_CHAR, REPLAY_USE},	//###167
        {"%hhu ",   &db_urms_status2.queue_stat[0][3].occ_msb, BASE_CHAR, REPLAY_USE},	//###168
        {"%hhu ",   &db_urms_status2.queue_stat[0][3].occ_lsb, BASE_CHAR, REPLAY_USE},	//###169
        {"%hhu ",   &db_urms_status2.queue_stat[0][3].stat, BASE_CHAR, REPLAY_USE},	//###170
        {"%hhu ",   &db_urms_status2.queue_stat[0][3].flag, BASE_CHAR, REPLAY_USE},	//###171

        {"%hhu ",   &db_urms_status2.queue_stat[1][0].vol, BASE_CHAR, REPLAY_USE},	//###172
        {"%hhu ",   &db_urms_status2.queue_stat[1][0].occ_msb, BASE_CHAR, REPLAY_USE},	//###173
        {"%hhu ",   &db_urms_status2.queue_stat[1][0].occ_lsb, BASE_CHAR, REPLAY_USE},	//###174
        {"%hhu ",   &db_urms_status2.queue_stat[1][0].stat, BASE_CHAR, REPLAY_USE},	//###175
        {"%hhu ",   &db_urms_status2.queue_stat[1][0].flag, BASE_CHAR, REPLAY_USE},	//###176

        {"%hhu ",   &db_urms_status2.queue_stat[1][1].vol, BASE_CHAR, REPLAY_USE},	//###177
        {"%hhu ",   &db_urms_status2.queue_stat[1][1].occ_msb, BASE_CHAR, REPLAY_USE},	//###178
        {"%hhu ",   &db_urms_status2.queue_stat[1][1].occ_lsb, BASE_CHAR, REPLAY_USE},	//###179
        {"%hhu ",   &db_urms_status2.queue_stat[1][1].stat, BASE_CHAR, REPLAY_USE},	//###180
        {"%hhu ",   &db_urms_status2.queue_stat[1][1].flag, BASE_CHAR, REPLAY_USE},	//###181

        {"%hhu ",   &db_urms_status2.queue_stat[1][2].vol, BASE_CHAR, REPLAY_USE},	//###182
        {"%hhu ",   &db_urms_status2.queue_stat[1][2].occ_msb, BASE_CHAR, REPLAY_USE},	//###183
        {"%hhu ",   &db_urms_status2.queue_stat[1][2].occ_lsb, BASE_CHAR, REPLAY_USE},	//###184
        {"%hhu ",   &db_urms_status2.queue_stat[1][2].stat, BASE_CHAR, REPLAY_USE},	//###185
        {"%hhu ",   &db_urms_status2.queue_stat[1][2].flag, BASE_CHAR, REPLAY_USE},	//###186

        {"%hhu ",   &db_urms_status2.queue_stat[1][3].vol, BASE_CHAR, REPLAY_USE},	//###187
        {"%hhu ",   &db_urms_status2.queue_stat[1][3].occ_msb, BASE_CHAR, REPLAY_USE},	//###188
        {"%hhu ",   &db_urms_status2.queue_stat[1][3].occ_lsb, BASE_CHAR, REPLAY_USE},	//###189
        {"%hhu ",   &db_urms_status2.queue_stat[1][3].stat, BASE_CHAR, REPLAY_USE},	//###190
        {"%hhu ",   &db_urms_status2.queue_stat[1][3].flag, BASE_CHAR, REPLAY_USE},	//###191

        {"%hhu ",   &db_urms_status2.queue_stat[2][0].vol, BASE_CHAR, REPLAY_USE},	//###192
        {"%hhu ",   &db_urms_status2.queue_stat[2][0].occ_msb, BASE_CHAR, REPLAY_USE},	//###193
        {"%hhu ",   &db_urms_status2.queue_stat[2][0].occ_lsb, BASE_CHAR, REPLAY_USE},	//###194
        {"%hhu ",   &db_urms_status2.queue_stat[2][0].stat, BASE_CHAR, REPLAY_USE},	//###195
        {"%hhu ",   &db_urms_status2.queue_stat[2][0].flag, BASE_CHAR, REPLAY_USE},	//###196

        {"%hhu ",   &db_urms_status2.queue_stat[2][1].vol, BASE_CHAR, REPLAY_USE},	//###197
        {"%hhu ",   &db_urms_status2.queue_stat[2][1].occ_msb, BASE_CHAR, REPLAY_USE},	//###198
        {"%hhu ",   &db_urms_status2.queue_stat[2][1].occ_lsb, BASE_CHAR, REPLAY_USE},	//###199
        {"%hhu ",   &db_urms_status2.queue_stat[2][1].stat, BASE_CHAR, REPLAY_USE},	//###200
        {"%hhu ",   &db_urms_status2.queue_stat[2][1].flag, BASE_CHAR, REPLAY_USE},	//###201

        {"%hhu ",   &db_urms_status2.queue_stat[2][2].vol, BASE_CHAR, REPLAY_USE},	//###202
        {"%hhu ",   &db_urms_status2.queue_stat[2][2].occ_msb, BASE_CHAR, REPLAY_USE},	//###203
        {"%hhu ",   &db_urms_status2.queue_stat[2][2].occ_lsb, BASE_CHAR, REPLAY_USE},	//###204
        {"%hhu ",   &db_urms_status2.queue_stat[2][2].stat, BASE_CHAR, REPLAY_USE},	//###205
        {"%hhu ",   &db_urms_status2.queue_stat[2][2].flag, BASE_CHAR, REPLAY_USE},	//###206

        {"%hhu ",   &db_urms_status2.queue_stat[2][3].vol, BASE_CHAR, REPLAY_USE},	//###207
        {"%hhu ",   &db_urms_status2.queue_stat[2][3].occ_msb, BASE_CHAR, REPLAY_USE},	//###208
        {"%hhu ",   &db_urms_status2.queue_stat[2][3].occ_lsb, BASE_CHAR, REPLAY_USE},	//###209
        {"%hhu ",   &db_urms_status2.queue_stat[2][3].stat, BASE_CHAR, REPLAY_USE},	//###210
        {"%hhu ",   &db_urms_status2.queue_stat[2][3].flag, BASE_CHAR, REPLAY_USE},	//###211

        {"%hhu ",   &db_urms_status2.queue_stat[3][0].vol, BASE_CHAR, REPLAY_USE},	//###212
        {"%hhu ",   &db_urms_status2.queue_stat[3][0].occ_msb, BASE_CHAR, REPLAY_USE},	//###213
        {"%hhu ",   &db_urms_status2.queue_stat[3][0].occ_lsb, BASE_CHAR, REPLAY_USE},	//###214
        {"%hhu ",   &db_urms_status2.queue_stat[3][0].stat, BASE_CHAR, REPLAY_USE},	//###215
        {"%hhu ",   &db_urms_status2.queue_stat[3][0].flag, BASE_CHAR, REPLAY_USE},	//###216

        {"%hhu ",   &db_urms_status2.queue_stat[3][1].vol, BASE_CHAR, REPLAY_USE},	//###217
        {"%hhu ",   &db_urms_status2.queue_stat[3][1].occ_msb, BASE_CHAR, REPLAY_USE},	//###218
        {"%hhu ",   &db_urms_status2.queue_stat[3][1].occ_lsb, BASE_CHAR, REPLAY_USE},	//###219
        {"%hhu ",   &db_urms_status2.queue_stat[3][1].stat, BASE_CHAR, REPLAY_USE},	//###220
        {"%hhu ",   &db_urms_status2.queue_stat[3][1].flag, BASE_CHAR, REPLAY_USE},	//###221

        {"%hhu ",   &db_urms_status2.queue_stat[3][2].vol, BASE_CHAR, REPLAY_USE},	//###222
        {"%hhu ",   &db_urms_status2.queue_stat[3][2].occ_msb, BASE_CHAR, REPLAY_USE},	//###223
        {"%hhu ",   &db_urms_status2.queue_stat[3][2].occ_lsb, BASE_CHAR, REPLAY_USE},	//###224
        {"%hhu ",   &db_urms_status2.queue_stat[3][2].stat, BASE_CHAR, REPLAY_USE},	//###225
        {"%hhu ",   &db_urms_status2.queue_stat[3][2].flag, BASE_CHAR, REPLAY_USE},	//###226

        {"%hhu ",   &db_urms_status2.queue_stat[3][3].vol, BASE_CHAR, REPLAY_USE},	//###227
        {"%hhu ",   &db_urms_status2.queue_stat[3][3].occ_msb, BASE_CHAR, REPLAY_USE},	//###228
        {"%hhu ",   &db_urms_status2.queue_stat[3][3].occ_lsb, BASE_CHAR, REPLAY_USE},	//###229
        {"%hhu ",   &db_urms_status2.queue_stat[3][3].stat, BASE_CHAR, REPLAY_USE},	//###230
        {"%hhu ",   &db_urms_status2.queue_stat[3][3].flag, BASE_CHAR, REPLAY_USE},	//###231

        {"%hhu ",   &db_urms_status.mainline_stat[6].lead_stat, BASE_CHAR, REPLAY_USE},	//###232
        {"%hhu ",   &db_urms_status.mainline_stat[6].lead_vol, BASE_CHAR, REPLAY_USE},	//###233
        {"%.3f ",   &urms_datafile.mainline_lead_occ[6], BASE_FLOAT, REPLAY_USE},	//###234
        {"%hhu ",   &db_urms_status.mainline_stat[6].trail_stat, BASE_CHAR, REPLAY_USE},//###235
        {"%hhu ",   &db_urms_status.mainline_stat[6].trail_vol, BASE_CHAR, REPLAY_USE},	//###236
        {"%.3f ",   &urms_datafile.mainline_trail_occ[6], BASE_FLOAT, REPLAY_USE},	//###237
        {"%hhu ",   &db_urms_status.mainline_stat[6].speed, BASE_CHAR, REPLAY_USE},	//###238

        {"%hhu ",   &db_urms_status.mainline_stat[7].lead_stat, BASE_CHAR, REPLAY_USE},	//###239
        {"%hhu ",   &db_urms_status.mainline_stat[7].lead_vol, BASE_CHAR, REPLAY_USE},	//###240
        {"%.3f ",   &urms_datafile.mainline_lead_occ[7], BASE_FLOAT, REPLAY_USE},	//###241
        {"%hhu ",   &db_urms_status.mainline_stat[7].trail_stat, BASE_CHAR, REPLAY_USE},//###242
        {"%hhu ",   &db_urms_status.mainline_stat[7].trail_vol, BASE_CHAR, REPLAY_USE},	//###243
        {"%.3f ",   &urms_datafile.mainline_trail_occ[7], BASE_FLOAT, REPLAY_USE},	//###244
        {"%hhu ",   &db_urms_status.mainline_stat[7].speed, BASE_CHAR, REPLAY_USE},	//###245

        {"%hhu ",   &db_urms_status.metered_lane_stat[3].demand_vol, BASE_CHAR, REPLAY_USE},	//###246
        {"%hhu ",   &db_urms_status.metered_lane_stat[3].passage_vol, BASE_CHAR, REPLAY_USE},	//###247
        {"%.3f ",   &urms_datafile.queue_occ[3], BASE_FLOAT, REPLAY_USE},	//###248
        {"%hhu ",   &db_urms_status2.queue_stat[3][0].vol, BASE_CHAR, REPLAY_USE},		//###249
        {"%d ",     &db_urms_status3.metering_rate[3], BASE_INT, REPLAY_USE},			//###250 Tried using BASE_SHORT but it didn't work!
        {"%hhu ",   &db_urms_status3.cmd_src[3], BASE_CHAR, REPLAY_USE},			//###251
        {"%hhu ",   &db_urms_status3.action[3], BASE_CHAR, REPLAY_USE},				//###252
        {"%hhu ",   &db_urms_status3.plan[3], BASE_CHAR, REPLAY_USE},				//###253

        {"%hhu ",   &db_urms_status.metered_lane_stat[0].demand_stat, BASE_CHAR, REPLAY_USE},	//###254
        {"%hhu ",   &db_urms_status.metered_lane_stat[0].passage_stat, BASE_CHAR, REPLAY_USE},	//###255

        {"%hhu ",   &db_urms_status.metered_lane_stat[1].demand_stat, BASE_CHAR, REPLAY_USE},	//###256
        {"%hhu ",   &db_urms_status.metered_lane_stat[1].passage_stat, BASE_CHAR, REPLAY_USE},	//###257

        {"%hhu ",   &db_urms_status.metered_lane_stat[2].demand_stat, BASE_CHAR, REPLAY_USE},	//###258
        {"%hhu ",   &db_urms_status.metered_lane_stat[2].passage_stat, BASE_CHAR, REPLAY_USE},	//###259

        {"%hhu ",   &db_urms_status.metered_lane_stat[3].demand_stat, BASE_CHAR, REPLAY_USE},	//###260
        {"%hhu ",   &db_urms_status.metered_lane_stat[3].passage_stat, BASE_CHAR, REPLAY_USE},	//###261

        {"%hhx ",   &db_urms_status.metered_lane_stat[0].metered_lane_rate_msb, BASE_CHAR, REPLAY_USE},	//###262
        {"%hhx ",   &db_urms_status.metered_lane_stat[0].metered_lane_rate_lsb, BASE_CHAR, REPLAY_USE},	//###263

        {"%hhx ",   &db_urms_status.metered_lane_stat[1].metered_lane_rate_msb, BASE_CHAR, REPLAY_USE},	//###264
        {"%hhx ",   &db_urms_status.metered_lane_stat[1].metered_lane_rate_lsb, BASE_CHAR, REPLAY_USE},	//###265

        {"%hhx ",   &db_urms_status.metered_lane_stat[2].metered_lane_rate_msb, BASE_CHAR, REPLAY_USE},	//###266
        {"%hhx ",   &db_urms_status.metered_lane_stat[2].metered_lane_rate_lsb, BASE_CHAR, REPLAY_USE},	//###267

        {"%hhx ",   &db_urms_status.metered_lane_stat[3].metered_lane_rate_msb, BASE_CHAR, REPLAY_USE},	//###268
        {"%hhx ",   &db_urms_status.metered_lane_stat[3].metered_lane_rate_lsb, BASE_CHAR, REPLAY_USE},	//###269
};

#define NUM_FILE_COLUMNS sizeof(file_spec)/sizeof(data_log_column_spec_t)
int num_file_columns = sizeof(file_spec)/sizeof(data_log_column_spec_t);
