#ifndef REF_TYPE_HPP
#define REF_TYPE_HPP

#include <iostream>
#include <vector>
#include "ref_reflection.hpp"

namespace reflection
{

std::vector<Method*> getMethodPtr(const std::string& fileName, const std::string& regex);

} // namespace reflection

#endif

