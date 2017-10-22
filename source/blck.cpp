
#include "blck.h"

// typical getters and setters
double Blck::get_x() {
	return x;
}

double Blck::get_y() {
	return y;
}


void Blck::set_x(double x) {
	this->x=x;
}

void Blck::set_y(double y) {
	this->y=y;
} 


// allow r/w access to weights.
double Blck::get_weight(int netNum) {
	return net_weights[netNum];
}

void Blck::set_weight(int netNum, double weight) {
	net_weights[netNum] = weight;
}


