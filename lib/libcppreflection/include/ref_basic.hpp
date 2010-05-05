#ifndef REF_BASIC_HPP
#define REF_BASIC_HPP

#include "ref_reflection.hpp"

namespace reflection
{

class ReflectionBasic
{
public:
    std::string value;
    std::string sibling;
    std::string specification;

    ReflectionBasic(const std::string &value, const std::string &sibling, const std::string &specification) :
        value(value), sibling(sibling), specification(specification) {}

    ReflectionBasic(const std::string &sibling) :
        value(""), sibling(sibling), specification("") {}
};

} // namespace reflection

#endif

