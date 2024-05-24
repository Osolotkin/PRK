#include "error.h"

const char* const errorDescription[] = {
    "No error.",
    "Malloc had a bad day.",
   
    "Unsupported lvalue token! Variable name or number expected!",
    "Unsupported operator!",
    "Invalid token!",
    "Invalid variable name!",
    "Undefined variable!",
    "Expected defined symbol! Use any assignment operator to define '%.*s'."
};
