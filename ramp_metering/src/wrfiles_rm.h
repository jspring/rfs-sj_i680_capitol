#pragma once

typedef struct {
	float mainline_lead_occ[3];
	float mainline_trail_occ[3];
	float queue_occ[3];
	short metering_rate[3];
} IS_PACKED urms_datafile_t;

