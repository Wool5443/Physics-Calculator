#include <string.h>
#include <signal.h>
#include "RecursiveDescent.hpp"
#include "Calculator.hpp"

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

    Error err = Error(); 

    LinkedList symbolTable = {};
    err = symbolTable.Init();
    ON_ERROR(err);

    #ifndef NDEBUG
    err = symbolTable.StartLogging(LIST_LOG_FOLDER);
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

    return 0;
}
