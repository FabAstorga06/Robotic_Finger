%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rf.tab.h"

// Regex for decimal numbers
%}

%%
	//Language keywords & return value, defined in y.tab.h

("MOVE"|"move")			{ return INST_MOVE;  }
("PIN"|"pin")			{ return INST_PIN;   }
("PRESS"|"press")		{ return INST_PRESS; }
("TOUCH"|"touch")		{ return INST_TOUCH; }
("MAP"|"map")			{ return INST_MAP;   }


[ \t]					//Ignore tabs and whitespace
[\n]					{ return NEWLINE;    }		
([0-9])+				{ yylval.ival = atoi(yytext); return NUMBER; }
.						{ return *yytext; }		

%%

//Called once yylex() reaches the end of the file
int yywrap()   {
	return 1;
}
