%{

#include <string.h>

%}

%option nounput
%option noinput
%option noyywrap

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
[^\ ]*			{
			    yylval.string = strdup(yytext);
			    return ARG;
			}

