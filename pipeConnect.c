/* This files provides address values that exist in the system */

#define SDRAM_BASE            0xC0000000
#define FPGA_ONCHIP_BASE      0xC8000000
#define FPGA_CHAR_BASE        0xC9000000

/* Cyclone V FPGA devices */
#define LEDR_BASE             0xFF200000
#define HEX3_HEX0_BASE        0xFF200020
#define HEX5_HEX4_BASE        0xFF200030
#define SW_BASE               0xFF200040
#define KEY_BASE              0xFF200050
#define TIMER_BASE            0xFFFEC600
#define PIXEL_BUF_CTRL_BASE   0xFF203020
#define CHAR_BUF_CTRL_BASE    0xFF203030

/* VGA colors */
#define WHITE 0xFFFF
#define BLACK 0x0000
#define YELLOW 0xFFE0
#define RED 0xF800
#define GREEN 0x07E0
#define BLUE 0x001F
#define CYAN 0x07FF
#define MAGENTA 0xF81F
#define GREY 0xC618
#define PINK 0xFC18
#define ORANGE 0xFC00
	
#define ABS(x) (((x) > 0) ? (x) : -(x))
#define ROL(x, num) ((x << num) | (x >> (4 - num))
#define ROR(x, num) ROL(x, 4-num)

/* Screen size. */
#define RESOLUTION_X 320
#define RESOLUTION_Y 240

/* Constants for animation */
#define BOX_LEN 30
#define NUM_BOXES_X 10
#define NUM_BOXES_Y 5
#define FIRST_X 5 
#define FIRST_Y 42
#define LAST_X 316
#define LAST_Y 198

#define MOVE 0b000001 
#define TURN 0b000010
#define RUN 0b000100
#define RESET 0b001000
	
#define UP 0b000001 
#define DOWN 0b000010
#define LEFT 0b000100
#define RIGHT 0b001000
#define CCW 0b010000
#define CW 0b100000	
					 

#define FALSE 0
#define TRUE 1

#include <stdlib.h>
#include <stdio.h>

volatile int pixel_buffer_start;
int USER_X, USER_Y, BOX_X, BOX_Y;
int pre_USER_X, pre_USER_Y;
int run_water;

int level3[5][10] = { {3, 2, 3, 1, 4, 1, 2, 2, 3, 6}, 
					  {1, 3, 1, 6, 0, 6, 3, 1, 3, 6},
					  {6, 3, 2, 5, 1, 1, 5, 2, 5, 5},
					  {2, 0, 1, 1, 3, 1, 3, 3, 6, 3},
					  {1, 5, 1, 4, 4, 1, 2, 2, 6, 1}
					};
int level3_pass[5][10] = { {3, 2, 3, 1, 4, 1, 2, 2, 5, 3}, 
					 {1, 5, 1, 6, 0, 5, 5, 1, 3, 3},
					 {5, 3, 1, 4, 1, 1, 3, 2, 5, 5},
					 {2, 0, 1, 1, 3, 1, 3, 6, 6, 3},
					 {2, 5, 1, 4, 4, 1, 2, 2, 6, 1}
					};
int temple[5][10] = { {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
					  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
					  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
					  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
					  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
					};
int available[5][10] = { {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
					  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
					  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
					  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
					  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
					};
int connect_pipe[5][10] = { {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
					  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
					  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
					  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
					  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
					};

int water_pipe[5][10] = { {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
					  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
					  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
					  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
					  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
					};

int path[50][2];
int path_num;
int water[2][2] = {{298, 6}, {301, 6}};	
int pre_water[2][2] = {{298, 0}, {301, 0}};	
int water_x1, water_y1, water_x2, water_y2;
int water_in_dir;
					 
void clear_screen();
void plot_pixel(int x, int y, short int line_color);
void wait_for_vsync();
void draw_grid();
void draw_user_box(int x, int y);
void clean_user_box(int x, int y);
void draw_straight_horizontal_pipe(int x, int y, int colour);
void draw_straight_vertical_pipe(int x, int y, int colour);
void draw_left_up_pipe(int x, int y, int colour);
void draw_right_up_pipe(int x, int y, int colour);
void draw_right_down_pipe(int x, int y, int colour);
void draw_left_down_pipe(int x, int y, int colour);
void user_move(int direction);
void user_turn(int direction);
void draw_all_pipes();
void load_available();
void check_available();
void draw_water();
void draw_strt_end_water();
void water_straight_horizontal_pipe(int x, int y, int dir);
void water_straight_vertical_pipe(int x, int y, int dir);
void water_left_up_pipe(int x, int y, int dir);
void water_right_up_pipe(int x, int y, int dir);
void water_right_down_pipe(int x, int y, int dir);
void water_left_down_pipe(int x, int y, int dir);
void print_fail(int colour);
void print_pass(int colour);
void initial_set();

					 
int main(void)
{
    volatile int * pixel_ctrl_ptr = (int *)PIXEL_BUF_CTRL_BASE;
	volatile int * key_ptr = (int *)KEY_BASE;
	volatile int * sw_ptr = (int *)SW_BASE;
	volatile int * timer_ptr = (int *)TIMER_BASE;
	
	*(timer_ptr) = 50000000;
	*(timer_ptr + 2) = 0b011;
    // declare other variables(not shown)
    // initialize location and direction of rectangles(not shown)
	
	
	//darw column 
   
	
	
	
	/* set front pixel buffer to start of FPGA On-chip memory */
    *(pixel_ctrl_ptr + 1) = 0xC8000000; // first store the address in the 
                                        // back buffer
    /* now, swap the front/back buffers, to set the front buffer location */
    wait_for_vsync();
    /* initialize a pointer to the pixel buffer, used by drawing functions */
    pixel_buffer_start = *pixel_ctrl_ptr;
    clear_screen(); // pixel_buffer_start points to the pixel buffer
    /* set back pixel buffer to start of SDRAM memory */
    *(pixel_ctrl_ptr + 1) = 0xC0000000;
    pixel_buffer_start = *(pixel_ctrl_ptr + 1); // we draw on the back buffer
    initial_set();
	
	
    while (1)
    {
        int direction = *(sw_ptr);
		int change  = *(key_ptr + 3);
		if(!run_water){
			print_fail(BLACK);
			print_pass(BLACK);
		}
		draw_grid();
		draw_strt_end_water();
		
		
		int timer_edge = *(timer_ptr + 3);
		if(timer_edge != 0b1){
			draw_user_box(USER_X, USER_Y);
		}else{
			
			*(timer_ptr + 3) = timer_edge;
		}
		
		
		check_available();
		draw_all_pipes();
		
		
		if(change != FALSE){
			if(change == MOVE){
				
				user_move(direction&0xf);
				
			}else if(change == TURN && run_water == 0){
				user_turn(direction&0x30);//
			}
			
			if(change == RUN){
				if(run_water == 0){
					run_water = 1;
				}else{
					
					run_water = 0;
					path_num = -1;
					water[0][0] = 298;
					water[0][1] = 6;
					water[1][1] = 6;
					water[1][0] = 301;
					pre_water[0][1] = 0;
					pre_water[1][1] = 0;
					pre_water[0][0] = water[0][0];
					pre_water[1][0] = water[1][0];
					int i, j;
					for(i = 0; i < 5; i++){
						for(j = 0; j < 10; j++){
							water_pipe[i][j] = 0;
						}
					}
				}
				
			}
			if(change == RESET){
				for(int i = 0; i < 5; i++){
					for(int j = 0; j < 10; j++){
						temple[i][j]=level3[i][j];
					}
				}
				initial_set();
			}
			*(key_ptr + 3) = change;
		}
		
		if(run_water){
			draw_water();
		}
		
		
        wait_for_vsync(); // swap front and back buffers on VGA vertical sync
        pixel_buffer_start = *(pixel_ctrl_ptr + 1); // new back buffer
    }

    
}

// code for subroutines (not shown)
void plot_pixel(int x, int y, short int line_color)
{
    *(short int *)(pixel_buffer_start + (y << 10) + (x << 1)) = line_color;
}

void clear_screen(){
	for(int y = 0; y < RESOLUTION_Y; y++){
		for(int x = 0; x < RESOLUTION_X; x++){
			plot_pixel(x, y, BLACK);
		}
	}
}


void wait_for_vsync(){
	volatile int * pixel_ctrl_ptr = (int *)0xFF203020;
	register int status;
	
	*pixel_ctrl_ptr = 1;
	
	status = *(pixel_ctrl_ptr +3);
	while((status & 0x01) != 0){
		status = *(pixel_ctrl_ptr +3);
	}
}

void initial_set(){
	clear_screen(); // pixel_buffer_start points to the pixel buffer
	USER_X = 9;
	USER_Y = 0;
	BOX_X = 9;
	BOX_Y = 0;
	run_water = 0;
	path_num = -1;
	for(int i = 0; i < 5; i++){
		for(int j = 0; j < 10; j++){
			if(level3[i][j]==2 || level3[i][j]==1){
				level3[i][j]=rand()%2+1;
			}else if(level3[i][j]==0){
				level3[i][j]=rand()%7;
			}else{
				level3[i][j]=rand()%4+3;
			}
		}
	}
	load_available();
}
					 
void load_available(){
	int i, j;
	for(i = 0; i < NUM_BOXES_Y; i++){
		for(j = 0; j < NUM_BOXES_X; j++){
			switch(level3[i][j]){
				case 1:
					available[i][j] = 0b0101;
					break;
				case 2:
					available[i][j] = 0b1010;
					break;
				case 3:
					available[i][j] = 0b1001;
					break;
				case 4:
					available[i][j] = 0b1100;
					break;
				case 5:
					available[i][j] = 0b0110;
					break;
				case 6:
					available[i][j] = 0b0011;
					break;
				default:
					break;
			}
		}
	}
}
					 
void draw_water(){
	int i;
	int j;
	
	if(path_num == -1){
		for(i = water[0][0]; i <= water[1][0]; i++){
			for(j = pre_water[0][1]; j <= water[0][1]; j++){
				plot_pixel(i, j, CYAN);
			}
		}
		if(water[0][1] <= FIRST_Y - 1){
			water[0][1] = water[0][1] + 6;
			water[1][1] = water[1][1] + 6;
			if(water[0][1] > FIRST_Y - 1){
				water[0][1] = FIRST_Y + 1;
				water[1][1] = FIRST_Y + 1;
				path_num++;
				pre_water[0][1] = water[0][1];
				pre_water[1][1] = water[0][1];
				water_in_dir = 1;
			}
		}		
	}else if(path[path_num][0] != -1 && path[path_num][1] != -1){
		int path_x = path[path_num][0];
		int path_y = path[path_num][1];
		//int type = level3[path_y][path_x];
		switch(level3[path_y][path_x]){
			case 1:
				water_straight_horizontal_pipe(path_x, path_y, water_in_dir);
				break;
			case 2:
				water_straight_vertical_pipe(path_x, path_y, water_in_dir);
				break;
			case 3:
				water_left_up_pipe(path_x, path_y, water_in_dir);
				break;
			case 4:
				water_right_up_pipe(path_x, path_y, water_in_dir);
				break;
			case 5:
				water_right_down_pipe(path_x, path_y, water_in_dir);
				break;
			case 6:
				water_left_down_pipe(path_x, path_y, water_in_dir);
				break;
			default:
				break;
		}
	}else if(path[path_num - 1][0] == 0 && path[path_num - 1][1] == 4 && water_in_dir == 1){
		
		for(i = water[0][0]; i <= water[1][0]; i++){
			for(j = pre_water[0][1]; j <= water[0][1]; j++){
				plot_pixel(i, j, CYAN);
			}
		}
		if(water[0][1] < LAST_Y + 40){
			water[0][1] = water[0][1] + 6;
			water[1][1] = water[1][1] + 6;
			if(water[0][1] > LAST_Y + 40){
				water[0][1] = LAST_Y + 40;
				water[1][1] = LAST_Y + 40;
			}
		}else{
			if(water[0][1] == LAST_Y + 40 && pre_water[0][0] == FIRST_X + 14){
				water[0][0] = FIRST_X + 1;
				water[0][1] = LAST_Y + 40;
				water[1][0] = FIRST_X + 30;
				water[1][1] = LAST_Y + 40;
				pre_water[0][0] = water[0][0];
				pre_water[0][1] = water[0][1];
				pre_water[1][0] = water[1][0];
				pre_water[1][1] = water[1][1];
				
			}
			if(pre_water[0][1] > LAST_Y + 11){
				pre_water[0][1] = pre_water[0][1] - 1;
				pre_water[1][1] = pre_water[1][1] - 1;
			}else if(pre_water[0][1] == LAST_Y + 11){
				print_pass(GREEN);
			}
			
			for(i = FIRST_X + 14; i <= FIRST_X + 17; i++){
				for(j = LAST_Y; j <= LAST_Y + 40; j++){
					plot_pixel(i, j, CYAN);
				}
			}
			
		}
	}else{
		print_fail(RED);
	}
	
}
					 

void check_available(){
	int connect = TRUE;
	int x = 9;
	int y = 0;
	int dir = 0b1000;
	
	int i, j;
	for(i = 0; i < 5; i++){
		for(j = 0; j < 10; j++){
			connect_pipe[i][j] = 0;
		}
	}
	
	for(i = 0; i < 50; i++){
		for(j = 0; j < 2; j++){
			path[i][j] = -1;
		}
	}
	
	int num = 0;
	while(connect){
		
		if((available[y][x] & dir) == dir){
			connect_pipe[y][x] = 1;
			
			
			path[num][0] = x;
			path[num][1] = y;
			num++;
			int new_dir = available[y][x] ^ dir;
			switch(new_dir){
				case 0b0001:
					x--;
					dir = 0b0100;
					break;
				case 0b0010:
					y++;
					dir = 0b1000;
					break;
				case 0b0100:
					x++;
					dir = 0b0001;
					break;
				case 0b1000:
					y--;
					dir = 0b0010;
					break;
				default:
					break;	
			}
		}else{
			connect = FALSE;
			
		}
		if(x < 0 || x > 9 || y < 0 || y > 9){
			connect = FALSE;
		}
	}
}
					 
void draw_grid(){
	int i, j;
	for(i = FIRST_X; i < LAST_X; i = i + BOX_LEN + 1){
		for(j = FIRST_Y; j < LAST_Y; j++){
			plot_pixel(i, j, WHITE);
		}
	}

	for(i = FIRST_X; i < LAST_X; i++){
		for(j = FIRST_Y; j < LAST_Y; j = j + BOX_LEN + 1){
			plot_pixel(i, j, WHITE);
		}
	}
	
	for(j = 10; j < 25; j++){
		plot_pixel(FIRST_X + 285, j, CYAN);
	}
	
	
	for(j = 25; j < 28; j++){
		int i_start, i_end;
		if(j == 25){
			i_start = FIRST_X + 283;
			i_end = FIRST_X + 288;
		}else{
			i_start++;
			i_end--;
		}
		
		for(i = i_start; i < i_end; i++){
			plot_pixel(i, j, CYAN);
		}
		
	}

	
	int x0 = FIRST_X + 9 * 30 + (9 + 1) * 1;

	for(i = 0; i < FIRST_Y; i++){
		if(i == 0 || i == FIRST_Y - 1){
			plot_pixel(x0 + 11, i, RED);
			plot_pixel(x0 + 18, i, RED);
		}
		plot_pixel(x0 + 12, i, RED);
		plot_pixel(x0 + 17, i, RED);
	}
	
	for(j = LAST_Y + 10; j < LAST_Y + 25; j++){
		plot_pixel(FIRST_X + 36, j, CYAN);
	}
	
	
	for(j = LAST_Y + 25; j < LAST_Y + 28; j++){
		int i_start, i_end;
		if(j == LAST_Y + 25){
			i_start = FIRST_X + 34;
			i_end = FIRST_X + 39;
		}else{
			i_start++;
			i_end--;
		}
		
		for(i = i_start; i < i_end; i++){
			plot_pixel(i, j, CYAN);
		}
		
	}
	
	x0 = FIRST_X + 1;
	
	for(j = LAST_Y; j < LAST_Y + 42; j++){
		if(j == LAST_Y){
			plot_pixel(x0 + 11, j, RED);
			plot_pixel(x0 + 18, j, RED);
		}
		if(j< LAST_Y + 10){
			plot_pixel(x0 + 12, j, RED);
			plot_pixel(x0 + 17, j, RED);
		}else if(j == LAST_Y + 10 || j == LAST_Y + 41){
			for(i = FIRST_X; i < FIRST_X + 32; i++){
				if(j == LAST_Y + 10 && i > x0 + 12 && i <x0 + 17){
					continue;
				}else{
					plot_pixel(i, j, RED);
				}
			}
		}else{
			plot_pixel(FIRST_X, j, RED);
			plot_pixel(FIRST_X + 31, j, RED);
		}
		
	}

}

void print_fail(int colour){
	int x0 = 172;
	int y0 = LAST_Y + 10;
	int i, j;
	for(i = x0; i < x0+105; i++){
		for(j = y0; j < y0+25; j++){
			
			if((i >= x0 && i < x0+5)||(i >= x0+50 && i < x0+55)||(i>=x0+60 && i< x0+65)){
				plot_pixel(i, j, colour);
			}
			if((i >=x0+25 && i <x0+30)||(i >=x0+40 && i <x0+45)){
				if(j>=y0+5 && j<y0+25){
					plot_pixel(i, j, colour);
				}
			}
			if((j < y0+5) || (j>=y0+10 && j<y0+15)){
				if(i < x0+20 || (i >= x0+30 && i < x0+40)){
					plot_pixel(i, j, colour);
				}
			}
			
			if(j>=y0+20 && j<y0+25){
				if(i>=x0+60 && i<x0+75){
					plot_pixel(i, j, colour);
				}
				
				if((i>=x0+80 && i<x0+85)||(i>=x0+90 && i<x0+95)||(i >=x0+100 && i<x0+105)){
					plot_pixel(i, j, colour);
				}
			}
			
			
		}
	}
}
					 
void print_pass(int colour){
	int x0 = 172;
	int y0 = LAST_Y + 10;
	int i, j;
	for(i = x0; i < x0+115; i++){
		for(j = y0; j < y0+25; j++){
			
			if((i >= x0 && i < x0+5)){
				plot_pixel(i, j, colour);
			}
			if((i >=x0+25 && i <x0+30)||(i >=x0+40 && i <x0+45)){
				if(j>=y0+5 && j<y0+25){
					plot_pixel(i, j, colour);
				}
			}
			if((i >=x0+50 && i <x0+55)||(i >=x0+70 && i <x0+75)){
				if(j>=y0 && j<y0+15){
					plot_pixel(i, j, colour);
				}
			}
			if((i >=x0+60 && i <x0+65)||(i >=x0+80 && i <x0+85)){
				if(j>=y0+10 && j<y0+25){
					plot_pixel(i, j, colour);
				}
			}
			
			
			if((j < y0+5) || (j>=y0+10 && j<y0+15)){
				if((i < x0+15) || (i >= x0+30 && i < x0+40)){
					plot_pixel(i, j, colour);
				}
			}
			
			if((j< y0+5)||(j>=y0+10&&j<y0+15)||(j>=y0+20&&j<y0+25)){
				if((i>=x0+50&&i<x0+65)||(i>=x0+70&&i<x0+85)){
					plot_pixel(i, j, colour);
				}
			}
			
			if(j>=y0+5&&j<y0+10){
				if(i>=x0+15&&i<x0+20){
					plot_pixel(i, j, colour);
				}
			}
			
			if(j>=y0+20 && j<y0+25){
				
				if((i>=x0+110 && i<x0+115)||(i>=x0+90 && i<x0+95)||(i >=x0+100 && i<x0+105)){
					plot_pixel(i, j, colour);
				}
			}
			
			if(j>=y0 && j<y0+15){
				
				if((i>=x0+110 && i<x0+115)||(i>=x0+90 && i<x0+95)||(i >=x0+100 && i<x0+105)){
					plot_pixel(i, j, colour);
				}
			}
			
		}
	}
}
					 
void draw_strt_end_water(){
	int x0 = FIRST_X + 9 * 30 + (9 + 1) * 1;
	
	int i, j;
	for(i = 0; i < FIRST_Y; i++){
		if(run_water == 1 && path_num > -1){
			for(j = x0 + 13; j < x0 + 17; j++){
				plot_pixel(j, i, CYAN);
			}
		}
		if(run_water == 0){
			for(j = x0 + 13; j < x0 + 17; j++){
				plot_pixel(j, i, BLACK);
			}
		}
	}
	
	x0 = FIRST_X + 1;
	
	for(j = LAST_Y; j < LAST_Y + 41; j++){
		if(j<= LAST_Y + 10){
			for(i = x0 + 13; i < x0 + 17; i++){
				if(run_water == 0){
					plot_pixel(i, j, BLACK);
				}
			}
		}else{
			for(i = x0; i < x0 + 30; i++){
				if(run_water == 0){
					plot_pixel(i, j, BLACK);
				}
			}
		}
		
	}
}
				 
void draw_user_box(int x, int y){
	int x0 = FIRST_X + x * 30 + x;
	int y0 = FIRST_Y + y * 30 + y;
	
	int i;
	int j;
	for(i = x0; i < x0 + 32; i++){
		plot_pixel(i, y0, RED);
		plot_pixel(i, y0 + 31, RED);
	}

	for(j = y0 + 1; j < y0 + 31; j++){
		plot_pixel(x0, j, RED);
		plot_pixel(x0 + 31, j, RED);
	}
	
}

void draw_all_pipes(){
	for(int i = 0; i < 10; i++){
		for(int j = 0; j < 5; j++){
			switch(temple[j][i]){
				case 1:
					draw_straight_horizontal_pipe(i, j, BLACK);
					break;
				case 2:
					draw_straight_vertical_pipe(i, j, BLACK);
					break;
				case 3:
					draw_left_up_pipe(i, j, BLACK);
					break;
				case 4:
					draw_right_up_pipe(i, j, BLACK);
					break;
				case 5:
					draw_right_down_pipe(i, j, BLACK);
					break;
				case 6:
					draw_left_down_pipe(i, j, BLACK);
					break;
				default:
					break;
			}
			
			
				
			int colour = WHITE;
			
			if (water_pipe[j][i] == 1){
				colour = CYAN;
			}else if(connect_pipe[j][i] == 1){
				colour = ORANGE;
			}
			
			switch(level3[j][i]){
				case 1:
					draw_straight_horizontal_pipe(i, j, colour);
					break;
				case 2:
					draw_straight_vertical_pipe(i, j, colour);
					break;
				case 3:
					draw_left_up_pipe(i, j, colour);
					break;
				case 4:
					draw_right_up_pipe(i, j, colour);
					break;
				case 5:
					draw_right_down_pipe(i, j, colour);
					break;
				case 6:
					draw_left_down_pipe(i, j, colour);
					break;
				default:
					break;
			}
		}
	}
}

void user_move(int direction){
	switch(direction){
		case UP:
			if(USER_Y > 0){
				pre_USER_Y = USER_Y;
				USER_Y = USER_Y - 1;
			}
			break;
		case DOWN:
			if(USER_Y < NUM_BOXES_Y -1){
				pre_USER_Y = USER_Y;
				USER_Y = USER_Y + 1;
			}
			break;
		case LEFT:
			if(USER_X > 0){
				pre_USER_X = USER_X;
				USER_X = USER_X - 1;
			}
			break;
		case RIGHT:
			if(USER_X < NUM_BOXES_X -1){
				pre_USER_X = USER_X;
				USER_X = USER_X + 1;
			}
			break;
		case (UP + LEFT):
			if(USER_Y > 0 && USER_X > 0){
				pre_USER_X = USER_X;
				pre_USER_Y = USER_Y;
				USER_Y = USER_Y - 1;
				USER_X = USER_X - 1;
			}
			break;
		case (UP + RIGHT):
			if(USER_Y > 0 && USER_X < NUM_BOXES_X -1){
				pre_USER_X = USER_X;
				pre_USER_Y = USER_Y;
				USER_Y = USER_Y - 1;
				USER_X = USER_X + 1;
			}
			break;
		case (DOWN + LEFT):
			if(USER_Y < NUM_BOXES_Y -1 && USER_X > 0){
				pre_USER_X = USER_X;
				pre_USER_Y = USER_Y;
				USER_Y = USER_Y + 1;
				USER_X = USER_X - 1;
			}
			break;
		case (DOWN + RIGHT):
			if(USER_Y < NUM_BOXES_Y -1 && USER_X < NUM_BOXES_X -1){
				pre_USER_X = USER_X;
				pre_USER_Y = USER_Y;
				USER_Y = USER_Y + 1;
				USER_X = USER_X + 1;
			}
			break;
		default:
			break;
	}
}

void user_turn(int direction){
	switch(level3[USER_Y][USER_X]){
		case 1:
			
			level3[USER_Y][USER_X] = 2;
			temple[USER_Y][USER_X] = 1;
			available[USER_Y][USER_X] = available[USER_Y][USER_X] * 2;
			break;
		case 2:
			
			level3[USER_Y][USER_X] = 1;
			temple[USER_Y][USER_X] = 2;
			available[USER_Y][USER_X] = available[USER_Y][USER_X] / 2;
			break;
		case 3:
			
			if(direction == CCW){
				level3[USER_Y][USER_X] = 6;
				available[USER_Y][USER_X] = 3;
			}else if(direction == CW){
				level3[USER_Y][USER_X]++;
				available[USER_Y][USER_X] = 12;
			}
			temple[USER_Y][USER_X] = 3;
			break;
		case 4:
			
			if(direction == CCW){
				level3[USER_Y][USER_X]--;
				available[USER_Y][USER_X] = 9;
			}else if(direction == CW){
				level3[USER_Y][USER_X]++;
				available[USER_Y][USER_X] = available[USER_Y][USER_X] / 2;
			}
			temple[USER_Y][USER_X] = 4;
			break;
		case 5:
			
			if(direction == CCW){
				level3[USER_Y][USER_X]--;
				available[USER_Y][USER_X] = available[USER_Y][USER_X] * 2;
			}else if(direction == CW){
				level3[USER_Y][USER_X]++;
				available[USER_Y][USER_X] = available[USER_Y][USER_X] / 2;
			}
			temple[USER_Y][USER_X] = 5;
			break;
		case 6:
		
			if(direction == CCW){
				level3[USER_Y][USER_X]--;
				available[USER_Y][USER_X] = available[USER_Y][USER_X] * 2;
			}else if(direction == CW){
				level3[USER_Y][USER_X] = 3;
				available[USER_Y][USER_X] = 9;
			}
			temple[USER_Y][USER_X] = 6;
			break;
		default:
			break;
		
	}
	
}

void draw_straight_horizontal_pipe(int x, int y, int colour){
	int x0 = FIRST_X + x * 30 + (x + 1) * 1;
	int y0 = FIRST_Y + y * 30 + (y + 1) * 1;
	
	int i;
	int j;
	int clean = 0;
	
	if(colour == BLACK){
		clean = 1;
	}
	
	for(i = x0; i < x0 + 30; i++){
		if(i == x0 || i == x0 + 29){
			if(clean){
				plot_pixel(i, y0 + 11, colour);
				plot_pixel(i, y0 + 18, colour);
			}else{
				plot_pixel(i, y0 + 11, WHITE);
				plot_pixel(i, y0 + 18, WHITE);
			}
		}
		for(j = y0 + 12; j < y0 + 18; j++){
			if(clean){
				plot_pixel(i, j, colour);
			}else{
				if(j == y0 + 12 || j == y0 + 17){
					plot_pixel(i, j, WHITE);
				}else{
					plot_pixel(i, j, colour);
				}
			}
			
		}
	}
	
}
void draw_straight_vertical_pipe(int x, int y, int colour){
	int x0 = FIRST_X + x * 30 + (x + 1) * 1;
	int y0 = FIRST_Y + y * 30 + (y + 1) * 1;
	int clean = 0;
	
	if(colour == BLACK){
		clean = 1;
	}
	
	int i;
	int j;
	for(j = y0; j < y0 + 30; j++){
		if(j == y0 || j == y0 + 29){
			if(clean){
				plot_pixel(x0 + 11, j, colour);
				plot_pixel(x0 + 18, j, colour);
			}else{
				plot_pixel(x0 + 11, j, WHITE);
				plot_pixel(x0 + 18, j, WHITE);
			}
		}
		for(i = x0 + 12; i < x0 + 18; i++){
			if(clean){
				plot_pixel(i, j, colour);
			}else{
				if(i == x0 + 12 || i == x0 + 17){
					plot_pixel(i, j, WHITE);
				}else{
					plot_pixel(i, j, colour);
				}
				
			}
			
		}
	}
}

void draw_left_up_pipe(int x, int y, int colour){// NO. 3
	int x0 = FIRST_X + x * 30 + (x + 1) * 1;
	int y0 = FIRST_Y + y * 30 + (y + 1) * 1;
	int clean = 0;
	
	if(colour == BLACK){
		clean = 1;
	}
	
	int i;
	int j;
	for(i = x0; i < x0 + 12; i++){
		if(i == x0){
			if(clean){
				plot_pixel(i, y0 + 11, colour);
				plot_pixel(i, y0 + 18, colour);
			}else{
				plot_pixel(i, y0 + 11, WHITE);
			    plot_pixel(i, y0 + 18, WHITE);
			}
		}
		for(j = y0 + 12; j < y0 + 18; j++){
			if(clean){
				plot_pixel(i, j, colour);
			}else{
				if(j == y0 + 12 || j == y0 + 17){
					plot_pixel(i, j, WHITE);
				}else{
					plot_pixel(i, j, colour);
				}
			}
		}
	}
	
	for(j = y0; j < y0 + 12; j++){
		if(j == y0){
			if(clean){
				plot_pixel(x0 + 11, j, colour);
				plot_pixel(x0 + 18, j, colour);
			}else{
				plot_pixel(x0 + 11, j, WHITE);
				plot_pixel(x0 + 18, j, WHITE);
			}
		}
		for(i = x0 + 12; i < x0 + 18; i++){
			if(clean){
				plot_pixel(i, j, colour);
			}else{
				if(i == x0+12 || i == x0+17){
					plot_pixel(i, j, WHITE);
				}else{
					plot_pixel(i, j, colour);
				}
			}
		}
	}
	
	for(i = x0 + 12; i < x0 + 18; i++){
		int y_end; 
		if(i < x0 + 14){
			y_end = y0 + 18;
		}else if(i < x0 + 16){
			y_end = y0 + 17;
		}else if(i == x0 + 16){
			y_end = y0 + 16;
		}else{
			y_end = y0 + 14;
		}
		for(j = y0 + 12; j < y_end; j++){
			if((j == (y_end - 1) || i == x0+17 || (i == x0+16 && j == y0+14)) && !clean){
				plot_pixel(i, j, WHITE);
			}else{
				plot_pixel(i, j, colour);
			}
		}
	}
}


void draw_right_up_pipe(int x, int y, int colour){
	int x0 = FIRST_X + x * 30 + (x + 1) * 1;
	int y0 = FIRST_Y + y * 30 + (y + 1) * 1;
	int clean = 0;
	
	if(colour == BLACK){
		clean = 1;
	}
	
	int i;
	int j;
	for(i = x0 + 18; i < x0 + 30; i++){
		if(i == x0 + 29){
			if(clean){
				plot_pixel(i, y0 + 11, colour);
				plot_pixel(i, y0 + 18, colour);
			}else{
				plot_pixel(i, y0 + 11, WHITE);
			    plot_pixel(i, y0 + 18, WHITE);
			}
		}
		for(j = y0 + 12; j < y0 + 18; j++){
			if(clean){
				plot_pixel(i, j, colour);
			}else{
				if(j == y0 + 12 || j == y0 + 17){
					plot_pixel(i, j, WHITE);
				}else{
					plot_pixel(i, j, colour);
				}
			}
		}
	}
	
	for(j = y0; j < y0 + 12; j++){
		if(j == y0){
			if(clean){
				plot_pixel(x0 + 11, j, colour);
				plot_pixel(x0 + 18, j, colour);
			}else{
				plot_pixel(x0 + 11, j, WHITE);
				plot_pixel(x0 + 18, j, WHITE);
			}
		}
		for(i = x0 + 12; i < x0 + 18; i++){
			
			if(clean){
				plot_pixel(i, j, colour);
			}else{
				if(i == x0+12 || i == x0+17){
					plot_pixel(i, j, WHITE);
				}else{
					plot_pixel(i, j, colour);
				}
			}

		}
	}
	
	for(i = x0 + 12; i < x0 + 18; i++){
		int y_end;
		
		if(i == x0 + 12){
			y_end = y0 + 14;
		}else if(i == x0 + 13){
			y_end = y0 + 16;
		}else if(i < x0 + 16){
			y_end = y0 + 17;
		}else if(i < x0 + 18){
			y_end = y0 + 18;
		}
		for(j = y0 + 12; j < y_end; j++){
			if((j == y_end -1 || i == x0+12 || (i == x0+13 && j == y0+14)) && !clean){
				plot_pixel(i, j, WHITE);
			}else{
				plot_pixel(i, j, colour);
			}
		}
	}
}


void draw_right_down_pipe(int x, int y, int colour){
	int x0 = FIRST_X + x * 30 + (x + 1) * 1;
	int y0 = FIRST_Y + y * 30 + (y + 1) * 1;
	int clean = 0;
	
	if(colour == BLACK){
		clean = 1;
	}
	
	int i;
	int j;
	for(i = x0 + 18; i < x0 + 30; i++){
		if(i == x0 + 29){
			if(clean){
				plot_pixel(i, y0 + 11, colour);
				plot_pixel(i, y0 + 18, colour);
			}else{
				plot_pixel(i, y0 + 11, WHITE);
			    plot_pixel(i, y0 + 18, WHITE);
			}
		}
		for(j = y0 + 12; j < y0 + 18; j++){
			if(clean){
				plot_pixel(i, j, colour);
			}else{
				if(j == y0 + 12 || j == y0 + 17){
					plot_pixel(i, j, WHITE);
				}else{
					plot_pixel(i, j, colour);
				}
			}
		}
	}
	
	for(j = y0 + 18; j < y0 + 30; j++){
		if(j == y0 + 29){
			if(clean){
				plot_pixel(x0 + 11, j, colour);
				plot_pixel(x0 + 18, j, colour);
			}else{
				plot_pixel(x0 + 11, j, WHITE);
				plot_pixel(x0 + 18, j, WHITE);
			}
		}
		for(i = x0 + 12; i < x0 + 18; i++){
			if(clean){
				plot_pixel(i, j, colour);
			}else{
				if( i == x0+12 || i== x0+17){
					plot_pixel(i, j, WHITE);
				}else{
					plot_pixel(i, j, colour);
				}
			}
			
		}
	}
	
	for(i = x0 + 12; i < x0 + 18; i++){
		int y_start; 
		if(i == x0 + 12){
			y_start = y0 + 16;
		}else if(i == x0 + 13){
			y_start = y0 + 14;
		}else if(i < x0 + 16){
			y_start = y0 + 13;
		}else if(i < x0 + 17){
			y_start = y0 + 12;
		}
		for(j = y_start; j < y0 + 18; j++){
			if((j == y_start || i == x0+12 || (i == x0+13 && j == y0+15)) && !clean){
				plot_pixel(i, j, WHITE);
			}else{
				plot_pixel(i, j, colour);
			}
		}
	}
}

//change this func
void draw_left_down_pipe(int x, int y, int colour){//NO.6
	int x0 = FIRST_X + x * 30 + (x + 1) * 1;
	int y0 = FIRST_Y + y * 30 + (y + 1) * 1;
	int clean = 0;
	
	if(colour == BLACK){
		clean = 1;
	}
	
	int i;
	int j;
	for(i = x0; i < x0 + 12; i++){
		if(i == x0){
			if(clean){
				plot_pixel(i, y0 + 11, colour);
				plot_pixel(i, y0 + 18, colour);
			}else{
				plot_pixel(i, y0 + 11, WHITE);
				plot_pixel(i, y0 + 18, WHITE);
			}
		}
		for(j = y0 + 12; j < y0 + 18; j++){
			if(clean){
				plot_pixel(i, j, colour);
			}else{
				if(j == y0+12 || j == y0+17){
					plot_pixel(i, j, WHITE);
				}else{
					plot_pixel(i, j, colour);
				}
			}
		}
	}
	
	for(j = y0 + 18; j < y0 + 30; j++){
		if(j == y0 + 29){
			if(clean){
				plot_pixel(x0 + 11, j, colour);
				plot_pixel(x0 + 18, j, colour);
			}else{
				plot_pixel(x0 + 11, j, WHITE);
				plot_pixel(x0 + 18, j, WHITE);
			}
			
		}
		for(i = x0 + 12; i < x0 + 18; i++){
			if(clean){
				plot_pixel(i, j, colour);
			}else{
				if(i == x0+12 || i == x0+17){
					plot_pixel(i, j, WHITE);
				}else{
					plot_pixel(i, j, colour);
				}
			}
		}
	}
	
	for(i = x0 + 12; i < x0 + 18; i++){
		int y_start; 
		
		if(i < x0 + 14){
			y_start = y0 + 12; 
		}else if(i < x0 + 16){
			y_start = y0 + 13; 
		}else if(i == x0 + 16){
			y_start = y0 + 14;
		}else if(i == x0 + 17){
			y_start = y0 + 16; 
		}
		for(j = y_start; j < y0 + 18; j++){
			if((j == y_start || i == x0+17 || (i == x0+17 && j == y0+15)) && !clean){
				plot_pixel(i, j, WHITE);
			}else{
				plot_pixel(i, j, colour);
			}
		}
	}
}
					 
void water_straight_horizontal_pipe(int x, int y, int dir){
	int x0 = FIRST_X + x * 30 + (x + 1) * 1;
	int y0 = FIRST_Y + y * 30 + (y + 1) * 1;
	
	int i;
	int j;

	int change;
	int end;
	if(dir == 2){
		change = -1;
		end = x0;
		
		if(water[0][0] > end){
			water[0][0] = water[0][0] + change * 3;
			water[1][0] = water[1][0] + change * 3;
			if(water[0][0] < end){
				water[0][0] = end;
				water[1][0] = end;
				water_pipe[y][x] = 1;
			}
			
			for(i = water[0][1]; i <= water[1][1]; i++){
				for(j = water[0][0]; j <= pre_water[0][0]; j++){
					plot_pixel(j, i, CYAN);
				}
			}
				
		}else{
			water[0][0] = water[0][0] + change * 2;
			water[1][0] = water[1][0] + change * 2;
			path_num++;
			water_in_dir = 2;
			pre_water[0][0] = water[0][0];
			pre_water[1][0] = water[1][0];
		}
	}else{
		change = 1;
		end = x0+ 29;
		if(water[0][0] < end){
			water[0][0] = water[0][0] + change * 3;
			water[1][0] = water[1][0] + change * 3;
			if(water[0][0] > end){
				water[0][0] = end;
				water[1][0] = end;
				water_pipe[y][x] = 1;
			}
			for(i = water[0][1]; i <= water[1][1]; i++){
				for(j = pre_water[0][0]; j <= water[0][0]; j++){
					plot_pixel(j, i, CYAN);
				}
			}
				
		}else{
			water[0][0] = water[0][0] + change * 2;
			water[1][0] = water[1][0] + change * 2;
			path_num++;
			pre_water[0][0] = water[0][0];
			pre_water[1][0] = water[1][0];
			water_in_dir = 4;
		}
	}
	
	
}
					 
void water_straight_vertical_pipe(int x, int y, int dir){
	int x0 = FIRST_X + x * 30 + (x + 1) * 1;
	int y0 = FIRST_Y + y * 30 + (y + 1) * 1;

	
	int i;
	int j;
	
	int change;
	int end;
	if(dir == 3){
		change = -1;
		end = y0;
		if(water[0][1] > end){
			water[0][1] = water[0][1] + change * 3;
			water[1][1] = water[1][1] + change * 3;
			if(water[0][1] < end){
				water[0][1] = end;
				water[1][1] = end;
				water_pipe[y][x] = 1;
			}
			for(i = water[0][0]; i <= water[1][0]; i++){
				for(j = water[0][1]; j <= pre_water[0][1]; j++){
					plot_pixel(i, j, CYAN);
				}
			}
			
		}else{
			water[0][1] = water[0][1] + change * 2;
			water[1][1] = water[1][1] + change * 2;
			path_num++;
			water_in_dir = 3;
			pre_water[0][1] = water[0][1];
			pre_water[1][1] = water[1][1];
		}
	}else{
		change = 1;
		end = y0+ 29;
		if(water[0][1] < end){
			water[0][1] = water[0][1] + change * 6;
			water[1][1] = water[1][1] + change * 6;
			if(water[0][1] > end){
				water[0][1] = end;
				water[1][1] = end;
				water_pipe[y][x] = 1;
			}
			for(i = water[0][0]; i <= water[1][0]; i++){
				for(j = pre_water[0][1]; j <= water[0][1]; j++){
					plot_pixel(i, j, CYAN);
				}
			}
				
		}else{
			water[0][1] = water[0][1] + change * 2;
			water[1][1] = water[1][1] + change * 2;
			path_num++;
			water_in_dir = 1;
			pre_water[0][1] = water[0][1];
			pre_water[1][1] = water[1][1];
		}
	}
	
	
}
					 
void water_left_up_pipe(int x, int y, int dir){
	int x0 = FIRST_X + x * 30 + (x + 1) * 1;
	int y0 = FIRST_Y + y * 30 + (y + 1) * 1;

	
	int i;
	int j;
	
	int change1, change3;
	int end1, end3;
	if(dir == 1){
		change1 = 1;
		end1 = y0 + 11;
		change3 = -1;
		end3 = x0;
		
		if(water[0][1] < end1){
			water[0][1] = water[0][1] + change1 * 6;
			water[1][1] = water[1][1] + change1 * 6;
			if(water[0][1] > end1){
				water[0][1] = end1;
				water[1][1] = end1;
			}
			for(i = water[0][0]; i <= water[1][0]; i++){
				for(j = pre_water[0][1]; j <= water[0][1]; j++){
					plot_pixel(i, j, CYAN);
				}
			}
		}else{
			for(i = x0 + 13; i <= x0 + 16; i++){
				for(j = y0; j <= y0 + 11; j++){
					plot_pixel(i, j, CYAN);
				}
			}
			if(water[0][1] == end1){
				water[0][0] = x0 + 11;
				water[0][1] = y0 + 13;
				water[1][0] = water[0][0];
				water[1][1] = y0 + 16;
				pre_water[0][1] = water[0][1];
				pre_water[1][1] = water[1][1];
				pre_water[0][0] = water[0][0];
				pre_water[1][0] = water[1][0];
			}
			for(i = x0 + 12; i < x0 + 17; i++){
				int y_end; 
				if(i < x0 + 14){
					y_end = y0 + 17;
				}else if(i < x0 + 16){
					y_end = y0 + 16;
				}else if(i == x0 + 16){
					y_end = y0 + 14;
				}
				for(j = y0 + 12; j < y_end; j++){
					plot_pixel(i, j, CYAN);
				}
			}
			
			if(water[0][0] > end3){
				water[0][0] = water[0][0] + change3 * 3;
				water[1][0] = water[1][0] + change3 * 3;
				if(water[0][0] < end3){
					water[0][0] = end3;
					water[1][0] = end3;
					water_pipe[y][x] = 1;
				}
				for(i = water[0][1]; i <= water[1][1]; i++){
					for(j = water[0][0]; j <= pre_water[0][0]; j++){
						plot_pixel(j, i, CYAN);
					}
				}
			}else{
				water[0][0] = water[0][0] + change3 * 2;
				water[1][0] = water[1][0] + change3 * 2;
				path_num++;
				water_in_dir = 2;
				pre_water[0][0] = water[0][0];
				pre_water[1][0] = water[1][0];
			}
		}
			
	}else{
		change1 = 1;
		end1 = x0 + 11;
		change3 = -1;
		end3 = y0;
		
		if(water[0][0] < end1){
			water[0][0] = water[0][0] + change1 * 3;
			water[1][0] = water[1][0] + change1 * 3;
			if(water[0][0] > end1){
				water[0][0] = end1;
				water[1][0] = end1;
			}
			for(i = water[0][1]; i <= water[1][1]; i++){
				for(j = pre_water[0][0]; j <= water[0][0]; j++){
					plot_pixel(j, i, CYAN);
				}
			}
		}else{
			for(i = x0; i <= x0 + 11; i++){
				for(j = y0 + 13; j <= y0 + 16; j++){
					plot_pixel(i, j, CYAN);
				}
			}
			if(water[0][0] == end1){
				water[0][0] = x0 + 13;
				water[0][1] = y0 + 11;
				water[1][1] = water[0][1];
				water[1][0] = x0 + 16;
				pre_water[0][1] = water[0][1];
				pre_water[1][1] = water[1][1];
				pre_water[0][0] = water[0][0];
				pre_water[1][0] = water[1][0];
			}
			for(i = x0 + 12; i < x0 + 17; i++){
				int y_end; 
				if(i < x0 + 14){
					y_end = y0 + 17;
				}else if(i < x0 + 16){
					y_end = y0 + 16;
				}else if(i == x0 + 16){
					y_end = y0 + 14;
				}
				for(j = y0 + 12; j < y_end; j++){
					plot_pixel(i, j, CYAN);
				}
			}
			if(water[0][1] > end3){
				water[0][1] = water[0][1] + change3 * 3;
				water[1][1] = water[1][1] + change3 * 3;
				if(water[0][1] < end3){
					water[0][1] = end3;
					water[1][1] = end3;
					water_pipe[y][x] = 1;
				}
				for(i = water[0][0]; i <= water[1][0]; i++){
					for(j = pre_water[0][1]; j >= water[0][1]; j--){
						plot_pixel(i, j, CYAN);
					}
				}
			}else{
				water[0][1] = water[0][1] + change3 * 2;
				water[1][1] = water[1][1] + change3 * 2;
				path_num++;
				water_in_dir = 3;
				pre_water[0][1] = water[0][1];
				pre_water[1][1] = water[1][1];
			}
		}
	}
}
					 
void water_right_up_pipe(int x, int y, int dir){
	int x0 = FIRST_X + x * 30 + (x + 1) * 1;
	int y0 = FIRST_Y + y * 30 + (y + 1) * 1;

	
	int i;
	int j;
	
	int change1, change3;
	int end1, end3;
	if(dir == 1){
		change1 = 1;
		end1 = y0 + 11;
		change3 = 1;
		end3 = x0+29;
		
		if(water[0][1] < end1){
			water[0][1] = water[0][1] + change1 * 6;
			water[1][1] = water[1][1] + change1 * 6;
			if(water[0][1] > end1){
				water[0][1] = end1;
				water[1][1] = end1;
			}
			for(i = water[0][0]; i <= water[1][0]; i++){
				for(j = pre_water[0][1]; j <= water[0][1]; j++){
					plot_pixel(i, j, CYAN);
				}
			}
		}else{
			for(i = x0 + 13; i <= x0 + 16; i++){
				for(j = y0; j <= y0 + 11; j++){
					plot_pixel(i, j, CYAN);
				}
			}
			if(water[0][1] == end1){
				water[0][0] = x0 + 18;
				water[0][1] = y0 + 13;
				water[1][0] = water[0][0];
				water[1][1] = y0 + 16;
				pre_water[0][1] = water[0][1];
				pre_water[1][1] = water[1][1];
				pre_water[0][0] = water[0][0];
				pre_water[1][0] = water[1][0];
			}
			
			for(i = x0 + 13; i < x0 + 18; i++){
				int y_end;

				if(i == x0 + 13){
					y_end = y0 + 14;
				}else if(i < x0 + 16){
					y_end = y0 + 16;
				}else if(i < x0 + 18){
					y_end = y0 + 17;
				}
				for(j = y0 + 12; j < y_end; j++){
					plot_pixel(i, j, CYAN);
				}
			}
			
			if(water[0][0] < end3){
				water[0][0] = water[0][0] + change3 * 3;
				water[1][0] = water[1][0] + change3 * 3;
				if(water[0][0] > end3){
					water[0][0] = end3;
					water[1][0] = end3;
					water_pipe[y][x] = 1;
				}
				for(i = water[0][1]; i <= water[1][1]; i++){
					for(j = pre_water[0][0]; j <= water[0][0]; j++){
						plot_pixel(j, i, CYAN);
					}
				}
			}else{
				water[0][0] = water[0][0] + change3 * 2;
				water[1][0] = water[1][0] + change3 * 2;
				path_num++;
				water_in_dir = 4;
				pre_water[0][0] = water[0][0];
				pre_water[1][0] = water[1][0];
			}
		}
			
	}else{
		change1 = -1;
		end1 = x0 + 16;
		change3 = -1;
		end3 = y0;
		
		if(water[0][0] > end1){
			water[0][0] = water[0][0] + change1 * 3;
			water[1][0] = water[1][0] + change1 * 3;
			if(water[0][0] < end1){
				water[0][0] = end1;
				water[1][0] = end1;
			}
			for(i = water[0][1]; i <= water[1][1]; i++){
				for(j = pre_water[0][0]; j >= water[0][0]; j--){
					plot_pixel(j, i, CYAN);
				}
			}
		}else{
			for(i = x0 + 18; i < x0 + 30; i++){
				for(j = y0 + 13; j < y0 + 17; j++){
					plot_pixel(i, j, CYAN);
				}
			}
			if(water[0][0] == end1){
				water[0][0] = x0 + 13;
				water[0][1] = y0 + 11;
				water[1][1] = water[0][1];
				water[1][0] = x0 + 16;
				pre_water[0][1] = water[0][1];
				pre_water[1][1] = water[1][1];
				pre_water[0][0] = water[0][0];
				pre_water[1][0] = water[1][0];
			}
			for(i = x0 + 13; i < x0 + 18; i++){
				int y_end;

				if(i == x0 + 13){
					y_end = y0 + 14;
				}else if(i < x0 + 16){
					y_end = y0 + 16;
				}else if(i < x0 + 18){
					y_end = y0 + 17;
				}
				for(j = y0 + 12; j < y_end; j++){
					plot_pixel(i, j, CYAN);
				}
			}
			if(water[0][1] > end3){
				water[0][1] = water[0][1] + change3 * 3;
				water[1][1] = water[1][1] + change3 * 3;
				if(water[0][1] < end3){
					water[0][1] = end3;
					water[1][1] = end3;
					water_pipe[y][x] = 1;
				}
				for(i = water[0][0]; i <= water[1][0]; i++){
					for(j = pre_water[0][1]; j >= water[0][1]; j--){
						plot_pixel(i, j, CYAN);
					}
				}
					
			}else{
				water[0][1] = water[0][1] + change3 * 2;
				water[1][1] = water[1][1] + change3 * 2;
				path_num++;
				water_in_dir = 3;
				pre_water[0][1] = water[0][1];
				pre_water[1][1] = water[1][1];
			}
		}
	}	
}
					 
void water_right_down_pipe(int x, int y, int dir){
	int x0 = FIRST_X + x * 30 + (x + 1) * 1;
	int y0 = FIRST_Y + y * 30 + (y + 1) * 1;

	
	int i;
	int j;
	
	int change1, change3;
	int end1, end3;
	if(dir == 3){
		change1 = -1;
		end1 = y0 + 18;
		change3 = 1;
		end3 = x0+29;
		
		if(water[0][1] > end1){
			water[0][1] = water[0][1] + change1 * 3;
			water[1][1] = water[1][1] + change1 * 3;
			if(water[0][1] < end1){
				water[0][1] = end1;
				water[1][1] = end1;
			}
			for(i = water[0][0]; i <= water[1][0]; i++){
				for(j = pre_water[0][1]; j >= water[0][1]; j--){
					plot_pixel(i, j, CYAN);
				}
			}
		}else{
			for(i = x0 + 13; i <= x0 + 16; i++){
				for(j = y0+18; j <= y0 + 29; j++){
					plot_pixel(i, j, CYAN);
				}
			}
			if(water[0][1] == end1){
				water[0][0] = x0 + 18;
				water[0][1] = y0 + 13;
				water[1][0] = water[0][0];
				water[1][1] = y0 + 16;
				pre_water[0][1] = water[0][1];
				pre_water[1][1] = water[1][1];
				pre_water[0][0] = water[0][0];
				pre_water[1][0] = water[1][0];
			}
			
			for(i = x0 + 13; i < x0 + 18; i++){
				int y_start; 
				if(i == x0 + 13){
					y_start = y0 + 16;
				}else if(i < x0 + 16){
					y_start = y0 + 14;
				}else if(i < x0 + 17){
					y_start = y0 + 13;
				}
				for(j = y_start; j < y0 + 18; j++){
					plot_pixel(i, j, CYAN);
				}
			}
			
			if(water[0][0] < end3){
				water[0][0] = water[0][0] + change3 * 3;
				water[1][0] = water[1][0] + change3 * 3;
				if(water[0][0] > end3){
					water[0][0] = end3;
					water[1][0] = end3;
					water_pipe[y][x] = 1;
				}
				for(i = water[0][1]; i <= water[1][1]; i++){
					for(j = pre_water[0][0]; j <= water[0][0]; j++){
						plot_pixel(j, i, CYAN);
					}
				}
			}else{
				water[0][0] = water[0][0] + change3 * 2;
				water[1][0] = water[1][0] + change3 * 2;
				path_num++;
				water_in_dir = 4;
				pre_water[0][0] = water[0][0];
				pre_water[1][0] = water[1][0];
			}
		}
			
	}else{
		change1 = -1;
		end1 = x0 + 16;
		change3 = 1;
		end3 = y0+29;
		
		if(water[0][0] > end1){
			water[0][0] = water[0][0] + change1 * 3;
			water[1][0] = water[1][0] + change1 * 3;
			if(water[0][0] < end1){
				water[0][0] = end1;
				water[1][0] = end1;
			}
			for(i = water[0][1]; i <= water[1][1]; i++){
				for(j = pre_water[0][0]; j >= water[0][0]; j--){
					plot_pixel(j, i, CYAN);
				}
			}
		}else{
			for(i = x0 + 18; i < x0 + 30; i++){
				for(j = y0 + 13; j < y0 + 17; j++){
					plot_pixel(i, j, CYAN);
				}
			}
			if(water[0][0] == end1){
				water[0][0] = x0 + 13;
				water[0][1] = y0 + 18;
				water[1][1] = water[0][1];
				water[1][0] = x0 + 16;
				pre_water[0][1] = water[0][1];
				pre_water[1][1] = water[1][1];
				pre_water[0][0] = water[0][0];
				pre_water[1][0] = water[1][0];
			}
			for(i = x0 + 13; i < x0 + 18; i++){
				int y_start; 
				if(i == x0 + 13){
					y_start = y0 + 16;
				}else if(i < x0 + 16){
					y_start = y0 + 14;
				}else if(i < x0 + 17){
					y_start = y0 + 13;
				}
				for(j = y_start; j < y0 + 18; j++){
					plot_pixel(i, j, CYAN);
				}
			}
			if(water[0][1] < end3){
				water[0][1] = water[0][1] + change3 * 6;
				water[1][1] = water[1][1] + change3 * 6;
				if(water[0][1] > end3){
					water[0][1] = end3;
					water[1][1] = end3;
					water_pipe[y][x] = 1;
				}
				for(i = water[0][0]; i <= water[1][0]; i++){
					for(j = pre_water[0][1]; j <= water[0][1]; j++){
						plot_pixel(i, j, CYAN);
					}
				}
			}else{
				water[0][1] = water[0][1] + change3 * 2;
				water[1][1] = water[1][1] + change3 * 2;
				path_num++;
				water_in_dir = 1;
				pre_water[0][1] = water[0][1];
				pre_water[1][1] = water[1][1];
			}
		}
	}
}
					 
void water_left_down_pipe(int x, int y, int dir){
	int x0 = FIRST_X + x * 30 + (x + 1) * 1;
	int y0 = FIRST_Y + y * 30 + (y + 1) * 1;

	
	int i;
	int j;
	
	int change1, change3;
	int end1, end3;
	if(dir == 3){
		change1 = -1;
		end1 = y0 + 18;
		change3 = -1;
		end3 = x0;
		
		if(water[0][1] > end1){
			water[0][1] = water[0][1] + change1 * 3;
			water[1][1] = water[1][1] + change1 * 3;
			if(water[0][1] < end1){
				water[0][1] = end1;
				water[1][1] = end1;
			}
			for(i = water[0][0]; i <= water[1][0]; i++){
				for(j = pre_water[0][1]; j >= water[0][1]; j--){
					plot_pixel(i, j, CYAN);
				}
			}
		}else{
			for(i = x0 + 13; i <= x0 + 16; i++){
				for(j = y0+18; j <= y0 + 29; j++){
					plot_pixel(i, j, CYAN);
				}
			}
			if(water[0][1] == end1){
				water[0][0] = x0 + 11;
				water[0][1] = y0 + 13;
				water[1][0] = water[0][0];
				water[1][1] = y0 + 16;
				pre_water[0][1] = water[0][1];
				pre_water[1][1] = water[1][1];
				pre_water[0][0] = water[0][0];
				pre_water[1][0] = water[1][0];
			}
			
			for(i = x0 + 12; i < x0 + 17; i++){
				int y_start; 

				if(i < x0 + 14){
					y_start = y0 + 13; 
				}else if(i < x0 + 16){
					y_start = y0 + 14; 
				}else if(i == x0 + 16){
					y_start = y0 + 16;
				}
				for(j = y_start; j < y0 + 18; j++){
					plot_pixel(i, j, CYAN);
				}
			}
			
			if(water[0][0] > end3){
				water[0][0] = water[0][0] + change3 * 3;
				water[1][0] = water[1][0] + change3 * 3;
				if(water[0][0] < end3){
					water[0][0] = end3;
					water[1][0] = end3;
					water_pipe[y][x] = 1;
				}
				for(i = water[0][1]; i <= water[1][1]; i++){
					for(j = water[0][0]; j <= pre_water[0][0]; j++){
						plot_pixel(j, i, CYAN);
					}
				}
			}else{
				water[0][0] = water[0][0] + change3 * 2;
				water[1][0] = water[1][0] + change3 * 2;
				path_num++;
				water_in_dir = 2;
				pre_water[0][0] = water[0][0];
				pre_water[1][0] = water[1][0];
			}
		}
			
	}else{
		change1 = 1;
		end1 = x0 + 11;
		change3 = 1;
		end3 = y0+29;
		
		if(water[0][0] < end1){
			water[0][0] = water[0][0] + change1 * 3;
			water[1][0] = water[1][0] + change1 * 3;
			if(water[0][0] > end1){
				water[0][0] = end1;
				water[1][0] = end1;
			}
			for(i = water[0][1]; i <= water[1][1]; i++){
				for(j = pre_water[0][0]; j <= water[0][0]; j++){
					plot_pixel(j, i, CYAN);
				}
			}
		}else{
			for(i = x0; i <= x0 + 11; i++){
				for(j = y0 + 13; j <= y0 + 16; j++){
					plot_pixel(i, j, CYAN);
				}
			}
			if(water[0][0] == end1){
				water[0][0] = x0 + 13;
				water[0][1] = y0 + 18;
				water[1][1] = water[0][1];
				water[1][0] = x0 + 16;
				pre_water[0][1] = water[0][1];
				pre_water[1][1] = water[1][1];
				pre_water[0][0] = water[0][0];
				pre_water[1][0] = water[1][0];
			}
			for(i = x0 + 12; i < x0 + 17; i++){
				int y_start; 

				if(i < x0 + 14){
					y_start = y0 + 13; 
				}else if(i < x0 + 16){
					y_start = y0 + 14; 
				}else if(i == x0 + 16){
					y_start = y0 + 16;
				}
				for(j = y_start; j < y0 + 18; j++){
					plot_pixel(i, j, CYAN);
				}
			}
			
			if(water[0][1] < end3){
				water[0][1] = water[0][1] + change3 * 6;
				water[1][1] = water[1][1] + change3 * 6;
				if(water[0][1] > end3){
					water[0][1] = end3;
					water[1][1] = end3;
					water_pipe[y][x] = 1;
				}
				for(i = water[0][0]; i <= water[1][0]; i++){
					for(j = y0 + 18; j <= water[0][1]; j++){
						plot_pixel(i, j, CYAN);
					}
				}
					
			}else{
				water[0][1] = water[0][1] + change3 * 2;
				water[1][1] = water[1][1] + change3 * 2;
				path_num++;
				water_in_dir = 1;
				pre_water[0][1] = water[0][1];
				pre_water[1][1] = water[1][1];
			}
		}
	}
}

		
			
			