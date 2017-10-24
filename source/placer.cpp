#include "placer.h"
extern "C" {
	#include <umfpack.h>
}

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
	for (i = 0 ; i < n ; i++) {
		printf ("x [%d] = %g\n", i, x [i]) ;	
	}
	return 0;
}

int Placer::place(IC ic, Configholder config) {
	int n=5;
	int Ap [ ] = {0, 2, 5, 9, 10, 12} ;
	int Ai [ ] = { 0, 1, 0, 2, 4, 1, 2, 3, 4, 2, 1, 4} ;
	double Ax [ ] = {2., 3., 3., -1., 4., 4., -3., 1., 2., 2., 6., 1.} ;
	double b [ ] = {8., 45., -3., 3., 19.} ;
	double x[5];

	int placer_status=1;
	LOG(INFO) << "Beginning Placement";

	map<int, vector<int> > nbs_map=config.get_nbs_map();
	vector< vector<int> > blck_to_nets=config.get_blck_to_nets();

	LOG(INFO) << "..-* Shaping Matrix *-..";
	for(vector<int> item : blck_to_nets){
		for(auto const& key: nbs_map) {
			LOG(INFO) << "--looking at net["<<key.first<<"]";
			solve_equation(n,Ap,Ai,Ax,x,b);
			//for()
			//ic.get_blck()
		}
	}



	return placer_status;
}


