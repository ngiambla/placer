#include "configholder.h"

/* sets configholder's state to represent 
the block to nets (for connections) */
void Configholder::add_blck_to_net(vector<int> blck_to_net) {

	blck_to_nets.push_back(blck_to_net);

	int i=0;
	for(int item : blck_to_net) {
		if(item ==0 ) {
			LOG(ERROR) << "FOUND";
			exit(-1);
		}
		if(i==1) {
			if ( nbs_map.count(item) == 0 ) {
				vector<int> blcks;
				nbs_map[item]=blcks;
				nbs_map[item].push_back(blck_to_net[0]);

			} else {
				nbs_map[item].push_back(blck_to_net[0]);
			}			
		} else {
			i=1;
		}
	}
}

void Configholder::update_blck_to_net(int blckid, int netnum) {
	int i=0;
	for(vector<int> vec: blck_to_nets) {
		if(vec[0]==blckid){
			blck_to_nets[i].push_back(netnum);
		}
		++i;
	}
}

void Configholder::add_ref_blck(vector<float> ref_blck) {
	ref_blcks.push_back(ref_blck);
}


vector< vector<int> > Configholder::get_blck_to_nets() {
	return blck_to_nets;
}

vector< vector<float> > Configholder::get_ref_blcks() {
	return ref_blcks;
}

map<int, vector<int> > Configholder::get_nbs_map() {
	return nbs_map;
}

int Configholder::get_grid_size() {
	return (int)ceil(sqrt(blck_to_nets.size()));
}

void Configholder::display_config() {
	printf("[config] -- ");
	for(const auto& key : nbs_map) {
		printf("[NET] [%d]\n", key.first);
		for(int bnum : key.second) {
			printf("--[BLCK] [%d]\n", bnum);
		}
		printf("\n");
	}

	for(vector<float> v: ref_blcks) {
		for(float i : v){

			printf("%f ", i);
		}
		printf("\n");
	}
}