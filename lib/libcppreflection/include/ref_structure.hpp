#ifndef __STRUCTURE_H
#define __STRUCTURE_H

#include <iostream>
#include "ref_reflection.hpp"
#include "ref_namespace.hpp"
#include "ref_member.hpp"
#include "ref_method.hpp"
#include "ref_basic.hpp"

namespace reflection {

class Namespace;
class Inheritance;
class Member;
class Method;

class Structure : public ReflectionBasic
{
    Namespace *ns;
    std::string name;
    std::vector<Member *> members;
    std::vector<Method *> methods;
    std::vector<Inheritance *> inheritances;
    unsigned int byteSize;
    typedef std::vector<Member *>::iterator iteratorMember;
    typedef std::vector<Method *>::iterator iteratorMethod;
    typedef std::vector<Inheritance *>::iterator iteratorInheritance;

public:
    Structure(Namespace *, const std::string &, const std::string &, const std::string &, const std::string &);
    Structure(Namespace *, const std::string &, const std::string &, const std::string &);
    Structure(Namespace *, const std::string &, const std::string &);
    Structure(Namespace *, const std::string &);
    Structure();

    std::string getName();
    std::vector<Member *> *getMembers();
    std::vector<Method *> *getMethods();
    std::vector<Method *> *getMethods(unsigned short);
    std::vector<Method *> *getConstructors();
    std::vector<Method *> *getDestructors();
    std::vector<Inheritance *> *getInheritances();
    Member *getMember(const std::string &);
    unsigned int getByteSize();
    Method *getMethod(const std::string &);
    Namespace *getNamespace();
    iteratorMember beginMember() { return members.begin(); };
    iteratorMethod beginMethod() { return methods.begin(); };
    iteratorInheritance beginInheritance() { return inheritances.begin(); };
    iteratorMember endMember() { return members.end(); };
    iteratorMethod endMethod() { return methods.end(); };
    iteratorInheritance endInheritance() { return inheritances.end(); }

    friend void setStructureValues(Structure *, const std::string &, const std::string &);
    friend void setStructureByteSize(Structure *, unsigned int);
    friend std::string getStructureSibling(Structure *);
    friend std::string getStructureSpecification(Structure *);
    friend std::string getStructureValue(Structure *);
};

class Inheritance
{
    Structure *structure;
    std::string name;
    unsigned int offset;

public:
    Inheritance(Structure *, const std::string &, unsigned int);
    Inheritance();

    std::string getName();
    unsigned int getOffset();
    Structure *getStructure();
    Structure *getClass();
};

}

#endif
