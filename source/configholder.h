#ifndef __CONFIGHOLDER_H__
#define __CONFIGHOLDER_H__

#include "pdefs.h"

class Configholder {

	private: 
		vector< vector<int> > blck_to_nets;
		vector< vector<float> > ref_blcks;
		map<int, vector<int> > nbs_map;

	public:
		Configholder(){};


		void add_blck_to_net(vector<int> blck_to_net);
		void add_ref_blck(vector<float> ref_blck);
		vector< vector<int> > get_blck_to_nets();
		vector< vector<float> > get_ref_blcks();
		map<int, vector<int> > get_nbs_map();
		int get_grid_size();
		void display_config();
		void update_blck_to_net(int blckid, int netnum);

};

#endif