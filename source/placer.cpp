#include "placer.h"
#include "iostream"
extern "C" {
	#include "umfpack.h"
}

// void constructors and destructors.
Placer::Placer() {}
Placer::~Placer() {}



int solve_equation(int n, int * Ap, int * Ai, double * Ax, double * x, double * b, int which_var) {
	double *null = (double *) NULL ;
	void *Symbolic, *Numeric ;
	(void) umfpack_di_symbolic (n, n, Ap, Ai, Ax, &Symbolic, null, null) ;
	(void) umfpack_di_numeric (Ap, Ai, Ax, Symbolic, &Numeric, null, null) ;
	umfpack_di_free_symbolic (&Symbolic) ;
	(void) umfpack_di_solve (UMFPACK_A, Ap, Ai, Ax, x, b, Numeric, null, null) ;
	umfpack_di_free_numeric (&Numeric) ;
	return 0;
}

void Placer::calculate_hpwl(IC ic, Configholder config) {
	float min_x=ic.get_grid_size(), max_x=0;
	float min_y=ic.get_grid_size(), max_y=0;

	float bx, by;

	for(vector<int> row : config.get_blck_to_nets()) {
		Blck b=ic.get_blck(row[0]);
		if(b.is_pseudo() == 0) {
			bx=b.get_x();
			by=b.get_y();
			

			if(bx<=min_x) { //find min x;
				min_x=bx;
			}
			if(by<=min_y) { // find min y;
				min_y=by;
			}

			if(bx>=max_x) { // find max x;
				max_x=bx;
			}

			if(by>=min_y) { // find max y;
				max_y=by;
			}
		}
	}

	hpwl=(max_x-min_x)+(max_y-min_y);
}

int Placer::place(IC &ic, Configholder config) {

	int n=config.get_blck_to_nets().size() - config.get_ref_blcks().size();
	int nz;
	int * Ti, * Ai;
	int * Tj, * Ap;
	double * Tx, *Ax;
	double * b_x;
	double * b_y;
	double * x;
	int status, i, cur_idx=0, cur_col=0;

	float cur_b_x=0;
	float cur_b_y=0;


	vector<int> Ti_t;
	vector<int> Tj_t;
	vector<double> Tx_t;

	map<int, int> blck_to_idx;
	map<int, int> idx_to_blck;
	map< pair<int, int>, float> check_entry;

	vector<double> b_x_t;
	vector<double> b_y_t;

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

			Ti_t.push_back(blck_to_idx[item[0]]);		// add sum of block egdes to diag;
			Tj_t.push_back(blck_to_idx[item[0]]);
			Tx_t.push_back(b.get_total_weight());

			for(i=1 ; i< item.size(); ++i) {
				for(int bnum : nbs_map[item[i]]) {
					if(bnum != item[0] && ic.get_blck(bnum).is_fixed()==0) {
						if(check_entry.count(make_pair(blck_to_idx[bnum], cur_col))>0) {
							check_entry[make_pair(blck_to_idx[bnum], cur_col)]=check_entry[make_pair(blck_to_idx[bnum], cur_col)]-b.get_net_weight(item[i]);
						} else {
							check_entry[make_pair(blck_to_idx[bnum], cur_col)]=-b.get_net_weight(item[i]);							
						}
					}
					if(bnum != item[0] && ic.get_blck(bnum).is_fixed()==1) {
						Blck b3 = ic.get_blck(bnum);
						cur_b_x=cur_b_x+b3.get_x()*b.get_net_weight(item[i]);
						cur_b_y=cur_b_y+b3.get_y()*b.get_net_weight(item[i]);	

					}
				}		
			}
			b_x_t.push_back(cur_b_x);
			b_y_t.push_back(cur_b_y);
			cur_b_x=0;
			cur_b_y=0;
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

	b_x=&b_x_t[0];
	b_y=&b_y_t[0];

	Ap=new int[n+1];
	Ai=new int[nz];
	Ax=new double[nz];
	x=new double[n];

	LOG(INFO) << "-- Non Zero Entries: "<< nz;

	status=umfpack_di_triplet_to_col(n, n, nz, Ti, Tj, Tx, Ap, Ai, Ax, NULL);
	if (status != UMFPACK_OK) {
		return EXIT_FAILURE;
	}



	LOG(INFO) << "-- Solving for X";
	solve_equation(n, Ap, Ai, Ax, x, b_x, 0);		// indicates x (only for semantic rep);

	LOG(INFO) << "About to modify X.";
	for(i=0; i< n; ++i) {
		ic.get_blck(idx_to_blck[i]).set_x(x[i]);
	}
	
	LOG(INFO) << "-- Solving for Y";
	solve_equation(n, Ap, Ai, Ax, x, b_y, 1);		// indicates y '''''''''''''	

	LOG(INFO) << "About to modify Y";
	for(i=0; i< n; ++i) {
		ic.get_blck(idx_to_blck[i]).set_y(x[i]);
	}

	calculate_hpwl(ic, config);

	LOG(INFO) << "Complete.";

	delete(Ap);					//free these guys
	delete(Ai);
	delete(Ax);
	delete(x);

	return placer_status;
}

//
//	[1][2]	--recursive cutting in increasing order.
//	[4][3]			
//
int Placer::spread(IC &ic, Configholder &config, int iter) {
	int spread_status=1;
	int i=0, j=0, last_net_id, last_blck_id;


	map<int, float> next_x_cuts;
	map<int, float> next_y_cuts;

	if(iter==0) {
		cur_x_cuts[0]=(float)ic.get_grid_size()/2;
		cur_y_cuts[0]=(float)ic.get_grid_size()/2;
	}

	LOG(INFO) << "Iteration: "<< iter;
	for(i=0; i<cur_x_cuts.size(); ++i) {

		// define new pseudo blocks for expansion.
		Blck b_1;
		Blck b_2;
		Blck b_3;
		Blck b_4;


		// fill in pseudos blocks;
		vector<float> new_refs;

		vector< vector<int> > new_blcks_to_net;
		vector<int> row;
		
		for(j=0; j<4; ++j){
			new_blcks_to_net.push_back(row);
		}



		b_1.set_x(cur_x_cuts[i]/2);
		b_1.set_y(cur_y_cuts[i]/2);
		b_1.set_pseudo();

		new_refs.clear();
		new_refs.push_back(config.get_blck_to_nets().size());
		new_refs.push_back(b_1.get_x());
		new_refs.push_back(b_1.get_y());
		new_blcks_to_net[0].push_back(config.get_blck_to_nets().size());

		config.add_ref_blck(new_refs);

		b_2.set_x(cur_x_cuts[i]+cur_x_cuts[i]/2);
		b_2.set_y(cur_y_cuts[i]/2);
		b_2.set_pseudo();

		new_refs.clear();
		new_refs.push_back(config.get_blck_to_nets().size()+1);
		new_refs.push_back(b_2.get_x());
		new_refs.push_back(b_2.get_y());
		new_blcks_to_net[1].push_back(config.get_blck_to_nets().size()+1);

		config.add_ref_blck(new_refs);

		b_3.set_x(cur_x_cuts[i]+cur_x_cuts[i]/2);
		b_3.set_y(cur_y_cuts[i]+cur_y_cuts[i]/2);
		b_3.set_pseudo();

		new_refs.clear();
		new_refs.push_back(config.get_blck_to_nets().size()+2);
		new_refs.push_back(b_3.get_x());
		new_refs.push_back(b_3.get_y());
		new_blcks_to_net[2].push_back(config.get_blck_to_nets().size()+2);

		config.add_ref_blck(new_refs);

		b_4.set_x(cur_x_cuts[i]/2);
		b_4.set_y(cur_y_cuts[i]+cur_y_cuts[i]/2);
		b_4.set_pseudo();

		new_refs.clear();
		new_refs.push_back(config.get_blck_to_nets().size()+3);
		new_refs.push_back(b_4.get_x());
		new_refs.push_back(b_4.get_y());
		new_blcks_to_net[3].push_back(config.get_blck_to_nets().size()+3);

		config.add_ref_blck(new_refs);

		// set blcks to classes.
		last_net_id=config.get_nbs_map().rbegin()->first + 1;

		for(vector<int> row : config.get_blck_to_nets()) {
			Blck b = ic.get_blck(row[0]);
			if(b.is_fixed()==0 && b.is_pseudo() ==0) {
				if(b.get_x() <= cur_x_cuts[i] && b.get_y() <= cur_y_cuts[i]) {
					config.update_blck_to_net(row[0], last_net_id);
					b.add_edge_weight(last_net_id, 1, 1);
					b_1.add_edge_weight(last_net_id, 1, 1);
					new_blcks_to_net[0].push_back(last_net_id);
					LOG(INFO) << "Class [1]";
				} else if(b.get_x() > cur_x_cuts[i] && b.get_y() <= cur_y_cuts[i]) {
					config.update_blck_to_net(row[0], last_net_id);
					b.add_edge_weight(last_net_id, 1, 1);
					b_2.add_edge_weight(last_net_id, 1, 1);
					new_blcks_to_net[1].push_back(last_net_id);
					LOG(INFO) << "Class [2]";
				} else if(b.get_x() <= cur_x_cuts[i] && b.get_y() > cur_y_cuts[i]) {
					config.update_blck_to_net(row[0], last_net_id);
					b.add_edge_weight(last_net_id, 1, 1);
					b_3.add_edge_weight(last_net_id, 1, 1);
					new_blcks_to_net[2].push_back(last_net_id);
					LOG(INFO) << "Class [4]";
				} else if(b.get_x() > cur_x_cuts[i] && b.get_y() > cur_y_cuts[i]) {
					config.update_blck_to_net(row[0], last_net_id);
					b.add_edge_weight(last_net_id, 1, 1);
					b_4.add_edge_weight(last_net_id, 1, 1);
					new_blcks_to_net[3].push_back(last_net_id);
					LOG(INFO) << "Class [3]";
				} else {
					LOG(INFO) << "Unclassified";
				}
				last_net_id++;
			}
		}

		for(j=0; j<4; ++j) {
			if(new_blcks_to_net[j].size() > 1) {
				switch(j) {
					case 0:
						ic.add_pseudo_block(new_blcks_to_net[j][0], b_1);
						break;
					case 1:
						ic.add_pseudo_block(new_blcks_to_net[j][0], b_2);
						break;
					case 2:
						ic.add_pseudo_block(new_blcks_to_net[j][0], b_3);
						break;
					case 3:
						ic.add_pseudo_block(new_blcks_to_net[j][0], b_4);
						break;
				}
				config.add_blck_to_net(new_blcks_to_net[j]);
			} else {
				LOG(ERROR) << "No config added.";
			}
		}



		// define new
		next_x_cuts[0+i*4]=cur_x_cuts[i]/2;
		next_x_cuts[1+i*4]=cur_x_cuts[i]+cur_x_cuts[i]/2;
		next_x_cuts[2+i*4]=cur_x_cuts[i]+cur_x_cuts[i]/2; 
		next_x_cuts[3+i*4]=cur_x_cuts[i]/2;

		next_y_cuts[0+i*4]=cur_y_cuts[i]/2;
		next_y_cuts[1+i*4]=cur_y_cuts[i]/2;
		next_y_cuts[2+i*4]=cur_y_cuts[i]+cur_y_cuts[i]/2;
		next_y_cuts[3+i*4]=cur_y_cuts[i]+cur_y_cuts[i]/2;


	}
	// purge cuts;
	cur_y_cuts.clear();
	cur_x_cuts.clear();

	// instantiate new cuts;
	cur_x_cuts=next_x_cuts;
	cur_y_cuts=next_y_cuts;

	ic.update_nbs_map(config);

	LOG(INFO) << "New cuts to make [x]: " <<cur_x_cuts.size();
	LOG(INFO) << "New cuts to make [y]: " <<cur_y_cuts.size();

	return spread_status;
}

float Placer::get_hpwl() {
	return hpwl;
}


int Placer::snap_to_grid() {
	return 0;
}

