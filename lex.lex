%{
    #include <string.h>
    #include "./src/code.h"
    #include "./src/error.h"
    #include "y.tab.h"
    
    String* newString(char* str, int len);

%}

%%
    /*  maybe there is a function / macro or whatever to do same stuff for each token, 
        but i have enough of digging this hole. */

[A-Za-z]+       { yylval.str = newString(yytext, yyleng); loc.idx += yyleng; lastTokenIdx = loc.idx; lastTokenLen = yyleng; return TT_IDENTIFIER; }
0|[1-9][0-9]*   { yylval.str = newString(yytext, yyleng); loc.idx += yyleng; lastTokenIdx = loc.idx; lastTokenLen = yyleng; return TT_NUMBER_LITERAL; }
"+"             { loc.idx += yyleng; lastTokenIdx = loc.idx; lastTokenLen = yyleng; return TT_PLUS; }
"-"             { loc.idx += yyleng; lastTokenIdx = loc.idx; lastTokenLen = yyleng; return TT_MINUS; }
"*"             { loc.idx += yyleng; lastTokenIdx = loc.idx; lastTokenLen = yyleng; return TT_STAR; }
"/"             { loc.idx += yyleng; lastTokenIdx = loc.idx; lastTokenLen = yyleng; return TT_SLASH; }
"="             { loc.idx += yyleng; lastTokenIdx = loc.idx; lastTokenLen = yyleng; return TT_EQUAL; }
":+"            { loc.idx += yyleng; lastTokenIdx = loc.idx; lastTokenLen = yyleng; return TT_ASSIGNMENT_PLUS; }
":-"            { loc.idx += yyleng; lastTokenIdx = loc.idx; lastTokenLen = yyleng; return TT_ASSIGNMENT_MINUS; }
":*"            { loc.idx += yyleng; lastTokenIdx = loc.idx; lastTokenLen = yyleng; return TT_ASSIGNMENT_STAR; }
":/"            { loc.idx += yyleng; lastTokenIdx = loc.idx; lastTokenLen = yyleng; return TT_ASSIGNMENT_SLASH; }
":="            { loc.idx += yyleng; lastTokenIdx = loc.idx; lastTokenLen = yyleng; return TT_ASSIGNMENT_EQUAL; }
"?"             { loc.idx += yyleng; lastTokenIdx = loc.idx; lastTokenLen = yyleng; return TT_QUESTION_MARK; }
"("             { loc.idx += yyleng; lastTokenIdx = loc.idx; lastTokenLen = yyleng; return TT_LEFT_BRACKET; }
")"             { loc.idx += yyleng; lastTokenIdx = loc.idx; lastTokenLen = yyleng; return TT_RIGHT_BRACKET; }
"\n"            { lastTokenIdx = loc.idx; loc.idx = 0; lastTokenLen = yyleng; return TT_STATEMENT_END; }

[ \t]           { loc.idx += yyleng; lastTokenIdx = loc.idx; lastTokenLen = yyleng; }
.               { lastTokenIdx = loc.idx; lastTokenLen = yyleng; return *yytext; }

%%

String* newString(char* istr, int len) {
    
    String* ostr = malloc(sizeof(String));
    if (!ostr) {
        my_logLite(LL_ERROR, ERR_DESCRIPTION(ERR_MALLOC));
        exit(1);
    }

    ostr->buff = malloc(sizeof(char) * len);
    if (!ostr->buff) {
        my_logLite(LL_ERROR, ERR_DESCRIPTION(ERR_MALLOC));
        exit(1);
    }

    memcpy(ostr->buff, istr, len);
    ostr->len = len;

}

int yywrap() {
    return 1;
}

