#include <string.h>
#include "RecursiveDescent.hpp"
#include "Calculator.hpp"
#include "DSL.hpp"

double _recEvaluate(const TreeNode* node, LinkedList& symbolTable);
Error _assign(const TreeNode* node, LinkedList& symbolTable);
double _getSymbolValue(const TreeNode* node, LinkedList& symbolTable);

#define ON_ERROR(err, ...)                  \
do                                          \
{                                           \
    if (err)                                \
    {                                       \
        err.Print();                        \
        __VA_ARGS__;                        \
        return err;                         \
    }                                       \
} while (0)


constexpr size_t MAX_LINE_SIZE = 512;

Error Run(const char* listLogFolder, const char* treeLogFolder)
{
    Error err = Error(); 

    LinkedList symbolTable = {};
    err = symbolTable.Init();
    ON_ERROR(err);

    #ifndef NDEBUG
    err = symbolTable.StartLogging(listLogFolder);
    ON_ERROR(err, symbolTable.Destructor());
    err = Tree::StartLogging(treeLogFolder);
    ON_ERROR(err, symbolTable.Destructor());
    #endif

    Tree tree = {};
    String expression = {};

    err = expression.Create(MAX_LINE_SIZE);
    ON_ERROR(err, symbolTable.Destructor());

    while (fgets(expression.buf, MAX_LINE_SIZE, stdin))
    {
        err = expression.Filter();
        ON_ERROR(err, symbolTable.Destructor(); expression.Destructor());
        expression.length = strlen(expression.buf);

        if (expression.length == 0)
            continue;

        err = ParseExpression(tree, symbolTable, expression);

        ON_ERROR(err, symbolTable.Destructor(); tree.Destructor(); expression.Destructor());

        err = EvaluateTree(tree, symbolTable, expression);

        ON_ERROR(err, symbolTable.Destructor(); tree.Destructor(); expression.Destructor());

        #ifndef NDEBUG
        symbolTable.Dump();
        tree.Dump();
        #endif
        tree.Destructor();
    }

    expression.Destructor();
    symbolTable.Destructor();

    #ifndef NDEBUG
    symbolTable.EndLogging();
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

Error EvaluateTree(const Tree& tree, LinkedList& symbolTable, const String& expression)
{
    TreeNode* root = tree.root;

    if (NODE_TYPE(root) == OPERATION_TYPE &&
        NODE_OPERATION(root) == ASSIGN_OPERATION)
        return _assign(root, symbolTable);

    double value = _recEvaluate(tree.root, symbolTable);

    printf("%s = %lg\n", expression.buf, value);

    return Error();
}

double _recEvaluate(const TreeNode* node, LinkedList& symbolTable)
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

Error _assign(const TreeNode* node, LinkedList& symbolTable)
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
