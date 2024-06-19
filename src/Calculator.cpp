#include "Calculator.hpp"
#include "DSL.hpp"

double _recEvaluate(const TreeNode* node, LinkedList& symbolTable);
Error _assign(const TreeNode* node, LinkedList& symbolTable);
double _getSymbolValue(const TreeNode* node, LinkedList& symbolTable);

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
