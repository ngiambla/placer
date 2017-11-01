#ifndef _IC_H__
#define _IC_H__

#include "pdefs.h"
#include "configholder.h"
#include "blck.h"

typedef vector<Blck> Blcks;

class IC {
	private:
		int grid_size;
		map<int, Blck> blck_map;
		map<int, vector<int> > nbs_map;

	public:
		IC();
		IC(Configholder config);
		void display_blcks();
		Blck &get_blck(int id);
		void update_nbs_map(Configholder config);
		void add_pseudo_block(int blck_id, Blck pblck);
		int get_grid_size();

};

#endif