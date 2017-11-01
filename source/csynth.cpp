#include <fstream>
#include <iostream>
#include "graphics.h"
#include "ic.h"
#include "placer.h"

// Usage Message
const char * usage="Usage ./placer -file [filename]\n";

Configholder config; 			// Reference to config
IC ic; 							// Reference to Integrated Circuit
Placer placer;					// Reference to placer;

bool show_clique=true;
int iters=1;
int finished=0;
char msg[255]="";


// function references for graphics.
void drawscreen(void);

void spread_and_place (void (*drawscreen_ptr) (void));
void toggle_clique_model (void (*drawscreen_ptr) (void));

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
	LOG(INFO) << "Reading file "<< filename;


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
							decode_stage=0;
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

					if(atoi(token)==-1) {
						break;
					}

					switch(decode_stage) {
						case 0:
							secure_block.clear();
							secure_block.push_back(atof(token));
							++decode_stage;
							break;
						case 1:
							secure_block.push_back(atof(token));
							++decode_stage;
							break;
						case 2:
							secure_block.push_back(atof(token));
							config.add_ref_blck(secure_block);
							decode_stage=0;
							break;
					}
				}
				token = strtok(NULL, " ");
			}

		}
		fclose(fp);
		LOG(INFO) << "-- config read-in complete.";
		LOG(INFO) << "Grid Size [" << config.get_grid_size() << "]X[" << config.get_grid_size() << "]";

	} else {
		LOG(ERROR) << "File name not found.\n";
		exit(-1);
	}

}

// main call.
int main(int argc, char * argv[]) {
	char filename[20]="../circuits/";
	//int was_placed=0;
	//int iters=1;

	if(argc == 3) {
		strncat(filename, argv[2], 4);		
		initialize_system(filename);

		IC ic_t(config);
		ic=ic_t;

      	placer.place(ic, config);
		for(vector<int> row : config.get_blck_to_nets()) {
			ic.get_blck(row[0]).display_pos(row[0]);
		}
		LOG(INFO) << " <placer> HPWL Measurement: "<< placer.get_hpwl() << "\n";

		init_graphics("..-* Probablisitic Placer *-..", WHITE);
		sprintf(msg, "<placer> HPWL Measurement: %f", placer.get_hpwl());
		update_message(msg);

		init_world(0,0, ic.get_grid_size(), ic.get_grid_size());

		clearscreen();

   		create_button ("Window", "Nets [1]", toggle_clique_model);
      	create_button ("Window", "Step >>", spread_and_place);

		update_message(msg);

		drawscreen();
      	event_loop(act_on_button_press, NULL, NULL, drawscreen); 
		

		// while(1) {
		// 	was_placed=placer.place(ic, config);
		// 	for(vector<int> row : config.get_blck_to_nets()) {
		// 		ic.get_blck(row[0]).display_pos(row[0]);
		// 	}
		// 	LOG(INFO) << " <placer> HPWL Measurement: "<< placer.get_hpwl() << "\n";

		// 	if(iters==10 || placer.is_grid_congested(ic, config)==1) {
		// 		placer.snap_to_grid(ic, config);			
		// 		for(vector<int> row : config.get_blck_to_nets()) {
		// 			ic.get_blck(row[0]).display_pos(row[0]);
		// 		}
		// 		LOG(INFO) << " <placer> HPWL Measurement: "<< placer.get_hpwl() << "\n";
		// 		break;
		// 	} else {
		// 		if(was_placed==1) {
		// 			LOG(INFO) << "..-* Spreading iter["<<iters<<"] *-..";

		// 			placer.spread(ic, config, iters);
		// 			LOG(INFO) << "..-* Spreading Complete. *-..";
		// 		}
		// 	}			


		// 	cin.ignore();
		// 	++iters;
		// }
		

	} else {
		printf("%s", usage);
		return ERR;
	}

	return SUCCESS;
}

void drawscreen(void) {
	int i, j;
	int max_size=ic.get_grid_size();
	set_draw_mode (DRAW_NORMAL);
	clearscreen();

	//draw grid;
	setcolor(BLACK);
	setlinewidth (4);
	setlinestyle (SOLID);
	//draw vertical lines;
	for(i=0; i< ic.get_grid_size()+1; i+=1) {
		drawline(0, i, max_size, i);
	}

	//draw horizontal lines;
	for(i=0; i<ic.get_grid_size()+1; i+=1) {
		drawline(i,0,i, max_size);
	}

	for(vector<int> row : config.get_blck_to_nets()) {
		Blck b=ic.get_blck(row[0]);
		if(b.is_pseudo()==0) {
			if(b.is_fixed()==1) {
				// draw black;
				setcolor(DARKGREY);
				fillarc(b.get_x(), b.get_y(), 0.25, 0.,360.);
				setcolor(MAGENTA);
				drawarc(b.get_x(), b.get_y(), 0.25, 0.,360.);

			} else {
				setcolor(GREEN);
				fillarc(b.get_x(), b.get_y(), 0.25, 0.,360.);
				setcolor(BLUE);
				drawarc(b.get_x(), b.get_y(), 0.25, 0.,360.);
				//draw grey;
			}
		}
	}
}

void spread_and_place (void (*drawscreen_ptr) (void)) {

	char old_button_name[200], new_button_name[200];
	if(finished==0) {

		if(iters==10 || placer.is_grid_congested(ic,config)==1) {
			placer.snap_to_grid(ic, config);
			sprintf (old_button_name, "Step <%d>",iters);
			sprintf (new_button_name, "Done[%d]",1);
			finished=1;
		} else {
			placer.spread(ic, config, iters);
			placer.place(ic, config);
			if(iters==1){
				sprintf (old_button_name, "Ste%c >>",'p');
			} else {
				sprintf (old_button_name, "Step <%d>",iters);				
			}
			++iters;
			sprintf (new_button_name, "Step <%d>",iters);
		}

		LOG(INFO) << " <placer> HPWL Measurement: "<< placer.get_hpwl() << "\n";
      	sprintf(msg, "<placer> HPWL Measurement: %f", placer.get_hpwl());
		update_message(msg);
		change_button_text (old_button_name, new_button_name);
		drawscreen();
	} else {
		close_graphics();
		exit(0);
	}
}

void toggle_clique_model(void (*drawscreen_ptr) (void)) {
	char old_button_name[200], new_button_name[200];
	sprintf (old_button_name, "Nets [%d]",show_clique);
	show_clique=!show_clique;
	sprintf (new_button_name, "Nets [%d]",show_clique);
	change_button_text (old_button_name, new_button_name);
	drawscreen();
}

void act_on_button_press (float x, float y) {


}