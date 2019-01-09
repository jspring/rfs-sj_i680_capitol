#include <db_include.h>
#include <db_utils.h>
#include <path_gps_lib.h>
#include <atsc.h>
#include <atsc_clt_vars.h>
#include <urms.h>
#include "data_log.h"
//#include "wrfiles_rm.h"
#include "variables2.h"
#include "rm_algo.h"r
#include "ab3418_lib.h"
#include "ab3418comm.h"


#define DB_166_253_101_113_TYPE	3000	
#define DB_166_253_101_111_TYPE	3200	
#define DB_166_255_77_236_TYPE	3400	
#define DB_166_253_101_112_TYPE	3600	
#define DB_166_255_77_235_TYPE	3800	

#define DB_166_253_101_113_VAR	DB_166_253_101_113_TYPE
#define DB_166_253_101_111_VAR	DB_166_253_101_111_TYPE
#define DB_166_255_77_236_VAR	DB_166_255_77_236_TYPE
#define DB_166_253_101_112_VAR	DB_166_253_101_112_TYPE
#define DB_166_255_77_235_VAR	DB_166_255_77_235_TYPE

db_id_t db_controller_list[] =  {
	{DB_166_253_101_113_VAR, sizeof(db_urms_status_t)},
	{DB_166_253_101_111_VAR, sizeof(db_urms_status_t)},
	{DB_166_255_77_236_VAR, sizeof(db_urms_status_t)},
	{DB_166_253_101_112_VAR, sizeof(db_urms_status_t)},
	{DB_166_255_77_235_VAR, sizeof(db_urms_status_t)},


	{DB_166_253_101_113_VAR + 1, sizeof(urms_datafile_t)},
	{DB_166_253_101_111_VAR + 1, sizeof(urms_datafile_t)},
	{DB_166_255_77_236_VAR + 1, sizeof(urms_datafile_t)},
	{DB_166_253_101_112_VAR + 1, sizeof(urms_datafile_t)},
	{DB_166_255_77_235_VAR + 1, sizeof(urms_datafile_t)},

	{DB_166_253_101_113_VAR + 2, sizeof(db_urms_t)},
	{DB_166_253_101_111_VAR + 2, sizeof(db_urms_t)},
	{DB_166_255_77_236_VAR + 2, sizeof(db_urms_t)},
	{DB_166_253_101_112_VAR + 2, sizeof(db_urms_t)},
	{DB_166_255_77_235_VAR + 2, sizeof(db_urms_t)},

	{DB_166_253_101_113_VAR + 3, sizeof(db_urms_status2_t)},
	{DB_166_253_101_111_VAR + 3, sizeof(db_urms_status2_t)},
	{DB_166_255_77_236_VAR + 3, sizeof(db_urms_status2_t)},
	{DB_166_253_101_112_VAR + 3, sizeof(db_urms_status2_t)},
	{DB_166_255_77_235_VAR + 3, sizeof(db_urms_status2_t)},

	{DB_166_253_101_113_VAR + 4, sizeof(db_urms_status3_t)},
	{DB_166_253_101_111_VAR + 4, sizeof(db_urms_status3_t)},
	{DB_166_255_77_236_VAR + 4, sizeof(db_urms_status3_t)},
	{DB_166_253_101_112_VAR + 4, sizeof(db_urms_status3_t)},
	{DB_166_255_77_235_VAR + 4, sizeof(db_urms_status3_t)},

	{DB_166_253_101_113_VAR + 5, sizeof(db_ramp_data_t)},
	{DB_166_253_101_111_VAR + 5, sizeof(db_ramp_data_t)},
	{DB_166_255_77_236_VAR + 5, sizeof(db_ramp_data_t)},
	{DB_166_253_101_112_VAR + 5, sizeof(db_ramp_data_t)},
	{DB_166_255_77_235_VAR + 5, sizeof(db_ramp_data_t)},
};
#define NUM_CONTROLLER_VARS (sizeof(db_controller_list)/sizeof(db_id_t))


int db_trig_list[] =  {
	DB_166_253_101_113_VAR,
	DB_166_253_101_111_VAR,
	DB_166_255_77_236_VAR,
	DB_166_253_101_112_VAR,
	DB_166_255_77_235_VAR,
};
#define NUM_TRIG_VARS (sizeof(db_trig_list)/sizeof(int))

#define DB_10_192_131_9_TYPE 	4000
#define DB_10_192_131_8_TYPE 	4100
#define DB_10_192_131_60_TYPE 	4200
#define DB_10_192_131_7_TYPE 	4300
#define DB_10_192_131_6_TYPE 	4400
#define DB_10_192_131_1_TYPE 	4500
#define DB_10_192_131_10_TYPE 	4600
#define DB_10_192_131_11_TYPE 	4700
#define DB_10_192_131_12_TYPE 	4800
#define DB_10_192_131_13_TYPE 	4900
#define DB_10_192_131_14_TYPE 	5000
#define DB_10_192_131_15_TYPE 	5100
#define DB_10_192_131_16_TYPE 	5200
#define DB_10_192_131_59_TYPE 	5300

#define DB_10_192_131_9_VAR	 DB_10_192_131_9_TYPE
#define DB_10_192_131_8_VAR	 DB_10_192_131_8_TYPE
#define DB_10_192_131_60_VAR	 DB_10_192_131_60_TYPE
#define DB_10_192_131_7_VAR	 DB_10_192_131_7_TYPE
#define DB_10_192_131_6_VAR	 DB_10_192_131_6_TYPE
#define DB_10_192_131_1_VAR	 DB_10_192_131_1_TYPE
#define DB_10_192_131_10_VAR	 DB_10_192_131_10_TYPE
#define DB_10_192_131_11_VAR	 DB_10_192_131_11_TYPE
#define DB_10_192_131_12_VAR	 DB_10_192_131_12_TYPE
#define DB_10_192_131_13_VAR	 DB_10_192_131_13_TYPE
#define DB_10_192_131_14_VAR	 DB_10_192_131_14_TYPE
#define DB_10_192_131_15_VAR	 DB_10_192_131_15_TYPE
#define DB_10_192_131_16_VAR	 DB_10_192_131_16_TYPE
#define DB_10_192_131_59_VAR	 DB_10_192_131_59_TYPE

db_id_t db_arterial_list[] =  {


        {DB_10_192_131_9_VAR, sizeof(get_long_status8_resp_mess_typ)},
        {DB_10_192_131_8_VAR, sizeof(get_long_status8_resp_mess_typ)},
        {DB_10_192_131_60_VAR, sizeof(get_long_status8_resp_mess_typ)},
        {DB_10_192_131_7_VAR, sizeof(get_long_status8_resp_mess_typ)},
        {DB_10_192_131_6_VAR, sizeof(get_long_status8_resp_mess_typ)},
        {DB_10_192_131_1_VAR, sizeof(get_long_status8_resp_mess_typ)},
        {DB_10_192_131_10_VAR, sizeof(get_long_status8_resp_mess_typ)},
        {DB_10_192_131_11_VAR, sizeof(get_long_status8_resp_mess_typ)},
        {DB_10_192_131_12_VAR, sizeof(get_long_status8_resp_mess_typ)},
        {DB_10_192_131_13_VAR, sizeof(get_long_status8_resp_mess_typ)},
        {DB_10_192_131_14_VAR, sizeof(get_long_status8_resp_mess_typ)},
        {DB_10_192_131_15_VAR, sizeof(get_long_status8_resp_mess_typ)},
        {DB_10_192_131_16_VAR, sizeof(get_long_status8_resp_mess_typ)},
        {DB_10_192_131_59_VAR, sizeof(get_long_status8_resp_mess_typ)},

        {DB_10_192_131_9_VAR+1, sizeof(db_set_pattern_t)},
        {DB_10_192_131_8_VAR+1, sizeof(db_set_pattern_t)},
        {DB_10_192_131_60_VAR+1, sizeof(db_set_pattern_t)},
        {DB_10_192_131_7_VAR+1, sizeof(db_set_pattern_t)},
        {DB_10_192_131_6_VAR+1, sizeof(db_set_pattern_t)},
        {DB_10_192_131_1_VAR+1, sizeof(db_set_pattern_t)},
        {DB_10_192_131_10_VAR+1, sizeof(db_set_pattern_t)},
        {DB_10_192_131_11_VAR+1, sizeof(db_set_pattern_t)},
        {DB_10_192_131_12_VAR+1, sizeof(db_set_pattern_t)},
        {DB_10_192_131_13_VAR+1, sizeof(db_set_pattern_t)},
        {DB_10_192_131_14_VAR+1, sizeof(db_set_pattern_t)},
        {DB_10_192_131_15_VAR+1, sizeof(db_set_pattern_t)},
        {DB_10_192_131_16_VAR+1, sizeof(db_set_pattern_t)},
        {DB_10_192_131_59_VAR+1, sizeof(db_set_pattern_t)},

};
#define NUM_ARTERIAL_VARS (sizeof(db_arterial_list)/sizeof(db_id_t))
