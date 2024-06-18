#include <string.h>
#include <signal.h>
#include "PrettyDumpList.hpp"
#include "RecursiveDescent.hpp"
#include "Calculator.hpp"

#define ON_ERROR(err, ...)                  \
do                                          \
{                                           \
    if (err)                                \
    {                                       \
        PRINT_ERROR(err);                   \
        __VA_ARGS__;                        \
        return err;                         \
    }                                       \
} while (0)

static const char* TREE_LOG_FOLDER = "../treeLog";
static const char* LIST_LOG_FOLDER = "../listLog";

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

    LinkedList symbolTable = {};
    err = symbolTable.Init(LIST_LOG_FOLDER);
    ON_ERROR(err);

    #ifndef NDEBUG
    err = StartHtmlLogging(LIST_LOG_FOLDER);
    ON_ERROR(err, symbolTable.Destructor());
    err = Tree::StartLogging(TREE_LOG_FOLDER);
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

        double result = EvaluateTree(tree);

        printf("%s = %lg\n", expression.buf, result);

        #ifndef NDEBUG
        DumpList(&symbolTable, symbolTable.Verify());
        tree.Dump();
        #endif
        tree.Destructor();
    }

    expression.Destructor();
    symbolTable.Destructor();

    #ifndef NDEBUG
    EndHtmlLogging();
    Tree::EndLogging();
    #endif

    return 0;
}
