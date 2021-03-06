#include "ic.h"
#include <iostream>

IC::IC() {
	
}

IC::IC(Configholder config) {
	int i=0;
	int size;
	this->grid_size=config.get_grid_size();
	nbs_map = config.get_nbs_map();
	
	for(vector<int> row : config.get_blck_to_nets()) {
		Blck blck;
		for(i=1; i< row.size(); ++i) {
			size=nbs_map[row[i]].size();
			blck.add_edge_weight(row[i], (float)2/size, size-1);
		}
		blck_map[row[0]]=blck;
	}
	for(vector<float> row : config.get_ref_blcks()) {
		int id=(int)row[0];
		blck_map[id].set_x(row[1]);
		blck_map[id].set_y(row[2]);
		blck_map[id].set_fixed();
	}

}

void IC::display_blcks() {

	for(const auto& key : blck_map) {
		blck_map[key.first].display_blck();
	}
}

void IC::update_nbs_map(Configholder config) {
	nbs_map=config.get_nbs_map();
}

void IC::add_pseudo_block(int blck_id, Blck pblck) {
	blck_map[blck_id]=pblck;
}

Blck& IC::get_blck(int id) {
	return (Blck&)blck_map[id];
}

int IC::get_grid_size() {
	return grid_size;
}