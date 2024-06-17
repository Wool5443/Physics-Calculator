#include <string.h>
#include "RecursiveDescent.hpp"
#include "Calculator.hpp"

static const char* LOG_FOLDER = "../log";

constexpr size_t MAX_LINE_SIZE = 512;

int main()
{
    #ifndef NDEBUG
    PRINT_ERROR(Tree::StartLogging(LOG_FOLDER));
    #endif

    Tree tree = {};
    String expression = {};
    ErrorCode err = EVERYTHING_FINE; 
    expression.Create(MAX_LINE_SIZE);

    while (true)
    {
        fgets(expression.buf, MAX_LINE_SIZE, stdin);
        expression.Filter();

        if (strlen(expression.buf) == 0)
            break;

        err = ParseExpression(tree, expression);

        double result = EvaluateTree(tree);

        printf("%s = %lg\n", expression.buf, result);

        #ifndef NDEBUG
        tree.Dump();
        #endif
        tree.Destructor();
    }

    expression.Destructor();

    return 0;
}