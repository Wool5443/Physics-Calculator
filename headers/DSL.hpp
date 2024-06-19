#pragma once

#define CREATE_NODE_SAFE(name, expression, ...)                         \
TreeNode* name;                                                         \
do                                                                      \
{                                                                       \
    TreeNodeResult _TEMP = (expression);                                \
    RETURN_RESULT(_TEMP);                                               \
    name = _TEMP.value;                                                 \
} while (0)                                                                      

#define NODE_TYPE(node) ((node)->value.type)
#define NODE_NUMBER(node) ((node)->value.number)
#define NODE_NAME(node) ((node)->value.name)
#define NODE_OPERATION(node) ((node)->value.operation)
#define NODE_PRIORITY(node) ((node)->value.priority)
