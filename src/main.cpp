#include <string.h>
#include <signal.h>
#include "RecursiveDescent.hpp"
#include "Calculator.hpp"

void sigintFunction(int signum);
void sigintFunction(int signum)
{
    printf("\nTo exit programm press Ctrl+D:%d\n", signum);
}

int main()
{
    signal(SIGINT, sigintFunction);

    Error err = Run("../listLog", "../treeLog");

    err.Print();

    return err;
}
