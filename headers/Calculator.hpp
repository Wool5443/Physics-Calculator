#pragma once

//!@file

#include "Tree.hpp"
#include "LinkedList.hpp"

struct SymbolTable
{
    LinkedList symbolTable;
};

double EvaluateTree(const Tree& tree);
