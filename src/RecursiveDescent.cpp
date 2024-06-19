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
        SetConsoleColor(stdout, ConsoleColor::RED);                     \
        fprintf(stdout, "SYNTAX ERROR AT %s\n", CUR_CHAR_PTR);          \
        SetConsoleColor(stdout, ConsoleColor::WHITE);                   \
        __VA_ARGS__;                                                    \
        return CREATE_ERROR(ERROR_SYNTAX);                              \
    }                                                                   \
} while (0)

#define SyntaxAssertResult(expression, poison, ...)                     \
do                                                                      \
{                                                                       \
    if (!(expression))                                                  \
    {                                                                   \
        SetConsoleColor(stdout, ConsoleColor::RED);                     \
        fprintf(stdout, "SYNTAX ERROR AT %s\n", CUR_CHAR_PTR);          \
        SetConsoleColor(stdout, ConsoleColor::WHITE);                   \
        __VA_ARGS__;                                                    \
        return { poison, CREATE_ERROR(ERROR_SYNTAX) };                  \
    }                                                                   \
} while (0)

#define CUR_CHAR_PTR (context.expression)

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

struct Context
{
    const char* expression;
    LinkedList* symbolTable;
};

TreeNodeResult _getS     (Context& context);
TreeNodeResult _getE     (Context& context);
TreeNodeResult _getT     (Context& context);
TreeNodeResult _getD     (Context& context);
TreeNodeResult _getP     (Context& context);
TreeNodeResult _getSymbol(Context& context);
TreeNodeResult _getName  (Context& context);
TreeNodeResult _getId    (Context& context);
TreeNodeResult _getN     (Context& context);

Error ParseExpression(Tree& tree, LinkedList& symbolTable, String& string)
{
    Context context = { string.buf, &symbolTable };

    TreeNodeResult root = _getS(context);

    RETURN_ERROR(root.error);

    SyntaxAssert(*CUR_CHAR_PTR == '\0');

    RETURN_ERROR(tree.Init(root.value));

    return Error();
}

TreeNodeResult _getS(Context& context)
{
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

    return { result, Error() };
}

TreeNodeResult _getE(Context& context)
{
    CREATE_NODE_SAFE(result, _getT(context));

    while (*CUR_CHAR_PTR == '+' || *CUR_CHAR_PTR == '-')
    {
        Operation op = *CUR_CHAR_PTR == '+' ? ADD_OPERATION : SUB_OPERATION;

        CUR_CHAR_PTR++;

        CREATE_NODE_SAFE(nextT, _getT(context), result->Delete());

        CREATE_NODE_SAFE(resCopy, TreeNode::New(result->value, result->left, result->right),
            result->Delete(), nextT->Delete());

        Error err = result->SetLeft(resCopy);
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

    return { result, Error() };
}

TreeNodeResult _getT(Context& context)
{
    CREATE_NODE_SAFE(result, _getD(context));

    while (*CUR_CHAR_PTR == '*' || *CUR_CHAR_PTR == '/')
    {
        Operation op = *CUR_CHAR_PTR == '*' ? MUL_OPERATION : DIV_OPERATION;

        CUR_CHAR_PTR++;

        CREATE_NODE_SAFE(nextD, _getD(context), result->Delete());

        CREATE_NODE_SAFE(resCopy, TreeNode::New(result->value, result->left, result->right),
            result->Delete(), nextD->Delete());

        Error err = result->SetLeft(resCopy);
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

    return { result, Error() };
}

TreeNodeResult _getD(Context& context)
{
    CREATE_NODE_SAFE(result, _getP(context));

    while (*CUR_CHAR_PTR == '^')
    {
        CUR_CHAR_PTR++;

        CREATE_NODE_SAFE(nextD, _getD(context), result->Delete());

        CREATE_NODE_SAFE(resCopy, TreeNode::New(result->value, result->left, result->right),
            result->Delete(), nextD->Delete());

        Error err = result->SetLeft(resCopy);
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

    return { result, Error() };
}

TreeNodeResult _getP(Context& context)
{
    if (*CUR_CHAR_PTR == '(')
    {
        CUR_CHAR_PTR++;

        CREATE_NODE_SAFE(result, _getE(context));

        SyntaxAssertResult(*CUR_CHAR_PTR == ')', nullptr, result->Delete());
        CUR_CHAR_PTR++;

        return { result, Error() };
    }

    return _getId(context);
}

TreeNodeResult _getSymbol(Context& context)
{
    return _getName(context);
}

TreeNodeResult _getName(Context& context)
{
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

    ListElemIndexResult findNameResult = context.symbolTable->Find({ name.buf, ANY_SYMBOL });

    if (findNameResult.error == ERROR_NOT_FOUND)
    {
        SymbolTableEntry symbol = {};
        symbol.Create(&name, VARIABLE_SYMBOL);
        Error err = context.symbolTable->PushBack(symbol);
        if (err)
        {
            result->Delete();
            return { nullptr, err };
        }
    }

    return { result, Error() };
}

TreeNodeResult _getId(Context& context)
{
    if (isalpha(*CUR_CHAR_PTR))
        return _getName(context);

    return _getN(context);
}

TreeNodeResult _getN(Context& context)
{
    char* endPtr = nullptr;

    double val = strtod(CUR_CHAR_PTR, &endPtr);
    SyntaxAssertResult(CUR_CHAR_PTR != endPtr, nullptr);

    CUR_CHAR_PTR = endPtr;

    CREATE_NODE_SAFE(result, TreeNode::New({}));

    NODE_TYPE(result)   = NUMBER_TYPE;
    NODE_NUMBER(result) = val;

    return { result, Error() };
}
