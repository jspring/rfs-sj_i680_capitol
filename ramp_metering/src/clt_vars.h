#include <db_include.h>
#include <db_utils.h>
#include <path_gps_lib.h>
#include <atsc.h>
#include <atsc_clt_vars.h>
#include <urms.h>
#include "data_log.h"
//#include "wrfiles_rm.h"
#include "variables2.h"
#include "rm_algo.h"

#define DB_166_253_101_111_TYPE	3000	
#define DB_166_255_77_236_TYPE	3200	
#define DB_166_253_101_112_TYPE	3400	
#define DB_166_255_77_235_TYPE	3600	
#define DB_166_253_101_113_TYPE	3800	

#define DB_166_253_101_111_VAR	3000	
#define DB_166_255_77_236_VAR	3200	
#define DB_166_253_101_112_VAR	3400	
#define DB_166_255_77_235_VAR	3600	
#define DB_166_253_101_113_VAR	3800	

db_id_t db_controller_list[] =  {
	{DB_166_253_101_111_VAR, sizeof(db_urms_status_t)},
	{DB_166_255_77_236_VAR, sizeof(db_urms_status_t)},
	{DB_166_253_101_112_VAR, sizeof(db_urms_status_t)},
	{DB_166_255_77_235_VAR, sizeof(db_urms_status_t)},
	{DB_166_253_101_113_VAR, sizeof(db_urms_status_t)},


	{DB_166_253_101_111_VAR + 1, sizeof(urms_datafile_t)},
	{DB_166_255_77_236_VAR + 1, sizeof(urms_datafile_t)},
	{DB_166_253_101_112_VAR + 1, sizeof(urms_datafile_t)},
	{DB_166_255_77_235_VAR + 1, sizeof(urms_datafile_t)},
	{DB_166_253_101_113_VAR + 1, sizeof(urms_datafile_t)},

	{DB_166_253_101_111_VAR + 2, sizeof(db_urms_t)},
	{DB_166_255_77_236_VAR + 2, sizeof(db_urms_t)},
	{DB_166_253_101_112_VAR + 2, sizeof(db_urms_t)},
	{DB_166_255_77_235_VAR + 2, sizeof(db_urms_t)},
	{DB_166_253_101_113_VAR + 2, sizeof(db_urms_t)},

	{DB_166_253_101_111_VAR + 3, sizeof(db_urms_status2_t)},
	{DB_166_255_77_236_VAR + 3, sizeof(db_urms_status2_t)},
	{DB_166_253_101_112_VAR + 3, sizeof(db_urms_status2_t)},
	{DB_166_255_77_235_VAR + 3, sizeof(db_urms_status2_t)},
	{DB_166_253_101_113_VAR + 3, sizeof(db_urms_status2_t)},

	{DB_166_253_101_111_VAR + 4, sizeof(db_urms_status3_t)},
	{DB_166_255_77_236_VAR + 4, sizeof(db_urms_status3_t)},
	{DB_166_253_101_112_VAR + 4, sizeof(db_urms_status3_t)},
	{DB_166_255_77_235_VAR + 4, sizeof(db_urms_status3_t)},
	{DB_166_253_101_113_VAR + 4, sizeof(db_urms_status3_t)},

	{DB_166_253_101_111_VAR + 5, sizeof(db_ramp_data_t)},
	{DB_166_255_77_236_VAR + 5, sizeof(db_ramp_data_t)},
	{DB_166_253_101_112_VAR + 5, sizeof(db_ramp_data_t)},
	{DB_166_255_77_235_VAR + 5, sizeof(db_ramp_data_t)},
	{DB_166_253_101_113_VAR + 5, sizeof(db_ramp_data_t)},
};
#define NUM_CONTROLLER_VARS (sizeof(db_controller_list)/sizeof(db_id_t))


int db_trig_list[] =  {
	DB_166_253_101_111_VAR,
	DB_166_255_77_236_VAR,
	DB_166_253_101_112_VAR,
	DB_166_255_77_235_VAR,
	DB_166_253_101_113_VAR,
};
#define NUM_TRIG_VARS (sizeof(db_trig_list)/sizeof(int))
