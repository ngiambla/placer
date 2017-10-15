#ifndef __CONFIGHOLDER_H__
#define __CONFIGHOLDER_H__

#include "pdefs.h"

class Configholder {

	private: 
		vector< vector<int> > blck_to_nets;
		vector< vector<int> > ref_blcks;

	public:
		Configholder(){};


		void add_blck_to_net(vector<int> blck_to_net);
		void add_ref_blck(vector<int> ref_blck);
		vector< vector<int> > get_blck_to_nets();
		vector< vector<int> > get_ref_blcks();
		int get_grid_size();
		void display_config();

};

#endif