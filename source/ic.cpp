#include "ic.h"

IC::IC(Configholder config) {
	int i=0;
	int size;
	nbs_map = config.get_nbs_map();
	
	for(vector<int> row : config.get_blck_to_nets()) {
		Blck blck(row[0]-1);
		for(i=1; i< row.size(); ++i) {
			size=nbs_map[row[i]-1].size();
			blck.add_edge_weight(row[i]-1, (float)2/size, size);
		}

		blcks.push_back(blck);
	}
	for(vector<float> row : config.get_ref_blcks()) {
		int id=(int)row[0]-1;
		blcks[id].set_x(row[1]);
		blcks[id].set_y(row[2]);
		blcks[id].set_fixed();
	}

}

void IC::display_blcks() {
	for(Blck b : blcks) {
		b.display_blck();
	}
}