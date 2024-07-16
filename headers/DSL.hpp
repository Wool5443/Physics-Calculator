#pragma once

#include "BinaryTree.hpp"
#include "LinkedList.hpp"
#include "AST.hpp"

using Node       = mlib::BinaryTreeNode<PhCalculator::TreeElement>;
using NodeResult = Utils::Result<mlib::BinaryTreeNode<PhCalculator::TreeElement>*>;
using Tree       = mlib::BinaryTree<PhCalculator::TreeElement>;
using List       = mlib::LinkedList<PhCalculator::SymbolTableEntry>;

#define CREATE_NODE_SAFE(name, expression)                              \
Node* name;                                                             \
do                                                                      \
{                                                                       \
    NodeResult _TEMP = (expression);                                    \
    RETURN_RESULT(_TEMP);                                               \
    name = _TEMP.value;                                                 \
} while (0)                                                                      

#define NODE_TYPE(node) ((node)->value.type)
#define NODE_NUMBER(node) ((node)->value.number)
#define NODE_NAME(node) ((node)->value.name)
#define NODE_OPERATION(node) ((node)->value.operation)
#define NODE_PRIORITY(node) ((node)->value.priority)
