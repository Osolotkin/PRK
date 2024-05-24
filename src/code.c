#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "code.h"
#include "hash_map.h"
#include "error.h"

#define WHITE_ESC "\033[1;37m"
#define YELLOW_ESC "\033[1;33m"
#define RED_ESC "\033[1;31m"
#define COLOR_RESET_ESC "\033[0m"



int lastTokenIdx;
int lastTokenLen;

Location loc;
HashMap* defs;



int countDigits(int x) {
    
    int cnt = 1;
    while (x /= 10) cnt++;
    
    return cnt;

}

void printUnderline(Location* loc, const int len, const char* color) {

    int i = 0;
    const int underlineStart = countDigits(loc->line) + loc->idx + 2 - len;
    const int underlineEnd = underlineStart + len;
    for (; i < underlineStart; i++) putchar(' ');
    printf(color);
    for (; i < underlineEnd; i++) putchar ('^');
    printf(COLOR_RESET_ESC);
    putchar('\n');

}

// there is no case of camels in this room, as there are no candies for 
// the guy who stole log definition
void my_log(const int level, const char* message, Location* loc, const int len, ...) {
    
    switch (level) {

        case LL_INFO : {
            printUnderline(loc, len, WHITE_ESC);
            printf("INFO : ");
            break;
        }

        case LL_WARNING : {
            printUnderline(loc, len, YELLOW_ESC);
            printf("WARNING : ");
            break;
        }

        case LL_ERROR : {
            printUnderline(loc, len, RED_ESC);
            printf(RED_ESC "ERROR" COLOR_RESET_ESC);
            printf("(%i, %i) : ", loc->line, loc->idx - len + 1);
            break;
        }

        default :
            break;
        
    }

    va_list args;
    va_start(args, len);
    vprintf(message, args);
    va_end (args);
    putchar('\n');

}

void my_logLite(const int level, const char* message) {

    switch (level) {

        case LL_INFO : {
            printf(WHITE_ESC "INFO" COLOR_RESET_ESC);
            printf(" : %s", message);
            break;
        }

        case LL_WARNING : {
            printf(YELLOW_ESC "WARNING" COLOR_RESET_ESC);
            printf(" : %s", message);
            break;
        }

        case LL_ERROR : {
            printf(RED_ESC "ERROR" COLOR_RESET_ESC);
            printf(" : %s", message);
            break;
        }

        default :
            break;
        
    }

}

// pass only valid strings ;)
int str2int(String str) {

    int value = 0; // TODO -> int64_t
    int tmp = 1;
    for (int i = 0; i < str.len; i++) {
        value += (str.buff[str.len - i - 1] - '0') * tmp;
        tmp *= 10;
    }

    return value;

}



int interpreterInit() {

    // it would be nice to be clever enough to know the optimal size
    // but for now and ever random magic number will stick with us
    return hashMapInit(&defs, 1024);

}



// answer - if true answer will be printed
int newCall(Variable* lvalue, int answer, Statement** outStmt) {

    Statement* stmt = malloc(sizeof(Statement));
    if (!stmt) {
        my_logLite(LL_ERROR, ERR_DESCRIPTION(ERR_MALLOC));
        return ERR_MALLOC;
    }

    stmt->call = malloc(sizeof(Call));
    stmt->call->lvalue = lvalue;
    stmt->endType = answer;
    stmt->node.type = NT_CALL;

    *outStmt = stmt;

    return 0;

}

// answer - if true answer will be printed
int newAssignment(Variable* lvalue, Operator asgmtOp, Expression* exp, int answer, Statement** outStmt) {
    
    Statement* stmt = malloc(sizeof(Statement));
    if (!stmt) {
        my_logLite(LL_ERROR, ERR_DESCRIPTION(ERR_MALLOC));
        return ERR_MALLOC;
    }

    stmt->asgmt = malloc(sizeof(Call));
    stmt->asgmt->lvalue = lvalue;
    stmt->asgmt->rvalue = exp;
    stmt->asgmt->op = asgmtOp;
    stmt->endType = answer;
    stmt->node.type = NT_ASSIGNMENT;

    *outStmt = stmt;

    return 0;

}

// lvalue - bool that specifies if variable is lvalue or not
int newVariable(Location* loc, String name, VariableType type, int lvalue, Variable** outVar) {

    Variable* var = malloc(sizeof(Variable));
    if (!var) {
        my_log(LL_ERROR, ERR_DESCRIPTION(ERR_MALLOC), loc, 0);
        return ERR_MALLOC;
    }

    VariableDefinition* def = NULL;
    hashMapAccess(defs, name.buff, name.len, (void**) &def);

    if (!def)
    switch (type) {

        case VT_NUMBER: {

            def = malloc(sizeof(VariableDefinition));
            if (!def) exit(1);

            def->value = str2int(name);
            def->exp = NULL;

            break;

        }

        case VT_VARIABLE: {

            if (!lvalue) {
                my_log(LL_ERROR, ERR_DESCRIPTION(ERR_UNDEFINED_VARIABLE), loc, name.len);
                return ERR_UNDEFINED_VARIABLE;
            }

            def = malloc(sizeof(VariableDefinition));
            if (!def) exit(1);

            def->value = 0;
            def->exp = NULL;

            break;

        }

        default: {

            if (lvalue) {
                my_log(LL_ERROR, ERR_DESCRIPTION(ERR_INVALID_LVALUE), loc, name.len);
                return ERR_INVALID_LVALUE; 
            } else {
                my_log(LL_ERROR, ERR_DESCRIPTION(ERR_INVALID_TOKEN), loc, name.len);
                return ERR_INVALID_TOKEN;
            }
        
        }

    };

    hashMapInsert(defs, name.buff, name.len, def);

    def->name = name;
    def->node.type = NT_VARIABLE_DEFINITION;

    var->def = def;
    var->node.type = NT_VARIABLE;

    *outVar = var;

    return 0;

}

int newBinaryExpression(Expression* a, Expression* b, Operator op, Expression** outExp) {
    
    Expression* exp = malloc(sizeof(Expression));
    if (!exp) {
        my_logLite(LL_ERROR, ERR_DESCRIPTION(ERR_MALLOC));
        return ERR_MALLOC;
    }

    exp->bex = malloc(sizeof(BinaryExpression));
    if (!exp->bex) {
        my_logLite(LL_ERROR, ERR_DESCRIPTION(ERR_MALLOC));
        return ERR_MALLOC;
    }

    exp->bex->op = op;
    exp->bex->a = a;
    exp->bex->b = b;
    exp->node.type = NT_BINARY_EXPRESSION;

    *outExp = exp;

    return 0;

}

int newUnaryExpression(Expression* a, Operator op, Expression** outExp) {
    
    Expression* exp = malloc(sizeof(Expression));
    if (!exp) {
        my_logLite(LL_ERROR, ERR_DESCRIPTION(ERR_MALLOC));
        return ERR_MALLOC;
    }

    exp->uex->a = a;
    exp->uex->op = op;
    exp->node.type = NT_UNARY_EXPRESSION;

    *outExp = exp;

    return 0;

}

int newTerminalExpression(Variable* var, Expression** outExp) {

    Expression* exp = malloc(sizeof(Expression));
    if (!exp) {
        my_logLite(LL_ERROR, ERR_DESCRIPTION(ERR_MALLOC));
        return ERR_MALLOC;
    }

    exp->var = var;
    exp->node.type = NT_TERMINAL_EXPRESSION;

    *outExp = exp;

    return 0;

}



void printOperator(Operator op, FILE* file) {
    
    switch (op) {
        
        case OP_IDENTITY:
            // fprintf(stdout, "");
            break;

        case OP_UNARY_PLUS:
        case OP_ADDITION:
            fputc('+', file);
            break;

        case OP_UNARY_MINUS:
        case OP_SUBTRACTION:
            fputc('-', file);
            break;

        case OP_MULTIPLICATION:
            fputc('*', file);
            break;

        case OP_DIVISION:
            fputc('/', file);
            break;

        case OP_EQUAL:
            fputc('=', file);
            break;

        case OP_ASSIGNMENT_EQUAL:
            fputc(':', file);
            fputc('=', file);
            break;

    }

}

void printExpression(Expression* exp, FILE* file) {

    fputc('(', file);

    switch (exp->node.type) {

        case NT_UNARY_EXPRESSION: {

            UnaryExpression* const uex = exp->uex;

            printOperator(uex->op, file);
            printExpression(uex->a, file);

            break;

        }

        case NT_BINARY_EXPRESSION: {

            BinaryExpression* const bex = exp->bex;

            printExpression(bex->a, file);
            printOperator(bex->op, file);
            printExpression(bex->b, file);

            
            break;

        }

        case NT_TERMINAL_EXPRESSION: {
            
            Variable* const var = exp->var;
            
            if (var->def->exp) printExpression(var->def->exp, file);
            else fprintf(file, "%.*s", var->def->name.len, var->def->name.buff);

        }

    }

    fputc(')', file);

}

void print(Statement* st, FILE* file) {
        
    switch (st->node.type) {

        case NT_ASSIGNMENT: {
            
            Assignment* asgmt = st->asgmt;
            switch (asgmt->op) {
                case OP_ASSIGNMENT_EQUAL:
                case OP_ASSIGNMENT_EQUAL_PLUS:
                case OP_ASSIGNMENT_EQUAL_MINUS:
                case OP_ASSIGNMENT_EQUAL_STAR:
                case OP_ASSIGNMENT_EQUAL_SLASH:
                    fprintf(file, "%i", asgmt->lvalue->def->value);
                    break;
                case OP_EQUAL:
                    printExpression(asgmt->rvalue, file);
                    break;
            }

            break;

        }

        case NT_CALL: {

            Call* call = st->call;
            if (call->lvalue->def->exp) {
                printExpression(call->lvalue->def->exp, stdout);
            } else {
                fprintf(stdout, "%i", call->lvalue->def->value);
            }

            break;

        }


    }

}

// evaluate expression
int eval(Expression* exp) {

    switch (exp->node.type) {
        
        case NT_UNARY_EXPRESSION: {

            UnaryExpression* const uex = exp->uex;

            const int type = uex->a->node.type;
            const int ans = eval(uex->a);// type == NT_TERMINAL_EXPRESSION ? uex->a->var->def->value : eval(uex->a);
            
            switch (uex->op) {
                case OP_IDENTITY:
                case OP_UNARY_PLUS: return ans;
                case OP_UNARY_MINUS: return -ans;
            }

        }
        
        case NT_BINARY_EXPRESSION: {

            BinaryExpression* const bex = exp->bex;

            const int typeA = bex->a->node.type;
            const int ansA = eval(bex->a); // typeA == NT_TERMINAL_EXPRESSION ? bex->a->var->def->value : eval(bex->a);
            
            const int typeB = bex->b->node.type;
            const int ansB = eval(bex->b);//typeB == NT_TERMINAL_EXPRESSION ? bex->b->var->def->value : eval(bex->b);
            
            switch (exp->bex->op) {
                case OP_ADDITION:       return ansA + ansB;
                case OP_SUBTRACTION:    return ansA - ansB;
                case OP_MULTIPLICATION: return ansA * ansB;
                case OP_DIVISION:       return ansA / ansB;
            }
                         
        }
        
        case NT_TERMINAL_EXPRESSION: {

            if (exp->var->def->exp) return eval(exp->var->def->exp);
            return exp->var->def->value;

        }

    }

}

// execute statement
int exec(Statement* stmt) {

    switch (stmt->node.type) {

        case NT_ASSIGNMENT: {

            Assignment* asgmt = stmt->asgmt;
            switch (asgmt->op) {
                case OP_EQUAL: {
                    asgmt->lvalue->def->exp = asgmt->rvalue;
                    break;
                }

                case OP_ASSIGNMENT_EQUAL: {
                    asgmt->lvalue->def->value = eval(asgmt->rvalue);
                    asgmt->lvalue->def->exp = NULL;
                    break;
                }

                case OP_ASSIGNMENT_EQUAL_PLUS: {
                    asgmt->lvalue->def->value += eval(asgmt->rvalue);
                    asgmt->lvalue->def->exp = NULL;
                    break;
                }

                case OP_ASSIGNMENT_EQUAL_MINUS: {
                    asgmt->lvalue->def->value -= eval(asgmt->rvalue);
                    asgmt->lvalue->def->exp = NULL;
                    break; 
                }

                case OP_ASSIGNMENT_EQUAL_STAR: {
                    asgmt->lvalue->def->value *= eval(asgmt->rvalue);
                    asgmt->lvalue->def->exp = NULL;
                    break;
                }

                case OP_ASSIGNMENT_EQUAL_SLASH: {
                    asgmt->lvalue->def->value /= eval(asgmt->rvalue);
                    asgmt->lvalue->def->exp = NULL;
                    break;
                }

            }

            break;

        }

        case NT_CALL: {


        }

    }

    if (!stmt->endType) return 0;
    print(stmt, stdout);
    fputc('\n', stdout);

}
