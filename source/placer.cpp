#include "placer.h"
extern "C" {
	#include <umfpack.h>
}

//#include "umfpack.h"

// void constructors and destructors.
Placer::Placer() {}
Placer::~Placer() {}



int solve_equation(int n, int * Ap, int * Ai, double * Ax, double * x, double * b) {
	double *null = (double *) NULL ;
	int i ;
	void *Symbolic, *Numeric ;
	(void) umfpack_di_symbolic (n, n, Ap, Ai, Ax, &Symbolic, null, null) ;
	(void) umfpack_di_numeric (Ap, Ai, Ax, Symbolic, &Numeric, null, null) ;
	umfpack_di_free_symbolic (&Symbolic) ;
	(void) umfpack_di_solve (UMFPACK_A, Ap, Ai, Ax, x, b, Numeric, null, null) ;
	umfpack_di_free_numeric (&Numeric) ;
	return 0;
}

int Placer::place(IC ic, Configholder config) {
	int placer_status=1;
	LOG(INFO) << "Beginning Placement";

	return placer_status;
}


