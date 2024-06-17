#include <ctype.h>
#include <string.h>
#include "RecursiveDescent.hpp"
#include "String.hpp"

#define SyntaxAssert(expression, ...)                                   \
do                                                                      \
{                                                                       \
    if (!(expression))                                                  \
    {                                                                   \
        SetConsoleColor(stdout, COLOR_RED);                \
        fprintf(stdout, "SYNTAX ERROR AT %s\n", CUR_CHAR_PTR);          \
        SetConsoleColor(stdout, COLOR_WHITE);              \
        __VA_ARGS__;                                                    \
        return ERROR_SYNTAX;                                            \
    }                                                                   \
} while (0)

#define SyntaxAssertResult(expression, poison, ...)                     \
do                                                                      \
{                                                                       \
    if (!(expression))                                                  \
    {                                                                   \
        SetConsoleColor(stdout, COLOR_RED);                \
        fprintf(stdout, "SYNTAX ERROR AT %s\n", CUR_CHAR_PTR);          \
        SetConsoleColor(stdout, COLOR_WHITE);              \
        __VA_ARGS__;                                                    \
        return { poison, ERROR_SYNTAX };                                \
    }                                                                   \
} while (0)

#define SKIP_ALPHA()                                                    \
do                                                                      \
{                                                                       \
    while (isalpha(*CUR_CHAR_PTR))                                      \
        CUR_CHAR_PTR++;                                                 \
} while (0)

#define CUR_CHAR_PTR (*context)

TreeNodeResult _getE(const char** context);
