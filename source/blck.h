#ifndef __BLCK_H__
#define __BLCK_H__

#include "pdefs.h"


class Blck {
	private:
		map<int, double> net_weights;
		vector<int> config;

		double x;
		double y;

	public:
		Blck(vector<int> config_t);

		double get_x();
		double get_y();

		void set_x(double x);
		void set_y(double y);

		double get_weight(int netNum);
		void set_weight(int netNum, double weight);

};

#endif