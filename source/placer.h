#ifndef __PLACER_H__
#define __PLACER_H__

#include "pdefs.h"
#include "ic.h"

class Placer {

	private:
		float hpwl;
		void begin_place();
		void find_sys();
		void calculate_hpwl(IC ic, Configholder config);

	public:
		Placer();
		~Placer();

		int place(IC &ic, Configholder config);
		int spread(IC &ic, int iter);
		float get_hpwl();

};

#endif