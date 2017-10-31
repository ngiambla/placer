
#include "blck.h"

//constructor
Blck::Blck() {
	isPseudo=0;
	isFixed=0;
	
	classNum=-2;
	isStale=0;

	x=UNKNOWN;
	y=UNKNOWN;
}


// typical getters and setters
float Blck::get_x() {
	return x;
}

float Blck::get_y() {
	return y;
}

void Blck::set_fixed() {
	isFixed=1;
}

int Blck::is_fixed() {
	return isFixed;
}

int Blck::is_stale() {
	return isStale;
}

void Blck::set_pseudo() {
	isPseudo=1;
	isFixed=1;
}

int Blck::is_pseudo() {
	return isPseudo;
}

void Blck::set_x(float x) {
	if(isFixed == 0) {
		this->x=x;
	}
}

void Blck::set_y(float y) {
	if(isFixed == 0) {
		this->y=y;
	}
} 

int Blck::belongs_to() {
	return classNum;
}

void Blck::set_to_group(int group_id) {
	if(isStale==0) {
		classNum=group_id;
		isStale=1;
	}
}

void Blck::refresh() {
	isStale=0;
}

// allow r/w access to weights.
float Blck::get_net_weight(int netNum) {
	if(net_w_expansion.count(netNum)>0) {
		return net_w_expansion[netNum][0];
	} else {
		return 0;
	}
}

void Blck::update_pseudo_blck_weight() {
	int i;

	if(isStale==0) {
		for(const auto& key : net_w_expansion) {
			for(i=0; i<net_w_expansion[key.first].size(); ++i) {

				if(net_history.count(key.first)> 0 ) {
					net_history[key.first]+=1;
				} else {
					net_history[key.first]=0;
				}
				if(net_history[key.first] >= 4) {
					//net_w_expansion[key.first][i]*=0.99;
				}
			}
		}
	}
	set_to_group(-3);
}

void Blck::add_edge_weight(int netNum, float weight, int howmany) {
	int i;
	vector<float> weights;
	for(i=0; i<howmany; ++i) {
		weights.push_back(weight);
	}
	net_w_expansion[netNum]=weights;
}

float Blck::get_total_weight() {
	float sum=0;
	for(const auto& key : net_w_expansion) {
		for(float weight : key.second) {
			sum=sum+weight;
		}	
	}	
	return sum;
}

void Blck::display_blck() {
	printf("#####################\n");
	printf("isFixed -- [%d]\n", isFixed);
	printf("pos [%f][%f]\n",x,y);

	for(const auto& key : net_w_expansion) {
		printf("net-[%d]\n  w_edges", key.first);
		for(float weight : key.second) {
			printf("[%f]", weight);
		}
		printf("\n");
	}
	printf("#####################\n\n");
}

void Blck::display_pos(int id) {
	if(isPseudo==0) {
		if(isFixed==1) {
			printf("blck [%d] @x[%f]y[%f] class[UNASSIGN]\n", id, x, y);
		} else {
			printf("blck [%d] @x[%f]y[%f] class[%d]\n", id, x, y, classNum);
		}
	}
}