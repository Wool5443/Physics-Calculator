#pragma once

#include "LinkedList.hpp"
#include "Tree.hpp"

ErrorCode ParseExpression(Tree& tree, LinkedList& symbolTable, String& string);
