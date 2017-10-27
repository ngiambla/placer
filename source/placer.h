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

		float q1_w;
		float q2_w;
		float q3_w;
		float q4_w;

	public:
		Placer();
		~Placer();

		int place(IC &ic, Configholder config);
		int spread(IC &ic, Configholder &config, int iter);
		float get_hpwl();
		int snap_to_grid();
};

#endif