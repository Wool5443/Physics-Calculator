#include <utility>
#include "DSL.hpp"
#include "RecursiveDescent.hpp"

#define SyntaxAssert(expression, ...)                                   \
do                                                                      \
{                                                                       \
    if (!(expression))                                                  \
    {                                                                   \
        SetConsoleColor(stdout, Utils::ConsoleColor::RED);              \
        fprintf(stdout, "SYNTAX ERROR AT %s\n", CUR_CHAR_PTR);          \
        SetConsoleColor(stdout, Utils::ConsoleColor::WHITE);            \
        __VA_ARGS__;                                                    \
        return CREATE_ERROR(Utils::ERROR_SYNTAX);                       \
    }                                                                   \
} while (0)

#define SyntaxAssertResult(expression, poison, ...)                     \
do                                                                      \
{                                                                       \
    if (!(expression))                                                  \
    {                                                                   \
        SetConsoleColor(stdout, Utils::ConsoleColor::RED);              \
        fprintf(stdout, "SYNTAX ERROR AT %s\n", CUR_CHAR_PTR);          \
        SetConsoleColor(stdout, Utils::ConsoleColor::WHITE);            \
        __VA_ARGS__;                                                    \
        return { poison, CREATE_ERROR(Utils::ERROR_SYNTAX) };           \
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
// P        -> -P | '(' E ')' | Id
// Symbol   -> Name
// Name     -> ALPHABET+ {DIGITS u ALPHABET}*
// Id       -> Symbol | N
// N        -> DIGITS+

using namespace mlib;
using namespace PhCalculator;

struct Context
{
    const char*                   expression;
    List& symbolTable;
};

NodeResult _getS     (Context& context);
NodeResult _getE     (Context& context);
NodeResult _getT     (Context& context);
NodeResult _getD     (Context& context);
NodeResult _getP     (Context& context);
NodeResult _getSymbol(Context& context);
NodeResult _getName  (Context& context);
NodeResult _getId    (Context& context);
NodeResult _getN     (Context& context);

Utils::Error PhCalculator::ParseExpression(Tree&     tree,
                                           List&     symbolTable,
                                           String<>& string)
{
    Context context = { string.RawPtr(), symbolTable };

    NodeResult root = _getS(context);

    RETURN_ERROR(root.error);

    SyntaxAssert(*CUR_CHAR_PTR == '\0');

    tree.root = root.value;

    return Utils::Error();
}

NodeResult _getS(Context& context)
{
    const char* assignmentCharPtr = strchr(CUR_CHAR_PTR, '=');

    Node* result = nullptr;

    if (assignmentCharPtr)
    {
        CREATE_NODE_SAFE(symbol, _getSymbol(context));

        SyntaxAssertResult(*CUR_CHAR_PTR == '=', nullptr);
        CUR_CHAR_PTR++;
        CREATE_NODE_SAFE(expression, _getE(context));
        CREATE_NODE_SAFE(_res, Node::New(ASSIGN_OPERATION, symbol, expression));
        result = _res;
    }
    else
    {
        CREATE_NODE_SAFE(_res, _getE(context));
        result = _res;
    }

    SyntaxAssertResult(*CUR_CHAR_PTR == '\0', nullptr);

    return { result, Utils::Error() };
}

NodeResult _getE(Context& context)
{
    CREATE_NODE_SAFE(result, _getT(context));

    while (*CUR_CHAR_PTR == '+' || *CUR_CHAR_PTR == '-')
    {
        Operation op = *CUR_CHAR_PTR == '+' ? ADD_OPERATION : SUB_OPERATION;

        CUR_CHAR_PTR++;

        CREATE_NODE_SAFE(nextT, _getT(context));

        CREATE_NODE_SAFE(resCopy, Node::New(result->value, result->left, result->right));

        RETURN_ERROR_RESULT(result->SetLeft (resCopy), nullptr);
        RETURN_ERROR_RESULT(result->SetRight(nextT),   nullptr);

        result->value = op;
    }

    return { result, Utils::Error() };
}

NodeResult _getT(Context& context)
{
    CREATE_NODE_SAFE(result, _getD(context));

    while (*CUR_CHAR_PTR == '*' || *CUR_CHAR_PTR == '/')
    {
        Operation op = *CUR_CHAR_PTR == '*' ? MUL_OPERATION : DIV_OPERATION;

        CUR_CHAR_PTR++;

        CREATE_NODE_SAFE(nextD, _getD(context));

        CREATE_NODE_SAFE(resCopy, Node::New(result->value, result->left, result->right));

        RETURN_ERROR_RESULT(result->SetLeft (resCopy), nullptr);
        RETURN_ERROR_RESULT(result->SetRight(nextD),   nullptr);

        result->value = op;
    }

    return { result, Utils::Error() };
}

NodeResult _getD(Context& context)
{
    CREATE_NODE_SAFE(result, _getP(context));

    while (*CUR_CHAR_PTR == '^')
    {
        CUR_CHAR_PTR++;

        CREATE_NODE_SAFE(nextD, _getD(context));

        CREATE_NODE_SAFE(resCopy, Node::New(result->value, result->left, result->right));

        RETURN_ERROR_RESULT(result->SetLeft (resCopy), nullptr);
        RETURN_ERROR_RESULT(result->SetRight(nextD),   nullptr);

        result->value = POWER_OPERATION;
    }

    return { result, Utils::Error() };
}

NodeResult _getP(Context& context)
{
    if (*CUR_CHAR_PTR == '-')
    {
        CUR_CHAR_PTR++;
        CREATE_NODE_SAFE(expression, _getE(context));
        CREATE_NODE_SAFE(result, Node::New(MINUS_OPERATION, expression, nullptr));

        return { result, Utils::Error() };
    }

    if (*CUR_CHAR_PTR == '(')
    {
        CUR_CHAR_PTR++;
        CREATE_NODE_SAFE(result, _getE(context));
        SyntaxAssertResult(*CUR_CHAR_PTR == ')', nullptr);
        CUR_CHAR_PTR++;

        return { result, Utils::Error() };
    }

    return _getId(context);
}

NodeResult _getSymbol(Context& context)
{
    return _getName(context);
}

NodeResult _getName(Context& context)
{
    SyntaxAssertResult(isalpha(*CUR_CHAR_PTR), nullptr);

    String name = *(CUR_CHAR_PTR++);

    while (isalnum(*CUR_CHAR_PTR) || *CUR_CHAR_PTR == '_')
        name += *(CUR_CHAR_PTR++);

    CREATE_NODE_SAFE(result, Node::New());
    NODE_TYPE(result) = NAME_TYPE;
    NODE_NAME(result) = name;

    auto findNameResult = context.symbolTable.Find(SymbolTableEntry(name));

    if (findNameResult.error == Utils::ERROR_NOT_FOUND)
    {
        SymbolTableEntry symbol{name, PhCalculator::VARIABLE_SYMBOL};

        RETURN_ERROR_RESULT(context.symbolTable.PushBack(symbol), nullptr);
    }

    return { result, Utils::Error() };
}

NodeResult _getId(Context& context)
{
    if (isalpha(*CUR_CHAR_PTR))
        return _getName(context);

    return _getN(context);
}

NodeResult _getN(Context& context)
{
    char* endPtr = nullptr;

    double val = strtod(CUR_CHAR_PTR, &endPtr);
    SyntaxAssertResult(CUR_CHAR_PTR != endPtr, nullptr);

    CUR_CHAR_PTR = endPtr;

    CREATE_NODE_SAFE(result, Node::New());

    NODE_TYPE(result)   = NUMBER_TYPE;
    NODE_NUMBER(result) = val;

    return { result, Utils::Error() };
}
