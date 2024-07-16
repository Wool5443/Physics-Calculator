#ifndef PH_CALCULATOR_AST_HPP
#define PH_CALCULATOR_AST_HPP

#include <ostream>
#include "String.hpp"

namespace PhCalculator {

enum TreeElementType
{
    OPERATION_TYPE,
    NUMBER_TYPE,
    NAME_TYPE,
};

enum Operation
{
#define DEF_FUNC(name, ...) \
name,

#include "Functions.hpp"

#undef DEF_FUNC
};

constexpr int OPERATION_PRIORITIES[] = {
#define DEF_FUNC(name, priority, ...) \
priority,

#include "Functions.hpp"

#undef DEF_FUNC
};

struct TreeElement
{
    TreeElementType type;
    int priority;
    mlib::String<> name{};

    union
    {
        double         number;
        Operation      operation;
    };

    TreeElement()
        : type(NUMBER_TYPE), priority(0), number(0) {}

    TreeElement(Operation operation)
        : type(OPERATION_TYPE), priority(OPERATION_PRIORITIES[operation]),
        operation(operation) {}

    TreeElement(const mlib::String<>& name)
        : type(NAME_TYPE), priority(0),
        name(name) {}

    TreeElement(double number)
        : type(NUMBER_TYPE), priority(0),
        number(number) {}

    friend std::ostream& operator<<(std::ostream& out, const TreeElement& elem)
    {
        switch (elem.type)
        {
            case OPERATION_TYPE:
                return out << "Operation: " << (int)elem.operation;
            case NUMBER_TYPE:
                return out << "Number: " << elem.number;
            case NAME_TYPE:
                return out << "Name: " << elem.name;
            default:
                return out << "Bad type";
        }
    }
};

static const char* TREE_WORD_SEPARATOR = " ";

enum SymbolType
{
    ANY_SYMBOL = 0,
    VARIABLE_SYMBOL,
    CONST_SYMBOL,
    FUNCTION_SYMBOL,
};

using function_t = void* (*)(void*);

struct SymbolTableEntry
{
    mlib::String<> Name{};
    SymbolType     Type{};

    Utils::Error   Error()
    {
        return Name.Error();
    }

    union
    {
        double     value;
        function_t function;
    };
    SymbolTableEntry() noexcept {}
    SymbolTableEntry(const mlib::String<>& name, SymbolType type = ANY_SYMBOL) noexcept
        : Name(name), Type(type) {}
    SymbolTableEntry(mlib::String<>&& name, SymbolType type = ANY_SYMBOL) noexcept
        : Name(name), Type(type) {}

    bool operator==(const SymbolTableEntry& other) const noexcept
    {
        if (Type == ANY_SYMBOL || other.Type == ANY_SYMBOL)
            return Name == other.Name;
        return Type == other.Type &&   Name == other.Name;
    }
    bool operator!=(const SymbolTableEntry& other) const noexcept
    {
        return !(*this == other);
    }

    friend std::ostream& operator<<(std::ostream& out, const SymbolTableEntry entry)
    {
        return out << "Symbol: " << entry.Name;
    }
};

} // namespace PhCalculator

#endif
