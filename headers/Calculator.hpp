#pragma once

//!@file

#include "Tree.hpp"
#include "LinkedList.hpp"

Error EvaluateTree(const Tree& tree, LinkedList& symbolTable, const String& expression);
