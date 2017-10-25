#include "placer.h"
#include "iostream"
extern "C" {
	#include "umfpack.h"
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

	int n=config.get_blck_to_nets().size() - config.get_ref_blcks().size();
	int nz;
	int * Ti, * Ai;
	int * Tj, * Ap;
	double * Tx, *Ax;
	int * b;
	int status, i, cur_idx=0, cur_col=0;

	float cur_b=0;

	vector<int> Ti_t;
	vector<int> Tj_t;
	vector<double> Tx_t;

	map<int, int> blck_to_idx;
	map<int, int> idx_to_blck;
	map< pair<int, int>, float> check_entry;

	vector<float> b_t;

	int placer_status=1;
	LOG(INFO) << "Beginning Placement";
	LOG(INFO) << "Matrix Size: ["<<n<<"]";


	map<int, vector<int> > nbs_map=config.get_nbs_map();
	vector< vector<int> > blck_to_nets=config.get_blck_to_nets();

	LOG(INFO) << "..-* Shaping Matrix *-..";
	for(vector<int> item : blck_to_nets){				// get blck, and check connected nets
		Blck b = ic.get_blck(item[0]);					// extract blck from ic
		if(b.is_fixed() == 0) {	
			blck_to_idx[item[0]]=cur_idx;				// make blocks such that there is a relative space between matrix i.e 
			idx_to_blck[cur_idx]=item[0];
			++cur_idx;
		}
	}

	for(vector<int> item : blck_to_nets){				// get blck, and check connected nets
		Blck b = ic.get_blck(item[0]);					// extract blck from ic
		if(b.is_fixed() == 0) {							// make sure it is not fixed (not included in the matrix to solve)

			//LOG(INFO) << "Checking block: "<<item[0];
			//b.display_blck();
			//cin.ignore();
			Ti_t.push_back(blck_to_idx[item[0]]);		// add sum of block egdes to diag;
			Tj_t.push_back(blck_to_idx[item[0]]);
			Tx_t.push_back(b.get_total_weight());

			for(i=1 ; i< item.size(); ++i) {
				for(int bnum : nbs_map[item[i]]) {
					if(bnum != item[0] && ic.get_blck(bnum).is_fixed()==0) {
						if(check_entry.count(make_pair(blck_to_idx[bnum], cur_col))>0) {
							check_entry[make_pair(blck_to_idx[bnum], cur_col)]=check_entry[make_pair(blck_to_idx[bnum], cur_col)]+b.get_net_weight(item[i]);
						} else {
							check_entry[make_pair(blck_to_idx[bnum], cur_col)]=b.get_net_weight(item[i]);							
						}
					}
					if(bnum != item[0] && ic.get_blck(bnum).is_fixed()==1) {
						Blck b3 = ic.get_blck(bnum);
						//cin.ignore();
						cur_b=cur_b+b3.get_x()*b.get_net_weight(item[i]);	
					}
				}		
			}
			b_t.push_back(cur_b);
			cur_b=0;
			++cur_col;
		} 
	}

	for(const auto &key : check_entry) {
		Ti_t.push_back(key.first.first);
		Tj_t.push_back(key.first.second);
		Tx_t.push_back(key.second);
	}

	LOG(INFO) << "-- Compressing Matrix.";
	nz=Tx_t.size();
	
	Ti=&Ti_t[0];
	Tj=&Tj_t[0];
	Tx=&Tx_t[0];

	Ap=new int[n+1];
	Ai=new int[nz];
	Ax=new double[nz];

	LOG(INFO) << "-- Non Zero Entries: "<< nz;

	for(i=0; i< b_t.size(); ++i) {
		LOG(INFO) << "["<< b_t[i] <<"]\n";
	}
	// for(i=0; i< Ti_t.size(); ++i) {
	// 	LOG(INFO) << "Value ["<<Tx_t[i]<<"] @i["<< Ti_t[i] <<"]j["<< Tj_t[i] <<"]";
	// }
	status=umfpack_di_triplet_to_col(n, n, nz, Ti, Tj, Tx, Ap, Ai, Ax, NULL);
	if (status != UMFPACK_OK) {
		return EXIT_FAILURE;
	}
	LOG(INFO) << "-- Sending to solver";

	//solve_equation();
	LOG(INFO) << "Complete.";

	delete(Ap);					//free these guys
	delete(Ai);
	delete(Ax);

	return placer_status;
}


