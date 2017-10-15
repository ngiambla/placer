#include "configholder.h"

/* sets configholder's state to represent 
the block to nets (for connections) */
void Configholder::add_blck_to_net(vector<int> blck_to_net) {
	blck_to_nets.push_back(blck_to_net);
}


void Configholder::add_ref_blck(vector<int> ref_blck) {
	ref_blcks.push_back(ref_blck);
}


vector< vector<int> > Configholder::get_blck_to_nets() {
	return ref_blcks;
}

vector< vector<int> > Configholder::get_ref_blcks() {
	return blck_to_nets;
}

int Configholder::get_grid_size() {
	return (int)ceil(sqrt(ref_blcks.size()+blck_to_nets.size()));
}

void Configholder::display_config() {
	int i;
	printf("[..-* Config *-..]\n\n");
	printf("<blck_to_nets>\n");
	for(vector<int> btn : blck_to_nets) {
		i=0;
		for(int item : btn) {
			if(i==0) {
				printf("   <blck>%d</blck>\n", item);
			} else {
				printf("      <net>%d</net>\n", item);
			}
			++i;
		}
	}
	printf("</blck_to_nets>\n<static_blck>\n");
	for(vector<int> btn : ref_blcks) {
		printf("   <blck>%d</blck>\n      <x_cor>%d</x_cor>\n      <y_cor>%d</y_cor>\n", btn[0], btn[1], btn[2]);
	}
	printf("</static_blck>\n");
	printf("**[eoc]**\n\n");
}