#ifndef __PLACER_H__
#define __PLACER_H__

#include "pdefs.h"
#include "ic.h"

class Placer {

	private:
		float hpwl;
		map<int, float> cur_x_cuts;
		map<int, float> cur_y_cuts;
		void begin_place();
		void find_sys();
		void calculate_hpwl(IC ic, Configholder config);

		float q1_w, q2_w, q3_w, q4_w; 	//edge weights;
		float wc1, wc2, wc3, wc4;		//class weights;

	public:
		Placer();
		~Placer();

		int place(IC &ic, Configholder config);
		int spread(IC &ic, Configholder &config, int iter);
		float get_hpwl();
		int is_grid_congested(IC ic, Configholder config);
		int snap_to_grid(IC &ic, Configholder config);
};

#endif