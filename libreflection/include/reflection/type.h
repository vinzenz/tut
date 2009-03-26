#ifndef __TYPE_H
#define __TYPE_H

#include <iostream>
#include <vector>
#include "reflection.h"
#include "structure.h"

namespace reflection {

/**  
 * \brief Basic reflection type
 * @author Krzysztof Jusiak 
 * @date 03/02/2009 
 */
class ReflectionType
{
public:
    std::string value;
    std::string sibling;
    std::string specification;

    ReflectionType(const std::string &value, const std::string &sibling, const std::string &specification) :
        value(value), sibling(sibling), specification(specification) {}

    ReflectionType(const std::string &sibling) :
        value(""), sibling(sibling), specification("") {}
};

}

#endif
