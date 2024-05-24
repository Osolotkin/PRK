#include "my_string.h"

typedef enum LogLevel {

    LL_INFO     = 0x1,
    LL_WARNING  = 0x2,
    LL_ERROR    = 0x3

} LogLevel;

typedef enum NodeType {
    
    NT_VARIABLE,
    NT_VARIABLE_DEFINITION,
    NT_UNARY_EXPRESSION,
    NT_BINARY_EXPRESSION,
    NT_TERMINAL_EXPRESSION,
    NT_ASSIGNMENT,
    NT_CALL,

} NodeType;

typedef enum VariableType {

    VT_VARIABLE,
    VT_NUMBER,
    VT_OPERATOR,

} VariableType;

typedef enum ExpressionType {

    EXT_UNARY,
    EXT_BINARY,
    EXT_TERMINAL

} ExpressionType;

typedef enum Operator {
    
    OP_IDENTITY,
    OP_UNARY_PLUS,
    OP_UNARY_MINUS,
    OP_ADDITION,
    OP_SUBTRACTION,
    OP_MULTIPLICATION,
    OP_DIVISION,
    OP_EQUAL,
    OP_ASSIGNMENT_EQUAL,
    OP_ASSIGNMENT_EQUAL_PLUS,
    OP_ASSIGNMENT_EQUAL_MINUS,
    OP_ASSIGNMENT_EQUAL_STAR,
    OP_ASSIGNMENT_EQUAL_SLASH

} Operator;



typedef struct Location {
    int line;
    int idx;
} Location;



typedef struct Variable Variable;
typedef struct Expression Expression;

typedef struct SyntaxNode {
    
    NodeType type;
    Location loc;

} SyntaxNode;



typedef struct UnaryExpression {

    Expression* a;
    Operator op;

} UnaryExpression;

typedef struct BinaryExpression {

    Expression* a;
    Expression* b;
    Operator op;

} BinaryExpression;

typedef struct Expression {
    
    SyntaxNode node;

    union {
        UnaryExpression* uex;
        BinaryExpression* bex;
        Variable* var;
    };

} Expression;



typedef struct VariableDefinition {

    SyntaxNode node;
    
    String name;
    Expression* exp;
    int value;

} VariableDefinition;

struct Variable {

    SyntaxNode node;
    VariableDefinition* def;

};



typedef struct Assignment {

    Variable* lvalue;
    Expression* rvalue;
    Operator op;

} Assignment;

typedef struct Call {
    
    Variable* lvalue;

} Call;

typedef struct Statement {
    
    SyntaxNode node;
    
    union {
        Assignment* asgmt;
        Call* call;
    };

    int endType;

} Statement;



int interpreterInit();

int newCall(Variable* lvalue, int answer, Statement** outStmt);
int newAssignment(Variable* lvalue, Operator asgmtOp, Expression* exp, int answer, Statement** outStmt);
int newVariable(Location* loc, String name, VariableType type, int lvalue, Variable** outVar);
int newBinaryExpression(Expression* a, Expression* b, Operator op, Expression** outExp);
int newUnaryExpression(Expression* a, Operator op, Expression** outExp);
int newTerminalExpression(Variable* var, Expression** outExp);

int exec(Statement* stmt);

void my_log(const int level, const char* message, Location* loc, const int len, ...);
void my_logLite(const int level, const char* message);


extern Location loc;
extern int lastTokenIdx;
extern int lastTokenLen;
