#ifndef _ROBOTIC_LIBRARY_H
#define _ROBOTIC_LIBRARY_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define TOUCH_T 250000
#define PUSH 20

FILE *_dev;   
char *_port;

typedef struct point3d {
	float x;
	float y;
	float z;
} point;

float current_x = 0;
float current_y = 0;
float current_z = 0;


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


//Press the screen
void press(int ptime ) {
	//Open port file
	_dev = fopen(_port, "r+");
	//Sets z axis to touch the screen
	point p = {.x = current_x, .y = current_y, .z = (current_z + PUSH)};
	if(_dev != NULL){
		//Send coordinates 
		fwrite((const void *)(&p), sizeof(point), 1, _dev);
		p.z = current_z;
		fclose(_dev);
		sleep(ptime+2);
	}
	else
	{
		printf("I/O Error\n");
	}
	_dev = fopen(_port, "r+");
	if(_dev != NULL){
		fwrite((const void *)(&p), sizeof(point), 1, _dev);
		fclose(_dev);
	}
	else
	{
		printf("I/O Error\n");
	}
	sleep(3);
}


//Taps the screen
void touch()  {
	//Open port file
	_dev = fopen(_port, "r+");
	//Sets z axis to touch the screen
	point p = {.x = current_x, .y = current_y, .z = (current_z + PUSH)};
	if(_dev != NULL){
		//Send coordinates 
		fwrite((const void *)(&p), sizeof(point), 1, _dev);
		p.z = current_z;
		fclose(_dev);
		sleep(2);
	}
	else
	{
		printf("I/O Error\n");
	}
	_dev = fopen(_port, "r+");
	if(_dev != NULL){
		fwrite((const void *)(&p), sizeof(point), 1, _dev);
		fclose(_dev);
	}
	else
	{
		printf("I/O Error\n");
	}
	sleep(3);
}


//Moves the finger to the given coordinates
void move(float pos_x, float pos_y, float pos_z )  {
	//Open port file
	_dev = fopen(_port, "r+");
	//Set X,Y coordinates to move the finger to
	point p = {.x = pos_x, .y = pos_y, .z = pos_z};
	if(_dev != NULL){
		//Send coordinates
		fwrite((const void *)(&p), sizeof(point), 1, _dev);
		fclose(_dev);
		current_x = p.x;
		current_y = p.y;
		current_z = p.z;
	}
	else 	{
		printf("I/O Error\n");
	}
	sleep(3);
}


#endif

