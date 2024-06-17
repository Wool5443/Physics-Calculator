#include <string.h>
#include "RecursiveDescent.hpp"

static const char* LOG_FOLDER = "../log";

int main(int argc, char* argv[])
{
    String expression = {};
    expression.Create(256);
    
    scanf("%s", expression.buf);

    expression.Filter();

    #ifndef NDEBUG
    PRINT_ERROR(Tree::StartLogging(LOG_FOLDER));
    #endif

    Tree tree = {};

    ErrorCode err = ParseExpression(&tree, &expression);

    #ifndef NDEBUG
    tree.Dump();
    Tree::EndLogging();
    #endif

    tree.Destructor();
    expression.Destructor();

    return 0;
}