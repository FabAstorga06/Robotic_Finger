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
int target = 0;			// number target from the keyboard
int p = 0;			// PIN
int default_keyboard_size = 1;	//Keyboard size, default 1x1cm
											/*************1x1**************/
const float keyboard[3][10][3] = {		{			{37,74,73},		
													{49,93,73.5},   
													{53,88,72},
													{54,81,71.5},
													{44.5,91,72},
													{47,84,71},
													{49,77,72},	
													{39,86,72},
													{42,80,72},
													{45.5,74,73.5}
										},
											/************2x2***********/
										{			{44,66,76},
													{69,97,73},
													{71,85,72},
													{74,73,74},
													{58,89,71},
													{60,79,74},
													{65,64,76},
													{47,86,71},
													{51,74,72},
													{55,61,75}
										},
                                            /************4x4***********/
										{			{55,45,80},
													{110,90,72},
													{105,65,76},
													{100,30,88},
													{90,98,73},
													{90,65,75},
													{90,35,85},
													{65,85,72},
													{70,60,80},
													{73,30,89}
										}									};

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
	INST_TOUCH				{ touch();  }
	;

instr_press:
	INST_PRESS time		{
							if (t > 50)  {
								yyerror("Push time cannot exceed 50 seconds");							
							}
							else	      {
								press(t);
							}							
						}
	;

instr_move:
	INST_MOVE num_target  	{ move(keyboard[default_keyboard_size-1][target][0], 
											keyboard[default_keyboard_size-1][target][1],
											keyboard[default_keyboard_size-1][target][2]);   }
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
	INST_MAP num_target		{
											move(keyboard[default_keyboard_size-1][target][0], 
												keyboard[default_keyboard_size-1][target][1],
												keyboard[default_keyboard_size-1][target][2]);
											touch(); 
										}
	;

time:
	NUMBER					{ t = $<ival>1; }
	;

num_target:
	NUMBER					{
								target = $<ival>1;
								if (target > 9)
								{
									yyerror("Invalid key");
									target = 0;
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
		move(keyboard[default_keyboard_size-1][digit][0], 
					keyboard[default_keyboard_size-1][digit][1],
					keyboard[default_keyboard_size-1][digit][2]		);
		touch();
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
			if (atoi(optarg) == 4) {
				default_keyboard_size = 3;
			} else {
				default_keyboard_size = atoi(optarg);
			}
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
	error_log_file = fopen("errlog.txt", "w");

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
