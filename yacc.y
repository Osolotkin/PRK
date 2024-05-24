%{
    #include <stdio.h>
    #include <stdlib.h>
    #include "./src/code.h"

    #define ERROR { printf("\n"); YYERROR; }

    int yylex(void);
    int yyerror(const char* str);
    
    // dont know what it does or what its for, seems unnecessary,
    // but at this point i am scared to delete it
    struct yy_buffer_state* get_buffer();
%}

%union { 
 String* str;
 Statement* stmt;
 Expression* exp;
 Variable* var;
 Operator op;
};

%type <stmt> Statement
%type <exp> Expression Expression1 Expression2 UnaryExpression
%type <var> LValue Identifier
%type <op> AssignmentOperator BinaryOperator1 BinaryOperator2 UnaryOperator

%start Syntax
%token 
    TT_IDENTIFIER 
    TT_NUMBER_LITERAL 
    TT_STATEMENT_END 
    TT_PLUS 
    TT_MINUS TT_STAR 
    TT_SLASH 
    TT_EQUAL
    TT_MACRO
    TT_ASSIGNMENT_PLUS
    TT_ASSIGNMENT_MINUS
    TT_ASSIGNMENT_STAR
    TT_ASSIGNMENT_SLASH
    TT_ASSIGNMENT_EQUAL
    TT_LEFT_BRACKET
    TT_RIGHT_BRACKET
    TT_QUESTION_MARK
    TT_EOF
    TT_INVALID

%%

Syntax              : Statements { exit(0); };

Statements          : { loc.line++; printf("%i> ", loc.line); }
                    | Statements TT_STATEMENT_END { loc.line++; printf("%i> ", loc.line); }
                    | Statements Statement { loc.line++; exec($2); printf("%i> ", loc.line); } 
                    | Statements error
                    ;

Statement           : LValue TT_STATEMENT_END                   { newCall($1, 0, &$$); }
                    | LValue TT_QUESTION_MARK TT_STATEMENT_END  { newCall($1, 1, &$$); }
                    | LValue AssignmentOperator Expression TT_STATEMENT_END                     { newAssignment($1, $2, $3, 0, &$$); }
                    | LValue AssignmentOperator Expression TT_QUESTION_MARK TT_STATEMENT_END    { newAssignment($1, $2, $3, 1, &$$); } 
                    ;

LValue              : TT_IDENTIFIER { if (newVariable(&loc, *yylval.str, VT_VARIABLE, 1, &$$)) ERROR; }
                    | TT_NUMBER_LITERAL { if (newVariable(&loc, *yylval.str, VT_NUMBER, 1, &$$)) ERROR; }
                    ;

Identifier          : TT_IDENTIFIER { if (newVariable(&loc, *yylval.str, VT_VARIABLE, 0, &$$)) ERROR; }
                    | TT_NUMBER_LITERAL { if (newVariable(&loc, *yylval.str, VT_NUMBER, 0, &$$)) ERROR; }
                    ;


AssignmentOperator  : TT_EQUAL                  { $$ = OP_EQUAL; }
                    | TT_ASSIGNMENT_EQUAL       { $$ = OP_ASSIGNMENT_EQUAL; }
                    | TT_ASSIGNMENT_PLUS        { $$ = OP_ASSIGNMENT_EQUAL_PLUS; } 
                    | TT_ASSIGNMENT_MINUS       { $$ = OP_ASSIGNMENT_EQUAL_MINUS; }
                    | TT_ASSIGNMENT_STAR        { $$ = OP_ASSIGNMENT_EQUAL_STAR; }
                    | TT_ASSIGNMENT_SLASH       { $$ = OP_ASSIGNMENT_EQUAL_SLASH; }
                    ;

BinaryOperator1     : TT_MINUS  { $$ = OP_SUBTRACTION; }
                    | TT_PLUS   { $$ = OP_ADDITION; }
                    ;

BinaryOperator2     : TT_STAR   { $$ = OP_MULTIPLICATION; }
                    | TT_SLASH  { $$ = OP_DIVISION; }
                    ;

UnaryOperator       : TT_MINUS  { $$ = OP_SUBTRACTION; } 
                    | TT_PLUS   { $$ = OP_ADDITION; }
                    ;

Expression          : Expression1 
                    ;

Expression1         : Expression2
                    | Expression2 BinaryOperator1 Expression1 { newBinaryExpression($1, $3, $2, &$$); }
                    ;

Expression2         : UnaryExpression
                    | UnaryExpression BinaryOperator2 Expression2 { newBinaryExpression($1, $3, $2, &$$); }
                    
                    ;

UnaryExpression     : TT_LEFT_BRACKET Expression TT_RIGHT_BRACKET { $$ = $2; }
                    | Identifier                    { newTerminalExpression($1, &$$); } 
                    | UnaryOperator UnaryExpression { newUnaryExpression($2, $1, &$$); }
                    ;

%%

int yyerror(const char* str) {

    const int tmpIdx = loc.idx;
    loc.idx = lastTokenIdx;
    my_log(LL_ERROR, str, &loc, lastTokenLen);
    loc.idx = tmpIdx;

    printf("\n");

}

int main() {
   
    // maybe interpreterInit is better place for them
    lastTokenLen = 0;
    lastTokenIdx = 0;
    loc.idx = 0;
    loc.line = 0;

    interpreterInit();
    
    printf("Welcome to the command-line interface!\n");   

    yyparse();

    return 0;

}
