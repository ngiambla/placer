#include "placer.h"

// void constructors and destructors.
Placer::Placer() {}
Placer::~Placer() {}

void assign_cost(map<int, vector<int> > &nbs_map_t) {
	LOG(INFO) << "-- assiging cost....";
}

void clique_model_assignment(map<int, vector<int> > &nbs_map_t) {
	
}

void send_net_for_analysis() {
	LOG(DEBUG) << "-- sending Net [] for analysis";
}

int Placer::place(Configholder &config_t) {
	int placer_status=1;

	vector< vector<int> > blck_to_nets = config_t.get_blck_to_nets();
	vector< vector<float> > ref_blcks = config_t.get_ref_blcks();

	iter=0;

	return placer_status;
}


