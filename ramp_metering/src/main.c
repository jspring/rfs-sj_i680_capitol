//#define _VARIABLES2_H_DECLS	1^M
//#define _VARIABLES_H_DECLS	1^M
//#define _TEMP_GLOBAL_H_DECLS	1^M

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include "variables2.h"
#include "variables.h"
#include "temp_global.h"
#include "interface.h"
#include "init.h"
#include "read_data.h"
#include "control_compute.h"

long int count = 0;

FILE *meter_fp;
FILE *signal_fp;

int test(char filename[])
{
	FILE *fp = fopen(filename,"r");
	char str[1000];
	struct signal_variables *psignal_data = &signal_data;
	struct ramp_variables *pramp_data = &ramp_data;
	int flag;




	if(fp!=NULL)
	{	
		while(fgets(str,sizeof(str),fp)!=NULL)
		{count++;
			get_new_data(str,psignal_data,pramp_data);
			if(ramp_flag == 1)
			{
				flag = read_ramp_data(pramp_data);
				//get new meter rate
				if(flag==0)
				{
					pramp_data->new_meter_rate = get_ALINEA_rate(pramp_data);
					fprintf(meter_fp,"%f\t%f\t%f\t%f\t%d\n",get_current_time(),pramp_data->mainline_lead_occ[NUMBER_RAMP_DATA-1][1],pramp_data->mainline_lead_occ[NUMBER_RAMP_DATA-1][2],pramp_data->new_meter_rate,count);
				}
				ramp_flag = 0;
			}
			read_real_time_data(psignal_data);
			if(signal_flag == 1)
			{
				old_phase6_signal_status = 0;
				//get new max green
				flag=get_intersection_measurement(psignal_data,pramp_data);

				if(flag==0)
				{
					flag=get_new_max_green_phase3(psignal_data,pramp_data);
					fprintf(signal_fp,"%f\t%f\t%f\t%f\t%f\t%d\t%d\n",get_current_time(),psignal_data->LT_occ,psignal_data->RT_occ,pramp_data->queue_occ[NUMBER_RAMP_DATA-1][1],psignal_data->ramp_queue,psignal_data->new_max_green,count);
					if (psignal_data->new_max_green!=psignal_data->last_sent_max_green)
					{
						set_new_max_green_phase3(psignal_data->new_max_green);
						psignal_data->last_sent_max_green=psignal_data->new_max_green;
					}
				}
				psignal_data->data_row=-1;
				signal_flag = 0;

			}
			if(QUEUE_RESET &&  get_current_time() - psignal_data->prev_queue_reset_time >=QUEUE_RESET_INTERVAL && METHOD_FOR_RAMP_QUEUE==1)
			{
				reset_ramp_queue(psignal_data);
				psignal_data->prev_queue_reset_time = get_current_time();
			}
		}
	}
	fclose(fp);
	return 0;
}

int main()
{
	int i;
	char file[200];
	meter_fp=fopen("meter_result.txt","w+");
	signal_fp=fopen("signal_result.txt","w+");

	init_signal(&signal_data);
	init_realtimedata(&signal_data);
	init_ramp_data(&ramp_data);

	for(i=50;i<=90;i++)
	{
		sprintf(file,"%s%d.dat",filename,i);
		test(file);
	}
	fclose(meter_fp);
	fclose(signal_fp);
	return 0;
}
