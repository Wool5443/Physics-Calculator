#include <string.h>
#include <signal.h>
#include "RecursiveDescent.hpp"
#include "Calculator.hpp"

static const char* LOG_FOLDER = "../log";

constexpr size_t MAX_LINE_SIZE = 512;

void sigintFunction(int signum);
void sigintFunction(int signum)
{
    printf("\nTo exit programm press Ctrl+D:%d\n", signum);
}

int main()
{
    signal(SIGINT, sigintFunction);

    ErrorCode err = EVERYTHING_FINE; 

    #ifndef NDEBUG
    err = Tree::StartLogging(LOG_FOLDER);
    #endif

    Tree tree = {};
    String expression = {};

    err = expression.Create(MAX_LINE_SIZE);
    if (err)
    PRINT_ERROR(err);

    while (fgets(expression.buf, MAX_LINE_SIZE, stdin))
    {
        err = expression.Filter();
        if (err)
        {
            PRINT_ERROR(err);
            tree.Destructor();
            expression.Destructor();
            return err;
        }
        expression.length = strlen(expression.buf);

        if (expression.length == 0)
            continue;

        err = ParseExpression(tree, expression);

        if (err)
        {
            PRINT_ERROR(err);
            tree.Destructor();
            expression.Destructor();
            return err;
        }

        double result = EvaluateTree(tree);

        printf("%s = %lg\n", expression.buf, result);

        #ifndef NDEBUG
        tree.Dump();
        #endif
        tree.Destructor();
    }

    expression.Destructor();

    #ifndef NDEBUG
    Tree::EndLogging();
    #endif

    return 0;
}
