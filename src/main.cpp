#include <string.h>
#include "RecursiveDescent.hpp"

static const char* LOG_FOLDER = "../log";

int main()
{
    PRINT_ERROR(Tree::StartLogging(LOG_FOLDER));

    String expression = {};
    expression.Create(32);

    // strcpy(expression.buf, "1 + 2 + 3 + 4");
    strcpy(expression.buf, "(1 + 2 + 3 + 4) * 2 ^ 2");

    expression.Filter();

    Tree tree = {};

    ErrorCode err = ParseExpression(&tree, &expression);

    tree.Dump();

    Tree::EndLogging();

    tree.Destructor();

    return 0;
}