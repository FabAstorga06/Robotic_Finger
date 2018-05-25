#ifndef _ROBOTIC_LIBRARY_H
#define _ROBOTIC_LIBRARY_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define PI  3.14159265358979323846
#define GEAR_D  56
#define TOUCH_T 250000
#define Z_TOUCH 45     /******* Verificar coordenada Z *********/

FILE *_dev;   
char *_port;

typedef struct point3d {
	uint8_t x;
	uint8_t y;
	uint8_t z;
  } point;

uint8_t current_x = 0;
uint8_t current_y = 0;


/*************************  LIBRARY FUNCTIONS     *******************************/

//Set port file
int set_port(char *pport ) {
	//Open port file
	_dev = fopen(pport, "r+");
	if (!_dev)	{
		//Returns false if failed
		printf("Error opening port\n");
		return 0;
	} 
	else  {
		//Set port if successful
		fclose(_dev);
		_port = pport;
		return 1;
	}
}

//  Converts mm to degrees for the servos
int mm2deg(float mm) {
	return (mm/PI/GEAR_D*360) ;
}


//Press the screen
void press(uint8_t ptime ) {
	//Open port file
	_dev = fopen(_port, "r+");
	//Sets z axis to touch the screen
	point p = {.x = current_x, .y = current_y, .z = Z_TOUCH};
	if(_dev != NULL){
		//Send coordinates 
		fwrite((const void *)(&p), sizeof(point), 1, _dev);
		//Delay and move finger up
		sleep(ptime); 
		p.z = 0;
		fwrite((const void *)(&p), sizeof(point), 1, _dev);
		fclose(_dev);
	}
	else
	{
		printf("I/O Error\n");
	}
}


//Taps the screen
void touch()  {
	//Open port file
	_dev = fopen(_port, "r+");
	//Sets z axis to touch the screen
	point p = {.x = current_x, .y = current_y, .z = Z_TOUCH};
	if(_dev != NULL){
		//Send coordinates 
		fwrite((const void *)(&p), sizeof(point), 1, _dev);
		//Very small delay and move finger up
		usleep(TOUCH_T);
		p.z = 0;
		fwrite((const void *)(&p), sizeof(point), 1, _dev);
		fclose(_dev);
	}
	else
	{
		printf("I/O Error\n");
	}
}

//Moves the finger to the given coordinates
void move(float pos_x, float pos_y )  {
	//Open port file
	_dev = fopen(_port, "r+");
	//Set X,Y coordinates to move the finger to
	point p = {.x = mm2deg(pos_x), .y = mm2deg(pos_y), .z = 0};
	if(_dev != NULL) {
		//Send coordinates
		fwrite((const void *)(&p), sizeof(point), 1, _dev);
		fclose(_dev);
		current_x = p.x;
		current_y = p.y;
	}
	else 	{
		printf("I/O Error\n");
	}
}

//Moves the finger to the given coordinates
void move_deg(int pos_x, int pos_y )  {
	//Open port file
	_dev = fopen(_port, "r+");
	//Set X,Y coordinates to move the finger to
	point p = {.x = pos_x, .y = pos_y, .z = 0};
	if(_dev != NULL){
		//Send coordinates
		fwrite((const void *)(&p), sizeof(point), 1, _dev);
		fclose(_dev);
		current_x = p.x;
		current_y = p.y;
	}
	else 	{
		printf("I/O Error\n");
	}
}


#endif

