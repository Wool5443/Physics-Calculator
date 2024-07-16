#ifndef PH_CALCULATOR_CALCULATOR_HPP
#define PH_CALCULATOR_CALCULATOR_HPP

//!@file

#include "Utils.hpp"

namespace PhCalculator {

#ifndef NDEBUG
Utils::Error Run(const char* listLogFolder, const char* treeLogFolder);
#else
Utils::Error Run();
#endif

} // namespace PhCalculator

#endif
