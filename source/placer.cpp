#include "placer.h"

// void constructors and destructors.
Placer::Placer() {}
Placer::~Placer() {}


int Placer::place(Configholder &config_t) {
	int placer_status=1;

	vector< vector<int> > blck_to_nets = config_t.get_blck_to_nets();
	vector< vector<float> > ref_blcks = config_t.get_ref_blcks();

	iter=0;

	return placer_status;
}


