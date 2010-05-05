#ifndef __METHOD_H
#define __METHOD_H

#include <iostream>
#include <vector>
#include <stdarg.h>

#include "ref_reflection.hpp"
#include "ref_structure.hpp"

#define _METHOD         (unsigned short)0x00
#define _CONSTRUCTOR    (unsigned short)0x01
#define _DESTRUCTOR     (unsigned short)0x02

template<typename _CastTo, typename _CastFrom>
_CastTo union_cast(_CastFrom _value);

namespace reflection {

class Structure;

class Method
{
    Structure *structure;
    std::string name;
    std::string mangle;
    std::string returnType;
    long long ptr;
    unsigned short accessibility;
    unsigned short returnPointerType;
    unsigned short type;

    void setType();

public:
    Method(Structure *, const std::string &, const std::string &, const std::string &, long long, unsigned int);
    Method(Structure *, const std::string &, const std::string &, const std::string &, unsigned int, unsigned short);
    Method(Structure *, const std::string &, const std::string &, const std::string &);
    Method(const std::string &, long long);
    Method();

    static std::vector<std::string> *getParameters(const std::string &);
    std::vector<std::string> *getParameters();
    std::string getName();
    std::string getMangle();
    std::string getDemangle();
    static std::string getDemangle(const std::string &mangle);
    std::string getReturnType();
    std::string getFullReturnType();
    unsigned short getReturnPointerType();
    unsigned short getAccessibility();
    long long getPtr();
    unsigned short getType();
    Structure *getStructure();
    Structure *getClass();

    friend void setMethodName(Method *, const std::string &);
};

} // namespace reflection

#endif
