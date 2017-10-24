#include "placer.h"
#include "umfpack.h"

// void constructors and destructors.
Placer::Placer() {}
Placer::~Placer() {}



int formulate_equation(int * ap, int * ai, double * Ax) {
	return 0;
}


int Placer::place(IC ic, Configholder config) {
	int placer_status=1;
	LOG(INFO) << "Beginning Placement";
	return placer_status;
}


