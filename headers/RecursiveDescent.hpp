#ifndef PH_CALCULATOR_RECURSIVE_DESCENT_HPP
#define PH_CALCULATOR_RECURSIVE_DESCENT_HPP

//!@file

#include "AST.hpp"
#include "BinaryTree.hpp"
#include "LinkedList.hpp"

namespace PhCalculator {

Utils::Error ParseExpression(mlib::BinaryTree<TreeElement>&      tree,
                             mlib::LinkedList<SymbolTableEntry>& symbolTable,
                             mlib::String<>&                     string);

} // namespace PhCalculator

#endif
