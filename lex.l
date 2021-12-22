%{

#include <string.h>

%}

%option nounput
%option noinput
%option noyywrap

word_valid		[^" ""("")""|"";""&&"">""<""2>"]
%%

"("			{ return '('; }
")"             	{ return ')'; }
"|"             	{ return PIPE; }
";"             	{ return SEQ; }
"&&"            	{ return AND; }
"||"            	{ return OR; }
">>"            	{ return APPEND; }
">"             	{ return OUTPUT; }
"<"             	{ return INPUT; }
"2>"            	{ return ERROR; }


(\"[^"]*\")|(\'[^']*\') {
			    yylval.string = strndup(yytext + 1, strlen(yytext + 2));
			    return ARG;
			}
#.*			;
{word_valid}*		{
			    yylval.string = strdup(yytext);
			    return ARG;
			}
.			; // if not added, unmatched characters will end up being printed in the buffer

