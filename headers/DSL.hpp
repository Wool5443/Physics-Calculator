#pragma once

#define NODE_TYPE(node) ((node)->value.type)
#define NODE_NUMBER(node) ((node)->value.value.number)
#define NODE_VAR(node) ((node)->value.value.var)
#define NODE_OPERATION(node) ((node)->value.value.operation)
#define NODE_PRIORITY(node) ((node)->value.priority)
