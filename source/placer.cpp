#include "placer.h"
#include "iostream"
extern "C" {
	#include "umfpack.h"
}
#include <unordered_map>
#include <random>

struct pair_hash {
    template <class T1, class T2>
    std::size_t operator () (const std::pair<T1,T2> &p) const {
        auto h1 = std::hash<T1>{}(p.first);
        auto h2 = std::hash<T2>{}(p.second);

        // Mainly for demonstration purposes, i.e. works but is overly simple
        // In the real world, use sth. like boost.hash_combine
        return h1 ^ h2;  
    }
};


// void constructors and destructors.
Placer::Placer() {
	q1_w=0.5;
	q2_w=0.5;
	q3_w=0.5;
	q4_w=0.5;

	wc1=3;			// weight per class;
	wc2=3; 
	wc3=3; 
	wc4=3;

}

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
	float sum=0;

	map<int, vector<int>> nbs_map_t=config.get_nbs_map();
	for(const auto &key : nbs_map_t) {
		float min_x=ic.get_grid_size(), max_x=0;
		float min_y=ic.get_grid_size(), max_y=0;
		float bx, by;

		for(int i : key.second) {
			Blck b=ic.get_blck(i);
			if(b.is_pseudo()==0) {
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

				if(by>=max_y) { // find max y;
					max_y=by;
				}
			}
		}
		if(max_x==0 && max_y==0 && min_x == ic.get_grid_size() && min_y == ic.get_grid_size()) {

		} else {
			sum=sum+(max_x-min_x)+(max_y-min_y);
		}
	}
	hpwl=sum;
}

int Placer::place(IC &ic, Configholder config) {

	int n=config.get_blck_to_nets().size() - config.get_ref_blcks().size();
	LOG(INFO) << "Number of Blocks " << config.get_blck_to_nets().size();
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
						cur_b_x=cur_b_x+b3.get_x()*b3.get_net_weight(item[i]);
						cur_b_y=cur_b_y+b3.get_y()*b3.get_net_weight(item[i]);	

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

	status=umfpack_di_triplet_to_col(n, n, nz, Ti, Tj, Tx, Ap, Ai, Ax, NULL);
	if (status != UMFPACK_OK) {
		return EXIT_FAILURE;
	}



	solve_equation(n, Ap, Ai, Ax, x, b_x, 0);		// indicates x (only for semantic rep);

	for(i=0; i< n; ++i) {
		ic.get_blck(idx_to_blck[i]).set_x(x[i]);
	}
	
	solve_equation(n, Ap, Ai, Ax, x, b_y, 1);		// indicates y '''''''''''''	

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

	float num_of_mv_blcks_per_quad=(float)(config.get_blck_to_nets().size()-config.get_ref_blcks().size())/4;

	LOG(INFO) << "Number of Moveable blocks per quad: "<<num_of_mv_blcks_per_quad;

	map<int, float> next_x_cuts;
	map<int, float> next_y_cuts;

	if(iter==1) {
		cur_x_cuts[0]=(float)ic.get_grid_size()/2;
		cur_y_cuts[0]=(float)ic.get_grid_size()/2;
	}

	double resize_inc=(float)ic.get_grid_size()/pow(2,iter+1);

	for(i=0; i<cur_x_cuts.size(); ++i) {

		map<int, vector<int> > class_to_blck; 				// define class holders;

		// define new pseudo blocks for expansion.
		Blck b_1;
		Blck b_2;
		Blck b_3;
		Blck b_4;

		last_blck_id=config.get_blck_to_nets()[config.get_blck_to_nets().size()-1][0];
		printf("Spreading: %3.2f%%\r", (float)100*i/cur_x_cuts.size());

		// fill in pseudos blocks;
		vector< vector<float> > new_refs;
		vector<float> refs;

		vector< vector<int> > new_blcks_to_net;
		vector<int> row;
		
		for(j=0; j<4; ++j){
			new_blcks_to_net.push_back(row);
			new_refs.push_back(refs);
			class_to_blck[j]=row;
		}

		b_1.set_x(cur_x_cuts[i]-resize_inc);
		b_1.set_y(cur_y_cuts[i]-resize_inc);
		b_1.set_pseudo();

		new_refs[0].push_back(last_blck_id+1);
		new_refs[0].push_back(b_1.get_x());
		new_refs[0].push_back(b_1.get_y());
		new_blcks_to_net[0].push_back(last_blck_id+1);

		b_2.set_x(cur_x_cuts[i]+resize_inc);
		b_2.set_y(cur_y_cuts[i]-resize_inc);
		b_2.set_pseudo();

		new_refs[1].push_back(last_blck_id+2);
		new_refs[1].push_back(b_2.get_x());
		new_refs[1].push_back(b_2.get_y());
		new_blcks_to_net[1].push_back(last_blck_id+2);

		b_3.set_x(cur_x_cuts[i]+resize_inc);
		b_3.set_y(cur_y_cuts[i]+resize_inc);
		b_3.set_pseudo();

		new_refs[2].push_back(last_blck_id+3);
		new_refs[2].push_back(b_3.get_x());
		new_refs[2].push_back(b_3.get_y());
		new_blcks_to_net[2].push_back(last_blck_id+3);

		b_4.set_x(cur_x_cuts[i]-resize_inc);
		b_4.set_y(cur_y_cuts[i]+resize_inc);
		b_4.set_pseudo();

		new_refs[3].push_back(last_blck_id+4);
		new_refs[3].push_back(b_4.get_x());
		new_refs[3].push_back(b_4.get_y());
		new_blcks_to_net[3].push_back(last_blck_id+4);

		// set blcks to classes.
		last_net_id=config.get_nbs_map().rbegin()->first + 1;
		default_random_engine generator; // used for rand number gens.
		if(i==0) {
			for(vector<int> row : config.get_blck_to_nets()) {

				int bid=row[0];
				Blck &blck_to_add=ic.get_blck(bid);
				blck_to_add.refresh();
			}			
		}

		for(vector<int> row : config.get_blck_to_nets()) {

			int bid=row[0];

			Blck &blck_to_add=ic.get_blck(bid);
			if(blck_to_add.is_stale()==0) {
	  			if(blck_to_add.is_fixed()==0 && (blck_to_add.belongs_to()==i || blck_to_add.belongs_to() == -2)) {
					
					discrete_distribution<int> distribution {wc1, wc2, wc3, wc4}; // adjust distribution on each roll.
					int potential_class=distribution(generator);

		  			switch(potential_class) {
		  				case 0:
		  					class_to_blck[0].push_back(bid);
		  					blck_to_add.set_to_group((i-i%4)+i*4);
		  					if(wc1==2) {
		  						wc1=1;
		  					}
		  					wc2=2;
		  					wc3=2;
		  					wc4=2;
		  					break;
		  				case 1:
		  					class_to_blck[1].push_back(bid);
		  					blck_to_add.set_to_group((i+1-i%4)+i*4);
		  					if(wc2==2) {
		  						wc2=1;
		  					}
		  					wc1=2;
		  					wc3=2;
		  					wc4=2;
		  					break;
		  				case 2:
		  					class_to_blck[2].push_back(bid);
		  					blck_to_add.set_to_group((i+2-i%4)+i*4);

		  					if(wc3==2) {
		  						wc3=1;
		  					}
		  					wc1=2;
		  					wc2=2;
		  					wc4=2; 
		  					break;
		  				case 3:
		  					class_to_blck[3].push_back(bid);
		  					blck_to_add.set_to_group((i+3-i%4)+i*4);

		  					if(wc4==2) {
		  						wc4=1;
		  					}
		  					wc1=2;
		  					wc2=2;
		  					wc3=2; 
		  					break;
		  			}

				} else if(blck_to_add.is_pseudo()==1) {
	 				blck_to_add.update_pseudo_blck_weight();
				}
			}
		}

  		for(j=0;j<4;++j) {
  			for(int bid : class_to_blck[j]) {
  				Blck &b = ic.get_blck(bid);
				
				uniform_real_distribution<double> x_dist(b.get_x(), 1);
				b.set_x(abs(x_dist(generator)));

				uniform_real_distribution<double> y_dist(b.get_y(), 1);
				b.set_y(abs(y_dist(generator)));

				config.update_blck_to_net(bid, last_net_id);

  				switch(j) {
  					case 0:
 						b.add_edge_weight(last_net_id, q1_w, 1);
						b_1.add_edge_weight(last_net_id, q1_w, 1);
						new_blcks_to_net[0].push_back(last_net_id);
  						break;

  					case 1:
  						b.add_edge_weight(last_net_id, q2_w, 1);
 						b_2.add_edge_weight(last_net_id, q2_w, 1);
						new_blcks_to_net[1].push_back(last_net_id);
  						break;
  					case 2:
  						b.add_edge_weight(last_net_id, q3_w, 1);
 						b_3.add_edge_weight(last_net_id, q3_w, 1);
						new_blcks_to_net[2].push_back(last_net_id);
  						break;
  					case 3:
  						b.add_edge_weight(last_net_id, q3_w, 1);
						b_4.add_edge_weight(last_net_id, q4_w, 1);
						new_blcks_to_net[3].push_back(last_net_id);
  						break;
  				}
  				++last_net_id;
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
				config.add_ref_blck(new_refs[j]);
			}
		}


		// define new
		next_x_cuts[0+i*4]=cur_x_cuts[i]-resize_inc;
		next_x_cuts[1+i*4]=cur_x_cuts[i]+resize_inc;
		next_x_cuts[2+i*4]=cur_x_cuts[i]-resize_inc; 
		next_x_cuts[3+i*4]=cur_x_cuts[i]+resize_inc;

		next_y_cuts[0+i*4]=cur_y_cuts[i]-resize_inc;
		next_y_cuts[1+i*4]=cur_y_cuts[i]-resize_inc;
		next_y_cuts[2+i*4]=cur_y_cuts[i]+resize_inc;
		next_y_cuts[3+i*4]=cur_y_cuts[i]+resize_inc;


		q1_w+=(2-2/iter)*abs(1-(class_to_blck[0].size())/num_of_mv_blcks_per_quad);
		q2_w+=(2-2/iter)*abs(1-(class_to_blck[1].size())/num_of_mv_blcks_per_quad);
		q3_w+=(2-2/iter)*abs(1-(class_to_blck[2].size())/num_of_mv_blcks_per_quad);
		q4_w+=(2-2/iter)*abs(1-(class_to_blck[3].size())/num_of_mv_blcks_per_quad);


	}
	printf("\n");
	// purge cuts;
	cur_y_cuts.clear();
	cur_x_cuts.clear();

	// instantiate new cuts;
	cur_x_cuts=next_x_cuts;
	cur_y_cuts=next_y_cuts;

	ic.update_nbs_map(config);

	return spread_status;
}

float Placer::get_hpwl() {
	return hpwl;
}


int Placer::is_grid_congested(IC ic, Configholder config) {
	int congestion_managable=0;
	int grid_size=config.get_grid_size();
	float p_overfill=0;
	int i,j;

	int overfill_count=0;

	map< pair<int, int> , vector<int> > grid_to_blcks;
	vector<int> blck_ids;

	for(i=0; i< grid_size; ++i) {
		for(j=0; j< grid_size; ++j) {
			grid_to_blcks[make_pair(i,j)]=blck_ids;
		}
	}

	for(vector<int> row : config.get_blck_to_nets()) {
		Blck b = ic.get_blck(row[0]);
		if(b.is_pseudo()==0) {
			grid_to_blcks[make_pair((int)floor(b.get_x()),(int)floor(b.get_y()))].push_back(row[0]);				
		}		
	}

	for(const auto& key : grid_to_blcks) {
		if(grid_to_blcks[make_pair(key.first.first, key.first.second)].size()>1) {
			overfill_count+=grid_to_blcks[make_pair(key.first.first, key.first.second)].size();
		}
	}
	p_overfill=100*((float)overfill_count/(grid_size*grid_size));
	LOG(INFO) << "Percentage of Slots filled: "<< p_overfill;

	if(p_overfill<=25) {
		congestion_managable=1;
	}


	return congestion_managable;
} 

int Placer::snap_to_grid(IC &ic, Configholder config) {

	int was_freed=0;

	int i, j;
	int grid_size=ic.get_grid_size();

	vector< vector<int> > blck_to_nets_t=config.get_blck_to_nets();

	map< pair<int, int> , vector<int> > grid_to_blcks;
	vector<int> blck_ids;

	for(i=0; i< grid_size; ++i) {
		for(j=0; j< grid_size; ++j) {
			grid_to_blcks[make_pair(i,j)]=blck_ids;

		}
	}

	LOG(INFO) << "Legalizing Blocks";
	for(vector<int> row : blck_to_nets_t) {
		Blck &b = ic.get_blck(row[0]);
		if(b.is_pseudo()==0) {
			if(b.get_x() >= grid_size) {
				LOG(ERROR) << "Placer panic. Exiting [x too large: " << b.get_x() <<"]";
				exit(-1);
			} 

			if(b.get_y() >= grid_size) {
				LOG(ERROR) << "Placer panic. Exiting [y too large: " << b.get_y() << "]";
				exit(-1);
			}

			b.set_x(floor(b.get_x())+0.5);
			b.set_y(floor(b.get_y())+0.5);
			
			if(grid_to_blcks.count(make_pair((int)floor(b.get_x()), (int)floor(b.get_y())))>0) {
				grid_to_blcks[make_pair((int)floor(b.get_x()), (int)floor(b.get_y()))].push_back(row[0]);
			} else {
				LOG(ERROR) << "Placer panic. Exiting [grid doesnt exist.]";	
				exit(-1);		
			}

		}
	}

	LOG(INFO) << "Snapping To Grid";

	vector<int> new_x_inspect={-1, -1, -1, 0, 0, 1, 1, 1};
	vector<int> new_y_inspect={-1, 0, 1, -1, 1, -1, 0, 1};

	blck_freed:

		for(const auto& key : grid_to_blcks) {
			if(grid_to_blcks[key.first].size()>1) {
				j=0;
				for(int bid : key.second) {

					if(ic.get_blck(bid).is_fixed() == 0) {
						unordered_map <pair<int, int> , int, pair_hash > where_to_look;	
						where_to_look[key.first]=0;


						while(was_freed==0) {
							for(auto& new_key : where_to_look) {
								int cx = new_key.first.first;
								int cy = new_key.first.second;
								pair<int, int> where_to_place;

								for(int i=0; i< new_x_inspect.size(); ++i) {
									if(grid_to_blcks.count(make_pair(cx + new_x_inspect[i], cy + new_y_inspect[i]))>0) {
			 							if(grid_to_blcks[make_pair( cx + new_x_inspect[i], cy + new_y_inspect[i])].size()==0) {
												Blck &b = ic.get_blck(bid);
				 								b.set_x(cx + new_x_inspect[i]+0.5);
				 								b.set_y(cy+new_y_inspect[i]+0.5);
					 								
					 							grid_to_blcks[make_pair( cx + new_x_inspect[i], cy + new_y_inspect[i])].push_back(grid_to_blcks[key.first][j]);
					 							grid_to_blcks[key.first].erase(grid_to_blcks[key.first].begin()+j);
				 								
				 								goto blck_freed;

			 							} else {
			 								if(where_to_look.count(make_pair( cx + new_x_inspect[i], cy + new_y_inspect[i])) <= 0) {
			 									where_to_look[make_pair( cx + new_x_inspect[i], cy + new_y_inspect[i])]=0;
			 								}
			 							}
									}
								}
							}
						}
					}
					++j;
				}
			} 
		}

	
	calculate_hpwl(ic, config);
	return 0;
}




