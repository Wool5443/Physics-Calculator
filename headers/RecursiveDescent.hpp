#pragma once

#include "LinkedList.hpp"
#include "Tree.hpp"

Error ParseExpression(Tree& tree, LinkedList& symbolTable, String& string);
