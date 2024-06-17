#include "Calculator.hpp"
#include "DSL.hpp"

double _recEvaluate(const TreeNode* node);

double EvaluateTree(const Tree& tree)
{
    return _recEvaluate(tree.root);
}

double _recEvaluate(const TreeNode* node)
{
    MyAssertSoft(node, ERROR_NULLPTR, return NAN);

    switch (NODE_TYPE(node))
    {
        case NUMBER_TYPE:
            return NODE_NUMBER(node);
        case OPERATION_TYPE:
            switch (NODE_OPERATION(node))
            {

#define DEF_FUNC(name, priority, hasOneArg, string, stringLength, code)     \
case name:                                                                  \
{                                                                           \
    double a, b;                                                            \
                                                                            \
    if (!node->left) return NAN;                                            \
    a = _recEvaluate(node->left);                                           \
                                                                            \
    if (!hasOneArg)                                                         \
    {                                                                       \
        if (!node->right) return NAN;                                       \
        b = _recEvaluate(node->right);                                      \
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
