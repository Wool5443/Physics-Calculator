#include <ctype.h>
#include <string.h>
#include "DSL.hpp"
#include "RecursiveDescent.hpp"
#include "String.hpp"

#define SyntaxAssert(expression, ...)                                   \
do                                                                      \
{                                                                       \
    if (!(expression))                                                  \
    {                                                                   \
        SetConsoleColor(stdout, COLOR_RED);                             \
        fprintf(stdout, "SYNTAX ERROR AT %s\n", CUR_CHAR_PTR);          \
        SetConsoleColor(stdout, COLOR_WHITE);                           \
        __VA_ARGS__;                                                    \
        return ERROR_SYNTAX;                                            \
    }                                                                   \
} while (0)

#define SyntaxAssertResult(expression, poison, ...)                     \
do                                                                      \
{                                                                       \
    if (!(expression))                                                  \
    {                                                                   \
        SetConsoleColor(stdout, COLOR_RED);                             \
        fprintf(stdout, "SYNTAX ERROR AT %s\n", CUR_CHAR_PTR);          \
        SetConsoleColor(stdout, COLOR_WHITE);                           \
        __VA_ARGS__;                                                    \
        return { poison, ERROR_SYNTAX };                                \
    }                                                                   \
} while (0)

#define CUR_CHAR_PTR (*context)

#define SKIP_ALPHA()                                                    \
do                                                                      \
{                                                                       \
    while (isalpha(*CUR_CHAR_PTR))                                      \
        CUR_CHAR_PTR++;                                                 \
} while (0)

// x = 56 + 45 ^ 2
// 56 + 45 ^ 2

// 56 + (23 + 5 * 8) ^ (45 - 3) ^ 3
// x

// G        -> S '\0'
// S        -> {E | Symbol = E} '\0'
// E        -> T {['+', '-']T}*
// T        -> D {['*', '/']D}*
// D        -> P {'^'D}*
// P        -> '(' E ')' | Id
// Symbol   -> Name
// Name     -> ALPHABET+ {DIGITS u ALPHABET}*
// Id       -> Symbol | N
// N        -> DIGITS+

TreeNodeResult _getS     (const char** context);
TreeNodeResult _getE     (const char** context);
TreeNodeResult _getT     (const char** context);
TreeNodeResult _getD     (const char** context);
TreeNodeResult _getP     (const char** context);
TreeNodeResult _getSymbol(const char** context);
TreeNodeResult _getName  (const char** context);
TreeNodeResult _getId    (const char** context);
TreeNodeResult _getN     (const char** context);

ErrorCode ParseExpression(Tree& tree, String& string)
{
    const char* buf = string.buf;
    const char** context = &buf;

    TreeNodeResult root = _getS(context);

    RETURN_ERROR(root.error);

    SyntaxAssert(*CUR_CHAR_PTR == '\0');

    RETURN_ERROR(tree.Init(root.value));

    return EVERYTHING_FINE;
}

TreeNodeResult _getS(const char** context)
{
    MyAssertSoftResult(context, nullptr, ERROR_NULLPTR);

    const char* assignmentCharPtr = strchr(CUR_CHAR_PTR, '=');

    TreeNode* result = nullptr;

    if (assignmentCharPtr)
    {
        CREATE_NODE_SAFE(symbol, _getSymbol(context), result->Delete());

        SyntaxAssertResult(*CUR_CHAR_PTR == '=', nullptr, result->Delete());
        CUR_CHAR_PTR++;

        CREATE_NODE_SAFE(expression, _getE(context), result->Delete(); symbol->Delete());

        CREATE_NODE_SAFE(_res, TreeNode::New({}, symbol, expression), symbol->Delete();
                                                                      expression->Delete());
        result = _res;
        NODE_TYPE(result)      = OPERATION_TYPE;
        NODE_OPERATION(result) = ASSIGN_OPERATION;
        NODE_PRIORITY(result)  = ASSIGN_OPERATION_PRIORITY;
    }
    else
    {
        CREATE_NODE_SAFE(_res, _getE(context));
        result = _res;
    }

    SyntaxAssertResult(*CUR_CHAR_PTR == '\0', nullptr);

    return { result, EVERYTHING_FINE };
}

TreeNodeResult _getE(const char** context)
{
    MyAssertSoftResult(context, nullptr, ERROR_NULLPTR);

    CREATE_NODE_SAFE(result, _getT(context));

    while (*CUR_CHAR_PTR == '+' || *CUR_CHAR_PTR == '-')
    {
        Operation op = *CUR_CHAR_PTR == '+' ? ADD_OPERATION : SUB_OPERATION;

        CUR_CHAR_PTR++;

        CREATE_NODE_SAFE(nextT, _getT(context), result->Delete());

        CREATE_NODE_SAFE(resCopy, TreeNode::New(result->value, result->left, result->right),
            result->Delete(), nextT->Delete());

        ErrorCode err = result->SetLeft(resCopy);
        if (err)
        {
            result->Delete();
            nextT->Delete();
            resCopy->Delete();

            return { nullptr, err };
        }

        err = result->SetRight(nextT);
        if (err)
        {
            result->Delete();
            nextT->Delete();
            resCopy->Delete();

            return { nullptr, err };
        }

        NODE_TYPE(result)      = OPERATION_TYPE;
        NODE_OPERATION(result) = op;
        NODE_PRIORITY(result)  = ADD_OPERATION_PRIORITY;
    }

    return { result, EVERYTHING_FINE };
}

TreeNodeResult _getT(const char** context)
{
    MyAssertSoftResult(context, nullptr, ERROR_NULLPTR);

    CREATE_NODE_SAFE(result, _getD(context));

    while (*CUR_CHAR_PTR == '*' || *CUR_CHAR_PTR == '/')
    {
        Operation op = *CUR_CHAR_PTR == '*' ? MUL_OPERATION : DIV_OPERATION;

        CUR_CHAR_PTR++;

        CREATE_NODE_SAFE(nextD, _getD(context), result->Delete());

        CREATE_NODE_SAFE(resCopy, TreeNode::New(result->value, result->left, result->right),
            result->Delete(), nextD->Delete());

        ErrorCode err = result->SetLeft(resCopy);
        if (err)
        {
            result->Delete();
            nextD->Delete();
            resCopy->Delete();

            return { nullptr, err };
        }

        err = result->SetRight(nextD);
        if (err)
        {
            result->Delete();
            nextD->Delete();
            resCopy->Delete();

            return { nullptr, err };
        }

        NODE_TYPE(result)      = OPERATION_TYPE;
        NODE_OPERATION(result) = op;
        NODE_PRIORITY(result)  = MUL_OPERATION_PRIORITY;
    }

    return { result, EVERYTHING_FINE };
}

TreeNodeResult _getD(const char** context)
{
    MyAssertSoftResult(context, nullptr, ERROR_NULLPTR);

    CREATE_NODE_SAFE(result, _getP(context));

    while (*CUR_CHAR_PTR == '^')
    {
        CUR_CHAR_PTR++;

        CREATE_NODE_SAFE(nextD, _getD(context), result->Delete());

        CREATE_NODE_SAFE(resCopy, TreeNode::New(result->value, result->left, result->right),
            result->Delete(), nextD->Delete());

        ErrorCode err = result->SetLeft(resCopy);
        if (err)
        {
            result->Delete();
            nextD->Delete();
            resCopy->Delete();

            return { nullptr, err };
        }

        err = result->SetRight(nextD);
        if (err)
        {
            result->Delete();
            nextD->Delete();
            resCopy->Delete();

            return { nullptr, err };
        }

        NODE_TYPE(result)      = OPERATION_TYPE;
        NODE_OPERATION(result) = POWER_OPERATION;
        NODE_PRIORITY(result)  = POWER_OPERATION_PRIORITY;
    }

    return { result, EVERYTHING_FINE };
}

TreeNodeResult _getP(const char** context)
{
    MyAssertSoftResult(context, nullptr, ERROR_NULLPTR);
    if (*CUR_CHAR_PTR == '(')
    {
        CUR_CHAR_PTR++;

        CREATE_NODE_SAFE(result, _getE(context));

        SyntaxAssertResult(*CUR_CHAR_PTR == ')', nullptr, result->Delete());
        CUR_CHAR_PTR++;

        return { result, EVERYTHING_FINE };
    }

    return _getId(context);
}

TreeNodeResult _getSymbol(const char** context)
{
    MyAssertSoftResult(context, nullptr, ERROR_NULLPTR);

    return _getName(context);
}

TreeNodeResult _getName(const char** context)
{
    MyAssertSoftResult(context, nullptr, ERROR_NULLPTR);

    SyntaxAssertResult(isalpha(*CUR_CHAR_PTR), nullptr);

    String name = {};
    name.Create();

    name.Append(*(CUR_CHAR_PTR++));

    while (isalnum(*CUR_CHAR_PTR) || *CUR_CHAR_PTR == '_')
    {
        name.Append(*(CUR_CHAR_PTR++));
    }

    CREATE_NODE_SAFE(result, TreeNode::New({}));
    NODE_TYPE(result) = NAME_TYPE;
    NODE_NAME(result) = name;

    return { result, EVERYTHING_FINE };
}

TreeNodeResult _getId(const char** context)
{
    MyAssertSoftResult(context, nullptr, ERROR_NULLPTR);

    if (isalpha(*CUR_CHAR_PTR))
        return _getName(context);

    return _getN(context);
}

TreeNodeResult _getN(const char** context)
{
    MyAssertSoftResult(context, nullptr, ERROR_NULLPTR);

    char* endPtr = nullptr;

    double val = strtod(CUR_CHAR_PTR, &endPtr);
    SyntaxAssertResult(CUR_CHAR_PTR != endPtr, nullptr);

    CUR_CHAR_PTR = endPtr;

    CREATE_NODE_SAFE(result, TreeNode::New({}));

    NODE_TYPE(result)   = NUMBER_TYPE;
    NODE_NUMBER(result) = val;

    return { result, EVERYTHING_FINE };
}
