#define ERR_DESCRIPTION(err) (errorDescription[-err])

enum Error {
    ERR_OK      = 0,
    ERR_MALLOC  = -1,

    ERR_INVALID_LVALUE          = -2,
    ERR_INVALID_OPERATOR_USED   = -3,
    ERR_INVALID_TOKEN           = -4,
    ERR_INVALID_VARIABLE_NAME   = -5,
    ERR_UNDEFINED_VARIABLE      = -6,
    ERR_EXPECTED_DEFINED_SYMBOL = -7,
};

extern const char* const errorDescription[];
