#ifndef __PLACER_H__
#define __PLACER_H__

#include "pdefs.h"
#include "configholder.h"

class Placer {

	private:
		int iter;
		void begin_place();
		void find_sys();

	public:
		Placer();
		~Placer();

		int place(Configholder config_t);

};

#endif