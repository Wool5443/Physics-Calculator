#pragma once

//!@file

#include "Tree.hpp"
#include "LinkedList.hpp"

ErrorCode EvaluateTree(const Tree& tree, LinkedList& symbolTable, const String& expression);
