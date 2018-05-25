%{
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "rflib.h"

//Flex/Bison external variables
extern int yylex();
extern int yyparse();
extern FILE *yyin;

FILE *error_log_file;		//Error log file 

int line_no = 1;	//Line being analyzed
int t = 0;			// Time to be pressed the current position
int x = 0;			// x coordinate matrix
int y = 0;			// y coordenate matrix
int p = 0;			// PIN
int default_keyboard_size = 1;	//Keyboard size, default 1x1cm

const int keyboard[3][10][2] = {		{{95,70}, 
													{40,100},
													{40,70},
													{40,40},
													{58,100},
													{58,70},	
													{58,40},
													{76,100},
													{76,70},
													{76,40}},
											/***********************/
													{{140,65},
													{20,130},
													{20,65},
													{20,20},
													{60,130},
													{60,65},
													{60,20},
													{100,130},
													{100,65},
													{100,20}},
                                            /***********************/
													{{180,70},
													{0,150},
													{0,70},
													{0,0},
													{60,150},
													{60,70},
													{60,0},
													{120,150},
													{120,70},
													{120,0}}};

%}

%union { int ival; char *string; };

//Grammar tokens
%start program
%token INST_MOVE
%token INST_PIN
%token INST_PRESS
%token INST_TOUCH
%token INST_MAP
%token NUMBER
%token NEWLINE

%%

program: 
	instruction newline program			
	| NEWLINE program
	|
	;

newline: 
	NEWLINE					{line_no++;}
	;

instruction:
	instr_move 
	| instr_pin			
	| instr_press						
	| instr_touch
	| instr_map
	;

instr_touch:
	INST_TOUCH				{touch();}
	;

instr_press:
	INST_PRESS time		{
							if (t > 255)  {
								yyerror("Push time cannot exceed 255 seconds");							
							}
							else	      {
								press(t);
							}							
						}
	;

instr_move:
	INST_MOVE x_target ',' y_target 	{ move(x, y); }
	;

instr_pin:
	INST_PIN pin		{
							if(p < 100000 || p > 999999)  {
								yyerror("PIN must be 6 digits long.");
							}
							else {
								enter_pin(p);
							}
						}
	;

instr_map:
	INST_MAP x_target ',' y_target 		{
											touch();
											move(x, y);
										}
	;

time:
	NUMBER					{ t = $<ival>1; }
	;

x_target:
	NUMBER					{
								x = $<ival>1;
								if (x > 120)
								{
									yyerror("Invalid X coordinate");
									x = 0;
								}
							}
	;

y_target:
	NUMBER					{
								y = $<ival>1;
								if (y > 90)
								{
									yyerror("Invalid Y coordinate");
									y = 0;
								}
							}
	;

pin:
	NUMBER					{ p = $<ival>1; }
	;
%%

//Enters a pin automatically
void enter_pin(int pin )  {
	char pin_buffer[7];
	sprintf(pin_buffer, "%d", pin);
	for(int i = 0; i < 6; ++i) {
		int digit = pin_buffer[i] - '0';
		move_deg(keyboard[default_keyboard_size-1][digit][0], 
					keyboard[default_keyboard_size-1][digit][1]);
		touch();
		//usleep(250000);
	}
}

//Reports errors in the error log
void yyerror(const char *s)  {
	fprintf(error_log_file, "Error in line %d. Error: %s.\n", line_no, s);
	printf("Error in line %d. Error: %s.\n", line_no, s);
	exit(1);
} 

/* Parses input parameters in order to execute the config file.
	-c is used to set the config file
	-s is used to set the keyboard size (1, 2, or 4 for 1x1cm, 2x2cm and 4x4cm)
	-p is used to set the port
*/
int main(int argc, char **argv )  {
	//Hardware port
	char *port = NULL;

	//Config file descriptor
	FILE *config_file;	

	//Config file name
	char *conf_fn = NULL;

	//Standard getopt() program options parsing
	opterr = 0;
	int c;
	while ((c = getopt (argc, argv, "hc:s:p:")) != -1){
	    switch (c) {
		case 'c':
			//Set config file
			conf_fn = optarg;
			break;
		case 's':
			//Set keyboard size
			default_keyboard_size = atoi(optarg);
			break;
		case 'p':
			//Set hardware port
			port = optarg;
			break;
		case 'h':
			printf("Usage: %s -c <config_file> -p <hardware_port> [-s <keyboard_size>]\n", argv[0]);
			return;
		case '?':
			//Input error handling
			if (optopt == 'p' )
				fprintf (stderr, "-%c option requires a valid port.\n", optopt);
			else if (optopt == 's' )
			  	fprintf (stderr, "-%c option requires a valid size (1x1, 2x2, 4x4).\n", optopt);
			else if (optopt == 'c' )
			 	 fprintf (stderr, "-%c option requires a configuration file name.\n", optopt);
			else if (isprint (optopt))
			 	 fprintf (stderr, "Unknown option `-%c'.\n", optopt);
			else
			 	 fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
			exit(0);
		default:
			abort();
		}
	}

	//Finish if no config file provided
	if (conf_fn == NULL) {
		printf("No configuration file provided\n");
		return -1;
	}

	//Finish if fopen error
	config_file = fopen(conf_fn, "r");
	if (!config_file) 	{
		printf("Error opening configuration file\n");
		return -1;
	}

	//Finish if fopen errors
	if( !set_port(port) )  {
		return -1;
	}
	
	//Open the error log
	error_log_file = fopen("error_log.txt", "w");

	//Set file to be parsed	
	yyin = config_file;

	//Parse the file
	do 	{
    	yyparse();
	} 
	while ( !feof(yyin) );

	//Close open files
	fprintf(error_log_file, "No errors in file \"%s\"", conf_fn);
	fclose(error_log_file);
	fclose(config_file);

	printf("Finished program execution...\n");
	return 0;
}
