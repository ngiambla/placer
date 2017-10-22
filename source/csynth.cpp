//#include "pdefs.h"
#include <fstream>
#include "graphics.h"
#include "ic.h"
#include "placer.h"

// Usage Message
const char * usage="Usage ./placer -file [filename]\n";

// Reference to config
Configholder config;


// function references for graphics.
void drawscreen(void);
void act_on_button_press (float x, float y);
void act_on_mouse_move (float x, float y);
void act_on_key_press (char c);


void initialize_system(char * filename) {

	FILE *fp;

	vector<int> blck_net;
	vector<float> secure_block;
	
	int decode_stage=0;
	int switch_to_secure_state=0;

	char line[128];

	fp=fopen(filename, "r");
	LOG(INFO) << "Reading file "<< filename << "\n";


	if(fp) {
		while(fgets(line, 128, fp) != NULL) {

			decode_stage=0;

			char * token = strtok(line, " ");
			
			while(token != NULL) {
				if(switch_to_secure_state==0) {
					if(decode_stage==0){
						blck_net.clear();
						if(atoi(token) == -1) {
							switch_to_secure_state=1;
							decode_stage=-1;
							break;
						}
					}
					if(atoi(token) != -1){
						blck_net.push_back(atoi(token));
						++decode_stage;
					} else {
						config.add_blck_to_net(blck_net);						
					}

				} else {
					if(decode_stage==0) {
						if(atoi(token) == -1) {
							break;
						} else {

						}
					}
					secure_block.push_back(atof(token));
					++decode_stage;
					if(decode_stage==3) {
						config.add_ref_blck(secure_block);
						secure_block.clear();
						decode_stage=0;
					}
				}
				token = strtok(NULL, " ");
			}

		}
		fclose(fp);
		LOG(DEBUG) << "Grid Size [" << config.get_grid_size() << "][" << config.get_grid_size() << "]";

	} else {
		LOG(ERROR) << "File name not found.\n";
		exit(-1);
	}

}

// main call.
int main(int argc, char * argv[]) {
	char filename[20]="../circuits/";

	if(argc == 3) {
		strncat(filename, argv[2], 4);		
		initialize_system(filename);
		Placer placer;
		IC ic(config);
		// if(placer.place()==1) {
		// 	LOG(INFO) << "Placement Complete.";
		// } else {
		// 	LOG(ERROR) << "Placement was unsucessful.";
		// }

	} else {
		printf("%s", usage);
		return ERR;
	}

	return SUCCESS;
}