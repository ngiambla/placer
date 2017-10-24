#ifndef _IC_H__
#define _IC_H__

#include "pdefs.h"
#include "configholder.h"
#include "blck.h"

typedef vector<Blck> Blcks;

class IC {
	private:
		Blcks blcks;
		map<int, vector<int> > nbs_map;

	public:
		IC(Configholder config);
		void display_blcks();
		Blck &get_blck(int id);
};

#endif