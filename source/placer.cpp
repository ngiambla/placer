#include "placer.h"

Placer::Placer() {

}

Placer::~Placer() {}

int Placer::place(Configholder config_t) {
	
	vector< vector<int> > blck_to_nets = config_t.get_blck_to_nets();
	vector< vector<float> > ref_blcks = config_t.get_ref_blcks();

	LOG(INFO) << "Going formulate nets.";
	for(vector<int> blck_connections : blck_to_nets) {

		LOG(INFO) << "Checking for ["<< blck_connections[0] <<"]";

	}
	return 1;
}


