// DEF_FUNC(name, priority, hasOneArg, string, stringLength, code)

DEF_FUNC(ASSIGN_OPERATION,  0, false, "=",      1, (NAN))
DEF_FUNC(ADD_OPERATION,     1, false, "+",      1, (a) + (b))
DEF_FUNC(SUB_OPERATION,     1, false, "-",      1, (a) - (b))
DEF_FUNC(MUL_OPERATION,     2, false, "*",      1, (a) * (b))
DEF_FUNC(DIV_OPERATION,     2, false, "/",      1, (a) / (b))
DEF_FUNC(POWER_OPERATION,   3, false, "^",      1, pow(a, b))
