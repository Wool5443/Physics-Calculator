#include <string.h>
#include "RecursiveDescent.hpp"
#include "Calculator.hpp"
#include "DSL.hpp"

static Error _evaluateTree(const Tree& tree, LinkedList& symbolTable, const String& expression);
static double _recEvaluate(const TreeNode* node, LinkedList& symbolTable);
static Error _assign(const TreeNode* node, LinkedList& symbolTable);
static double _getSymbolValue(const TreeNode* node, LinkedList& symbolTable);

constexpr size_t MAX_LINE_SIZE = 512;

Error Run(const char* listLogFolder, const char* treeLogFolder)
{
    LinkedList symbolTable = {};
    RETURN_ERROR(symbolTable.Init());

    #ifndef NDEBUG
    RETURN_ERROR(LinkedList::StartLogging(listLogFolder), symbolTable.Destructor());
    RETURN_ERROR(Tree::StartLogging(treeLogFolder), symbolTable.Destructor());
    #endif

    Tree tree = {};
    String expression = {};

    RETURN_ERROR(expression.Create(MAX_LINE_SIZE), symbolTable.Destructor());

    while (fgets(expression.buf, MAX_LINE_SIZE, stdin))
    {
        RETURN_ERROR(expression.Filter(),
                     symbolTable.Destructor(); expression.Destructor());
        expression.length = strlen(expression.buf);

        if (expression.length == 0)
            continue;

        RETURN_ERROR(ParseExpression(tree, symbolTable, expression),
                     symbolTable.Destructor(); tree.Destructor(); expression.Destructor());

        RETURN_ERROR(_evaluateTree(tree, symbolTable, expression),
                     symbolTable.Destructor(); tree.Destructor(); expression.Destructor());

        #ifndef NDEBUG
        RETURN_ERROR(symbolTable.Dump());
        RETURN_ERROR(tree.Dump());
        #endif
        RETURN_ERROR(tree.Destructor());
    }

    expression.Destructor();
    RETURN_ERROR(symbolTable.Destructor());

    #ifndef NDEBUG
    LinkedList::EndLogging();
    Tree::EndLogging();
    #endif

    return Error();
}

Error SymbolTableEntry::Create(String* name, SymbolType type)
{
    RETURN_ERROR(this->name.Create(name));
    this->type = type;

    return Error();
}

void SymbolTableEntry::Destructor()
{
    this->name.Destructor();
    this->type = ANY_SYMBOL;
}


static Error _evaluateTree(const Tree& tree, LinkedList& symbolTable, const String& expression)
{
    TreeNode* root = tree.root;

    if (NODE_TYPE(root) == OPERATION_TYPE &&
        NODE_OPERATION(root) == ASSIGN_OPERATION)
        return _assign(root, symbolTable);

    double value = _recEvaluate(tree.root, symbolTable);

    printf("%s = %lg\n", expression.buf, value);

    return Error();
}

static double _recEvaluate(const TreeNode* node, LinkedList& symbolTable)
{
    SoftAssert(node, ERROR_NULLPTR, return NAN);

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

static Error _assign(const TreeNode* node, LinkedList& symbolTable)
{
    SoftAssert(node, ERROR_NULLPTR);

    double value = _recEvaluate(node->right, symbolTable);

    ListElemIndexResult indexRes = symbolTable.Find({ NODE_NAME(node->left).buf, ANY_SYMBOL });

    if (indexRes.error)
        return indexRes.error;
    else
        symbolTable.data[indexRes.value].value = value;

    return Error();
}

double _getSymbolValue(const TreeNode* node, LinkedList& symbolTable)
{
    SoftAssert(node, ERROR_NULLPTR);

    ListElemIndexResult indexRes = symbolTable.Find({ NODE_NAME(node).buf, ANY_SYMBOL });

    if (indexRes.error == ERROR_NOT_FOUND)
        return NAN;

    return symbolTable.data[indexRes.value].value;
}
