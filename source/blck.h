#ifndef __BLCK_H__
#define __BLCK_H__

#include "pdefs.h"


class Blck {
	private:
		map<int, vector<float> > net_w_expansion;

		float x;
		float y;

		int isFixed;
		int isPseudo;

		map<int, int> net_history;
		int classNum;
		int isStale;

	public:
		Blck();

		float get_x();
		float get_y();

		void set_x(float x);
		void set_y(float y);

		float get_net_weight(int netNum);
		void update_pseudo_blck_weight();

		void add_edge_weight(int netNum, float weight, int howmany);
		float get_total_weight();

		void set_fixed();
		int is_fixed();

		void set_pseudo();
		int is_pseudo();

		int belongs_to();
		void set_to_group(int group_id);
		void refresh();
		int is_stale();

		void display_blck();
		void display_pos(int id);
};

#endif