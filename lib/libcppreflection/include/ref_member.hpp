#ifndef __MEMBER_H
#define __MEMBER_H

#include <iostream>
#include "ref_reflection.hpp"
#include "ref_structure.hpp"

template<typename _CastTo, typename _CastFrom>
_CastTo union_cast(_CastFrom _value);

namespace reflection {

class Structure;

class Member
{
    Structure *structure;
    std::string name;
    std::string type;
    unsigned int accessibility;
    unsigned int offset;
    unsigned int byteSize;
    unsigned short pointerType;
    unsigned short constType;
    unsigned short flags;

    void setType();
    void setValues(const std::string &, unsigned int, unsigned short, unsigned short);

public:
    Member(Structure *, const std::string &, const std::string &, unsigned int, unsigned int);
    Member(Structure *, const std::string &);
    Member();

    std::string getName();
    std::string getType();
    std::string getFullType();
    unsigned int getAccessibility();
    unsigned int getOffset();
    unsigned int getByteSize();
    unsigned short getPointerType();
    unsigned short getConstType();
    Structure *getStructure();
    Structure *getClass();

/*    void *getValue(void *object)*/
    //{
        //int *tmp = (int *)object;
        //*(tmp += offset / sizeof(int *));

        //return (void *)*tmp;
    //}

    //void setValue(void *object, void *value)
    //{
        //int *tmp = (int *)object;
        //*(tmp += offset / sizeof(int *));

        //*tmp = reinterpret_cast<int>(value);
    /*}*/
};

}

#endif
