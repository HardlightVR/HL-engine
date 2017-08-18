#pragma once
#include "PluginApi.h"

struct nsvr_bodygraph_region {
	nsvr_parallel parallel;
	double rotation;
	double width_cm;
	double height_cm;

	nsvr_bodygraph_region() 
		: parallel{ nsvr_bodypart::nsvr_bodypart_unknown, 0 }
		, rotation {0}
		, width_cm {0}
		, height_cm {0} {}
};

