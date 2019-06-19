/*****************************************************************************************

    This files defines all the global variables and parameters
	For simulation: usinng Hybrid Metering

******************************************************************************************/
#pragma once

#include <stdio.h>

#define DYNAMIC_BOUNBDS
//#define DBG_COEFF

//#define NUM_CONTROLLER_VARS 24
	
#define NumOnRamp           	4	// the number of onramp; SR99
#define SecSize             	5	// one more than NumOnRam
#define use_CRM			2	// 1: default; 2: Opt CRM; 3: Coord ALINEA;  4: Hybrid

#define VSL_Update_Step		2	// twice of time length for detection; .g if detection is 60s; VSL will be updated every 120s
#define Occ_thresh		20.0        // 04_20_10; 25: 05_12_12; 12.5 used for I-80 which was good; 15.0 used
#define rm_band			450.0		// default: 400
#define reduction_coeff		0.075
#define increase_coeff		0.15
//#define RELEASE_CYCLE_LEN   10.0
#define RELEASE_OCC		70.0
#define DEMAND_CHANGE       	1           // 0: no change; 1: selective onramp dmd change; 2: all onramp dmd change // 02_20_14
//#define SWITCHING_METERING  1
#define Gain_Up			8.0
#define Gain_Dn			4.0
#define Occ_Cr			12.0
#define OnRamp_Occ_Cr		30.0


//#define CellSize 15
#define ALLvehType          	0
#define ShortSecSize		30
#define NumLnDetector       	27     //
#define NumCrossDetector    	30
#define max_onramp_ln       	3
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
//static int ISUPDATE2=1;
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
//float alpha[SecSize]={0};
//float beta_c[SecSize]={0};
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

static float ln_CRM_rt[NumOnRamp][max_onramp_ln]={{0.0,0.0,0.0},{0.0,0.0,0.0}};
static float ln_LRRM_rt[NumOnRamp][max_onramp_ln]={{0.0,0.0,0.0},{0.0,0.0,0.0}};   // local responsive RM


	static int release_cycle[NumOnRamp][max_onramp_ln]={{0,0,0}};

	static float total_rt[NumOnRamp]={0.0,0.0,0.0,0.0};
	static float total_LRRM_rt[NumOnRamp]={0.0,0.0,0.0,0.0};
	
	static float L[SecSize]={238.9,299.8,824.9,1411.3,968.5};  // composite length; including most downstream sec; 1st section has no meter; 10_21_13
	const float Q[SecSize]={2050.0,2000.0,2050.0,2050.0, 2050.0};	 //onramp flow capacity
	
	const float min_Ln_RM_rt[NumOnRamp]=  {450,480,480,450};   // revised lower bound; 9/28/16
	
	const float max_Ln_RM_rt[NumOnRamp]=  {950,950,950,950}; // revised lower bound  9/28/16
										  
	
	// for downstream 11 onramps only
	const int N_OnRamp_Ln[NumOnRamp]={1,2,2,2};  // from upstream to downstream
	const int N_OffRamp_Ln[NumOnRamp]={1,1,0,0};  // from upstream to downstream
	
	//const int N_Mainline_Ln_RM_All[(NumOnRamp)]={3,3,3};
	//const int N_OnRamp_Ln_All[(NumOnRamp)]={3,1,1};
	
//for measuremnt only
static float onrampL[NumOnRamp]={152.4, 152.4, 304.8, 304.8};

const float lambda[SecSize]={4.0,4.0,4.0,4.0, 4.0}; // composite ln number

const float SR99_RM_occ_tbl[N_interv][NumOnRamp]=
   {{6.0,   5.0,    5.0, 5.0},
    {6.9,   6.1,    6.1, 6.1},
    {7.7,   7.1,    7.1, 7.1},
    {8.6,   8.2,    8.2, 8.2},
    {9.4,   9.2,    9.3, 9.3},
    {10.3,  10.3,   10.4, 10.4},
    {11.1,  11.3,   11.4, 11.4},
    {12.0,  12.4,   12.5, 12.5},
    {12.9,  13.5,   13.6, 13.6},
    {13.7,  14.5,   14.6, 14.6},
    {14.6,  15.6,   15.7, 15.7},
    {15.4,  16.6,   16.8, 16.8},
    {16.3,  17.7,   17.9, 17.9},
    {17.1,  18.7,   18.9, 18.9},
    {18.0,  19.8,   20.0, 20.0}};
const float SR99_RM_rate_tbl[N_interv][NumOnRamp]=
   {{ 1000,    750,  910, 910},
    { 963,    740,   894, 894},
    { 926,    729,   878, 878},
    { 889,    718,   861, 861},
    { 852,    708,   845, 845},
    { 815,    697,   828, 828},
    { 778,    686,   812, 812},
    { 740,    675,   796, 796},
    { 703,    665,   779, 779},
    { 666,    654,   763, 763},
    { 629,    643,   746, 746},
    { 592,    633,   730, 730},
    { 555,    622,   713, 713},
    { 518,    611,   697, 697},
    {480,    600,   681, 681}};

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
