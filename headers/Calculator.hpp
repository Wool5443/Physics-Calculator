#pragma once

//!@file

#include "Tree.hpp"
#include "LinkedList.hpp"

Error Run(const char* listLogFolder, const char* treeLogFolder);

Error EvaluateTree(const Tree& tree, LinkedList& symbolTable, const String& expression);
