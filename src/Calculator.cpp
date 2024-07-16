#include <string.h>
#include "RecursiveDescent.hpp"
#include "Calculator.hpp"
#include "DSL.hpp"

using namespace mlib;
using namespace PhCalculator;

namespace {

Utils::Error _evaluateTree(const Tree& tree, List& symbolTable, const String<>& expression);
double _recEvaluate(const Node* node, List& symbolTable);
Utils::Error _assign(const Node* node, List& symbolTable);
double _getSymbolValue(const Node* node, List& symbolTable);

}

constexpr std::size_t MAX_LINE_SIZE = 512;

Utils::Error PhCalculator::Run(const char* listLogFolder, const char* treeLogFolder)
{
    List   symbolTable{};
    RETURN_ERROR(symbolTable.Error());
    Tree   tree{};
    RETURN_ERROR(tree.error);
    String expression{MAX_LINE_SIZE};
    RETURN_ERROR(expression.Error());

    #ifndef NDEBUG
    symbolTable.StartLogging(listLogFolder);
    tree.StartLogging(treeLogFolder);
    #endif

    char readBuffer[MAX_LINE_SIZE];

    while (fgets(readBuffer, MAX_LINE_SIZE, stdin))
    {
        expression = readBuffer;
        RETURN_ERROR(expression.Filter());

        if (expression.Length() == 0)
            continue;

        RETURN_ERROR(ParseExpression(tree, symbolTable, expression));

        RETURN_ERROR(_evaluateTree(tree, symbolTable, expression));

        #ifndef NDEBUG
        RETURN_ERROR(symbolTable.Dump());
        RETURN_ERROR(tree.Dump());
        #endif

        tree.root->Delete();
        tree.root = nullptr;
    }

    #ifndef NDEBUG
    symbolTable.EndLogging();
    tree.EndLogging();
    #endif

    return Utils::Error();
}
namespace {

Utils::Error _evaluateTree(const Tree& tree, List& symbolTable, const String<>& expression)
{
    Node* root = tree.root;

    if (NODE_TYPE(root) == OPERATION_TYPE &&
        NODE_OPERATION(root) == ASSIGN_OPERATION)
        return _assign(root, symbolTable);

    double value = _recEvaluate(tree.root, symbolTable);

    std::cout << expression << " = " << value << '\n';

    return Utils::Error();
}

double _recEvaluate(const Node* node, List& symbolTable)
{
    HardAssert(node, Utils::ERROR_NULLPTR);

    switch (NODE_TYPE(node))
    {
        case NUMBER_TYPE:
            return NODE_NUMBER(node);
        case NAME_TYPE:
            return _getSymbolValue(node, symbolTable);
        case OPERATION_TYPE:
            switch (NODE_OPERATION(node))
            {

#define DEF_FUNC(name, priority, hasOneArg, string, stringLength, code)     \
case name:                                                                  \
{                                                                           \
    double a, b;                                                            \
                                                                            \
    if (!node->left) return NAN;                                            \
    a = _recEvaluate(node->left, symbolTable);                              \
                                                                            \
    if (!hasOneArg)                                                         \
    {                                                                       \
        if (!node->right) return NAN;                                       \
        b = _recEvaluate(node->right, symbolTable);                         \
    }                                                                       \
                                                                            \
    return code;                                                            \
}

#include "Functions.hpp"

#undef DEF_FUNC

                default:
                    return NAN;
            }
        default:
            return NAN;
    }

    return NAN;
}

Utils::Error _assign(const Node* node, List& symbolTable)
{
    HardAssert(node, Utils::ERROR_NULLPTR);

    double value = _recEvaluate(node->right, symbolTable);

    auto indexRes = symbolTable.Find(SymbolTableEntry(NODE_NAME(node->left)));

    RETURN_ERROR(indexRes);

    symbolTable[indexRes.value].value = value;

    return Utils::Error();
}

double _getSymbolValue(const Node* node, List& symbolTable)
{
    HardAssert(node, Utils::ERROR_NULLPTR);

    auto indexRes = symbolTable.Find(SymbolTableEntry(NODE_NAME(node)));

    if (!indexRes)
        return NAN;

    return symbolTable[indexRes.value].value;
}

}
