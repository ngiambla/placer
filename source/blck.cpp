
#include "blck.h"

//constructor
Blck::Blck(int id) {
	this->id=id;
	isFixed=0;
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


// allow r/w access to weights.
float Blck::get_net_weight(int netNum) {
	
}

void Blck::add_edge_weight(int netNum, float weight, int howmany) {
	int i;
	vector<float> weights;
	for(i=0; i<howmany-1; ++i) {
		weights.push_back(weight);
	}
	net_w_expansion[netNum]=weights;
}

float Blck::get_total_weight() {

}

void Blck::display_blck() {
	printf("#####################\nid -- [%d]\n", id);
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