/*****************************************************************************************

    This files defines all the global variables and parameters
	For simulation: usinng Hybrid Metering

******************************************************************************************/
#pragma once

#include <stdio.h>

#define DYNAMIC_BOUNBDS
//#define DBG_COEFF

	
#define NumOnRamp           11	   // the number of onramp; SR99
#define SecSize             12                // one more than NumOnRam
#define use_CRM				2           // 1: default; 2: Opt CRM; 3: Coord ALINEA;  4: Hybrid

#define VSL_Update_Step		2           // twice of time length for detection; .g if detection is 60s; VSL will be updated every 120s
#define Occ_thresh			20.0        // 04_20_10; 25: 05_12_12; 12.5 used for I-80 which was good; 15.0 used
#define rm_band				450.0		// default: 400
#define reduction_coeff		0.075
#define increase_coeff		0.15
//#define RELEASE_CYCLE_LEN   10.0
#define RELEASE_OCC         70.0
#define DEMAND_CHANGE       1           // 0: no change; 1: selective onramp dmd change; 2: all onramp dmd change // 02_20_14
//#define SWITCHING_METERING  1
#define Gain_Up				8.0
#define Gain_Dn				4.0
#define Occ_Cr				12.0

//#define CellSize 15
#define ALLvehType          0
#define ShortSecSize		30
#define NumLnDetector       27     //
#define NumCrossDetector    30
#define max_onramp_ln       3
#define max_mainline_ln		5
#define dmd_up_t            (30.0*60.0)     //[s]: 6:30am; this needs to be changed dynamically
#define dmd_dwn_t           (120.0*60.0)    //[s]: 8:00am
#define dmd_change			(0.5)			// demand of each onramp incresaed by 5%
#define dmd_slice_len       300             // 5[min]  or 300[s] per slice
#define max_RM_rt			1080             // 1080.0  // Field used Lane Max Rate value
#define min_RM_rt			100             // Field used value
#define N_interv			15              // number of RM rate based occ threshold intervals
#define RM_RELEASE_CYCLE	2
#define Onramp_HOV_Util		0.2				// assume 20% Utility instead 15% as Leo stated

#define Np 2                // for 2-step MPC
const float T=30/3600.0;  // 30s RM control time interval in [hr]

#define Rou_J 210
#define Gama 0.004
#define QM 7200
//static int ISUPDATE=3;         // for VSL update
static int ISUPDATE2=1;
#define Compliance_Level 0.3   // VSL compliance level: 1.0 is full level
#define	exp_flt 0.85           // sensor measure exp filter gain

#define eta 0.5
#define a_tts 1.0              // default: 1.0 OK on 01_01_13;
#define a_ttdM 2.0             //default: 2.0
#define a_ttd 2.0              // default: 2.0
//#define a_w 6.5			       // default: 6.5;   2.0 and 6.5 are the same in effect (1/7/14); not sure what is this? value=?
#define len_str 1280           // for using sprintf_s
#define Omega 20.11             // shockwave back-propagation speed 12.5 mph=20.1km/h
static float a_w[NumOnRamp]={0.0};

#define vsl_gain   15.0        // 20.0 fro I-80;    17=>10; 15.0
#define vsl_p_gain 2.0         // 2.0
#define V_free    112.63         // fre flow speed on I-66
#define Q_ln      2050.0         // lane capacity; 1950 used for I-80
#define	VSL_max	  104.6
#define	VSL_min	  55.0         //10  // 20 & 30 did not work; 15 seems to be good; better than 10
#define gama_3    15.0         // gain of VSL_1 at Sycamore
#define gama_6    15.0         // Lee Highway, Tr 3
#define	alpha6_1  0.618
#define	alpha6_2  0.382

// for OptSolver
#define N (NumOnRamp)*Np
#define M1 (NumOnRamp)*6        /* M1+M2+M3 = M */
#define M2 (NumOnRamp)*2 
#define M3 0
#define NM1M2 (N+M1+M2)
#define M (M1+M2+M3)
#define NP N+1        /* NP >= N+1 */
#define MP M+2        /* MP >= M+2 */
//#define alpha_TTD 65
//#define alpha_TTD_M alpha_TTD*5
// for OptSolver End

static int ControlOn;
static int StateOn=0;
static int StateOff=0;
//static int count=1;

static float u[SecSize]={0};    // For all cells
float alpha[SecSize]={0};
float beta_c[SecSize]={0};
static float q[SecSize]={0.0};   // composite mainline flow
static float v[SecSize]={0.0};   // composite speed for each cell
static float o[SecSize]={0.0};   // composite occupancy for each cell
static float qc[SecSize]={7200};      // mainline capacity; assigned in Init()
static float queue[SecSize]={0.0};      // updated with update_queue();
//float s[SecSize]={0.0};          // off-ramp flow  // changed on 03/04/14
static float s[NumOnRamp]={0.0};          // off-ramp flow
static float R[NumOnRamp]={0.0};   // composite onramp flow;  changed on 01/03/13
static float dmd[NumOnRamp]={0.0};          // onramp demand flow;     changed on 01/03/13
static float Q_o[NumOnRamp]={0.0};  // Onramp capacity; total onramp max RM rate
static float Q_min[NumOnRamp]={0.0};  // Total Onramp minimum RM rate
static float ss[NumOnRamp][Np]={{0.0}};
static float dd[NumOnRamp][Np]={{0.0}};     // onramp dmd
static float pre_w[NumOnRamp]={0};
static float max_occ_all_dwn[NumOnRamp]={0};
static float max_occ_2_dwn[NumOnRamp]={0};

static float q_main[SecSize]={0.0};     // mainline flow of all cells   
static float u2[SecSize]={0};           // speed to feed into the model

static float pre_rho[SecSize]={0};
static float opt_r[SecSize][Np]={{0.0}};
//static float metering_rate_change_time=0.0;
static float up_rho[Np]={0};


unsigned int replication=0;
//static int detectorNum=0,sectionNum=0,nodeNum=0,centroidNum=0;  // moved from data_save.h and readdetectordata.h; 05_29_13

static float dyna_min_r[NumOnRamp]={0.0};
static float dyna_max_r[NumOnRamp]={0.0};
static float Ramp_rt[NumOnRamp]={0.0,0.0,0.0};
static float RM_occ[NumOnRamp]={0.0,0.0,0.0};
//static float RM_occ_All[NumOnRamp+5]={0.0,0.0,0.0};
static float ln_RM_rt[NumOnRamp][max_onramp_ln]={{0.0,0.0,0.0},{0.0,0.0,0.0}};
static float ln_LRRM_rt[NumOnRamp][max_onramp_ln]={{0.0,0.0,0.0},{0.0,0.0,0.0}};   // local responsive RM
const int ln_meteringId[16][max_onramp_ln]={{21044,21045,21046},{19780,0,0},{19782,0,0},{19789,0,0},{21063,21062,21060},
                       {19892,0,19796},{21080,21081,21082},{19924,0,19801},{19936,0,19804},{19811,0,0},{19968,0,19815},
                       {19959,0,19819},{19948,0,19823},{19827,0,0},{19831,0,0},{19836,0,0}};



	static int release_cycle[NumOnRamp][max_onramp_ln]={{0,0,0}};
//	static float actual_r[NumOnRamp][max_onramp_ln]={{0.0,0.0,0.0}};
	static float total_rt[NumOnRamp]={0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
	static float total_field_rt[NumOnRamp]={0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
	
	static float L[SecSize]={555.2,1562.0,455.4,1971.8,440.2,2402.1,427.6,1509.9,374.4,1374.9,377.9,1863.9};  // composite length; including most downstream sec; 1st section has no meter; 10_21_13
	const float Q[SecSize]={2050.0,2000.0,2050.0,2050.0,2050.0,2000.0,2050.0,2050.0,2050.0,2000.0,2000.0,2000.0};	 //onramp flow capacity
	//const float min_Ln_RM_rt[NumOnRamp]={480,600,681,240,500,700,480,480,480,575,800}; // field lower bound
	//const float min_Ln_RM_rt[NumOnRamp]=  {380,500,520,240,400,550,380,380,380,475,550}; // revised lower bound; used Day_1
	//const float min_Ln_RM_rt[NumOnRamp]=  {380,500,520,240,350,500,380,350,380,420,550}; // revised lower bound; 9/27/16
	const float min_Ln_RM_rt[NumOnRamp]=  {350,380,480,240,350,400,350,320,330,420,550};   // revised lower bound; 9/28/16
	//const float max_Ln_RM_rt[NumOnRamp]={1000,750,910,900,900,990,900,900,750,920,1080}; // field upper bound;   used Day_1
	//const float max_Ln_RM_rt[NumOnRamp]=  {1000,750,910,800,800,850,900,850,750,850,1080}; // revised lower bound
	const float max_Ln_RM_rt[NumOnRamp]=  {750,750,800,800,800,850,900,850,750,850,1080}; // revised lower bound  9/28/16
										  
	
	// for downstream 11 onramps only
	const int N_OnRamp_Ln[NumOnRamp]={2,3,2,2,1,2,2,2,1,1,1};  // from upstream to downstream
	const int N_OffRamp_Ln[NumOnRamp]={1,1,0,0,1,1,1,1,1,1,2};  // from upstream to downstream
	const int OnRamp_Ln_Id[NumOnRamp][max_onramp_ln]={{19884,0,17256},{19904,19896,17264},{19916,0,17324},{19928,0,17336},
	                     {17392,0,0},{19964,0,17510},{19955,0,16705},{19940,0,16731},{16785,0,0},{16571,0,0},{16853,0,0}};

	const int CellDetectionId[SecSize]={19791,19795,19798,19800,19803,19810,19814,19818,19822,19826,19830,19834};  // 10_24_13; most downstream not detected
	
	//for all 16 onramps
	const int N_Mainline_Ln_RM_All[(NumOnRamp+5)]={3,3,3,3,3,3,4,3,4,4,4,4,4,4,4,5};
	const int N_OnRamp_Ln_All[(NumOnRamp+5)]={3,1,1,2,3,2,3,2,2,1,2,2,2,1,1,1};
	const int OnRamp_Ln_Id_All[(NumOnRamp+5)][max_onramp_ln]={{19852,19842,17550},{338,0,0},{336,0,0},{19978,0,2218},{19875,19871,17232},{19884,0,17256},{19904,19896,17264},
                         {19916,0,17324},{19928,0,17336},{17392,0,0},{19964,0,17510},{19955,0,16705},{19940,0,16731},{16785,0,0},{16571,0,0},{16853,0,0}};	                                                                                                                                     
	const int Mainline_Ln_Det_Id_All[(NumOnRamp+5)][max_mainline_ln]={{18392,18394,0,0,18390},{18378,18380,0,0,18376},{18371,18373,0,0,18369},{18357,18359,0,0,18355},
				{18667,18669,0,0,18665},{18336,18338,0,0,18334},{18327,18329,18331,0,18325},{18311,18309,0,0,18313},
				{18302,18304,18306,0,18300},{18275,18277,18279,0,18273},{18266,18268,18270,0,18264},{18512,18514,18516,0,18510},
				{18228,18230,18232,0,18226},{18192,18194,18196,0,18190},{18201,18203,18205,0,18199},{18535,18537,18539,18541,18533}}; // all 16 Sections
	
	
//for measuremnt only
static float onrampL[NumOnRamp]={379.2,453.6,587.6,831.7,284.8,615.9,334.6,470.7,402.7,522.7,272.3};
const int OnRampDetectionEndId[NumOnRamp]={19797,20139,19802,19806,19812,19816,20140,19824,19828,19832,19835}; // single detector after meter

const int OnRamp_Ln_AdvanceDetEndId[NumOnRamp][max_onramp_ln]={{19886,0,18424},{19906,19898,18425},{19922,0,18689},{19934,0,18691},
                                  {18434,0,0},{19966,0,18436},{19957,0,18438},{19944,0,18693},{18442,0,0},{18444,0,0},{18446,0,0}};

int Green[(NumOnRamp+5)][max_onramp_ln]={{0,0,0},{0,0,0},{0,0,0},{0,0,0},
                                      {0,0,0},{0,0,0},{0,0,0},{0,0,0},
									  {0,0,0},{0,0,0},{0,0,0},{0,0,0},
									  {0,0,0},{0,0,0},{0,0,0},{0,0,0}};



const int OffRampDetectionId[NumOnRamp]={18423, 0, 18426, 0, 18433, 18435, 18437, 18439,18441,18443,18445}; // OffRamp Matched with OnRamp
const int offramp_secId[NumOnRamp]=     {17246, 0, 17286, 0, 17384, 17500, 16697, 16727,16751,16565,16833}; // OffRamp Matched with OnRamp

const float lambda[SecSize]={3.0000,4.0000,3.3654,4.0000,4.0000, 4.0000,4.0000,4.2945,4.0000,4.1191,5.0000,4.6147}; // composite ln number

const double SR99_RM_occ_tbl[N_interv][NumOnRamp+5]=
   {{ 3.0,      5.0,     5.0,      5.0,    5.0,    4.0,    6.0,   5.0,    5.0,   6.0,  7.0,   8.0,   6.0,   9.0,   7.0,     5.0},
    { 3.9,      5.9,     5.9,      5.7,    5.4,    4.8,    6.9,   6.1,    6.1,   6.3,  7.4,   8.7,   6.9,  10.5,   7.7,     5.9},
    { 4.7,      6.7,     6.8,      6.4,    5.7,    5.6,    7.7,   7.1,    7.1,   6.5,  7.7,   9.4,   7.7,  12.0,   8.5,     6.9},
    { 5.6,      7.6,     7.7,      7.1,    6.1,    6.4,    8.6,   8.2,    8.2,   6.8,  8.1,  10.1,   8.6,  13.5,   9.3,     7.8},
    { 6.4,      8.4,     8.6,      7.9,    6.4,    7.1,    9.4,   9.2,    9.3,   7.0,  8.4,  10.9,   9.4,  15.0,  10.1,     8.8},
    { 7.3,      9.3,     9.5,      8.6,    6.8,    7.9,   10.3,  10.3,   10.4,   7.3,  8.8,  11.6,  10.3,  16.5,  10.9,     9.7},
    { 8.1,     10.1,    10.4,      9.3,    7.1,    8.7,   11.1,  11.3,   11.4,   7.5,  9.1,  12.3,  11.1,  18.0,  11.7,    10.7},
    { 9.0,     11.0,    11.3,     10.0,    7.5,    9.5,   12.0,  12.4,   12.5,   7.8,  9.5,  13.0,  12.0,  19.5,  12.5,    11.6},
    { 9.9,     11.9,    12.1,     10.7,    7.9,   10.3,   12.9,  13.5,   13.6,   8.0,  9.9,  13.7,  12.9,  21.0,  13.2,    12.5},
    {10.7,     12.7,    13.0,     11.4,    8.2,   11.1,   13.7,  14.5,   14.6,   8.3, 10.2,  14.4,  13.7,  22.5,  14.0,    13.5},
    {11.6,     13.6,    13.9,     12.1,    8.6,   11.9,   14.6,  15.6,   15.7,   8.5, 10.6,  15.1,  14.6,  24.0,  14.8,    14.4},
    {12.4,     14.4,    14.8,     12.9,    8.9,   12.6,   15.4,  16.6,   16.8,   8.8, 10.9,  15.9,  15.4,  25.5,  15.6,    15.4},
    {13.3,     15.3,    15.7,     13.6,    9.3,   13.4,   16.3,  17.7,   17.9,   9.0, 11.3,  16.6,  16.3,  27.0,  16.4,    16.3},
    {14.1,     16.1,    16.6,     14.3,    9.6,   14.2,   17.1,  18.7,   18.9,   9.3, 11.6,  17.3,  17.1,  28.5,  17.2,    17.3},
    {15.0,     17.0,    17.5,     15.0,   10.0,   15.0,   18.0,  19.8,   20.0,   9.5, 12.0,  18.0,  18.0,  30.0,  18.0,    18.2}};
const double SR99_RM_rate_tbl[N_interv][NumOnRamp+5]=
   {{ 700,     800,     900,     900,     700,     1000,    750,   910,   900,   900,   990,  900,  900,    750,   920,    1080},
    { 668,     778,     870,     853,     672,      963,    740,   894,   853,   827,   970,  870,  870,    730,   895,    1060},
    { 635,     755,     840,     806,     643,      926,    729,   878,   806,   843,   949,  840,  840,    711,   870,    1040},
    { 602,     732,     810,     759,     615,      889,    718,   861,   759,   815,   928,  810,  810,    692,   846,    1020},
    { 569,     709,     780,     712,     586,      852,    708,   845,   712,   786,   908,  780,  780,    672,   821,    1000},
    { 536,     686,     750,     665,     558,      815,    697,   828,   665,   758,   887,  750,  750,    653,   796,     980},
    { 503,     663,     720,     618,     529,      778,    686,   812,   618,   729,   866,  720,  720,    634,   772,     960},
    { 471,     640,     690,     570,     500,      740,    675,   796,   570,   700,   845,  690,  690,    615,   747,     940},
    { 438,     618,     660,     523,     472,      703,    665,   779,   523,   672,   825,  660,  660,    595,   722,     920},
    { 405,     595,     630,     476,     443,      666,    654,   763,   476,   643,   804,  630,  630,    576,   698,     900},
    { 372,     572,     600,     429,     415,      629,    643,   746,   429,   615,   783,  600,  600,    557,   673,     880},
    { 339,     549,     570,     382,     386,      592,    633,   730,   382,   586,   763,  570,  570,    537,   648,     860},
    { 306,     526,     540,     335,     358,      555,    622,   713,   335,   558,   742,  540,  540,    518,   624,     840},
    { 273,     503,     510,     288,     329,      518,    611,   697,   288,   529,   721,  510,  510,    499,   599,     820},
    { 241,     480,     480,     240,     300,      480,    600,   681,   240,   500,   700,  480,  480,    480,   575,     800}};


int origin[20]={0};
int destination[20]={0};


int InitRealtimeDetection(void);
int InitRealtimeDetection_s(void);	// memory allocation; for control detection, almost the same as InitRealtimeDetection(), just has less detector and not save data
int InitRealTimeSection(void);	//for section measure

// from data_save.h
int open_detector(char* data_saving, unsigned int replic);
int open_section(char* data_saving, unsigned int replic);
int open_system(char* data_saving, unsigned int replic);
int open_detector_instant(char* data_saving, unsigned int replic);
int open_section_instant(char* data_saving, unsigned int replic);
int open_network(char* data_saving, unsigned int replic);
int open_signal(char* data_saving, unsigned int replic);
int open_meter(char* data_saving, unsigned int replic);
int init_data_saving(unsigned int replica);
int get_detIds(void);
int get_sectIds(void);
int get_nodeIds(void);
int read_detector(float);
int read_detector_instant(float);
int read_section(float);
int read_system(float);
int read_section_instant(float);
int read_meter_state(float);
int read_signal_state(float);
int save_networkinfo(char * data_saving, unsigned replic);
//int data_dir(char* data_saving_dir, unsigned int contr_sw);

float detInterval,detInstantInterval,last_det_readtime,last_sect_readtime,sectStatInterval,last_syst_readtime,systStatInterval,last_det_inst_readtime;
float last_sect_inst_readtime,sectInstantInterval;
int N_emission;
//int detectorNum,sectionNum,nodeNum;


// from readdetector.h

typedef struct{
	int vehType;
	float DataTime;
	float flow;	
	float speed;
	float occupancy;
	float density;
	float instant_flow;
	float instant_speed;
	float instant_density;
}detectorData;

typedef struct{
	int detId;
	float practical_flow;  // used for Onramp only
	int detId_ln[max_onramp_ln];
	float ln_flow[max_onramp_ln];
	int sectionId;
	detectorData data[Np]; //
}detData;

typedef struct{
	int vehType;
	float DataTime;
	float flow;
	float speed;
	float density;
	float Harmonic_speed;
}sectionData;

typedef struct{
	int sectionId;
	sectionData data[Np];
}secData;

typedef struct {
	int detId;
	detectorData data[Np];
}data_profile;

detData detection_s_0[SecSize];  
detData detection_onramp_0[NumOnRamp];	//the realtime data for onramp from detector
detData detection_offramp_0[NumOnRamp];	//the realtime data for offramp from detector

detData *detection_s[SecSize]; 
detData *detection_onramp[NumOnRamp];
detData *detection_offramp[NumOnRamp];

int Init();
int Init_sim_data_io();
int Finish_sim_data_io();
int get_u_for_opt();
int det_data_4_contr(float);
int get_s();
int get_q_main();
int update_q_R();
int get_meas(float);
int get_state(float);  
int Set_Default_Meter(float,float,float);
void simplx(float **, int , int, int, int, int, int *, int*, int *);
int opt_metering(void);
//int Set_Hybrid_Meter(float,float,float);
int Set_Opt_Meter();
//int Set_Coord_ALINEA(float,float,float);
int Finish_sim_data_out();
float Mins(float,float);
float Maxs(float,float);
float get_min(float, float);
float Maxd(float,float);
float Mind(float,float);
