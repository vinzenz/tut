#include "structure.h"
#include "errors.h"

namespace reflection {

Structure::Structure(Namespace *ns, const std::string &name, const std::string &value, const std::string &sibling, const std::string &specification) : 
    ReflectionType(value, sibling, specification), ns(ns), name(name), byteSize(0x00)
{}

Structure::Structure(Namespace *ns, const std::string &name, const std::string &value, const std::string &sibling) : 
    ReflectionType(value, sibling, ""), ns(ns), name(name), byteSize(0x00)
{}

Structure::Structure(Namespace *ns, const std::string &name, const std::string &value) : 
    ReflectionType(value, "", ""), ns(ns), name(name), byteSize(0x00)
{}

Structure::Structure(Namespace *ns, const std::string &name) : 
    ReflectionType("", "", ""), ns(ns), name(name), byteSize(0x00)
{}

Structure::Structure() : 
    ReflectionType("", "", ""), ns(NULL), name(""), byteSize(0x00)
{}

std::string Structure::getName()
{
    return this->name;
}

std::vector<Member *> *Structure::getMembers()
{
    return &this->members;
}

std::vector<Method *> *Structure::getMethods()
{
    return &this->methods;
}

std::vector<Method *> *Structure::getMethods(unsigned short type)
{
    std::vector<Method *> *result = new std::vector<Method *>();

    std::vector<Method *>::iterator method;
    for(method = getMethods()->begin(); method < getMethods()->end(); method++)
    {
        switch(type)
        {
//FIXME
        case _CONSTRUCTOR:
            if( (*method)->getType() == type )
                result->push_back(*method);
        break;

        case _DESTRUCTOR:
            if( (*method)->getType() == type )
                result->push_back(*method);
        break;

        case _METHOD:
            if( (*method)->getType() == type )
                result->push_back(*method); 
        break;
        
        }
    }

    return result;
}

std::vector<Method *> *Structure::getConstructors()
{
    return getMethods(_CONSTRUCTOR);
}

std::vector<Method *> *Structure::getDestructors()
{
    return getMethods(_DESTRUCTOR);
}

std::vector<Inheritance *> *Structure::getInheritances()
{
    return &this->inheritances;
}

Member *Structure::getMember(const std::string &name)
{
    for(std::vector<Member *>::iterator member = this->getMembers()->begin(); member < this->getMembers()->end(); member++)
        if( (*member)->getName() == name )
            return *member;

    throw Failure(_MEMBER_ERROR + name);
}

unsigned int Structure::getByteSize()
{
    return byteSize;
}

Method *Structure::getMethod(const std::string &name)
{
    for(std::vector<Method *>::iterator method = this->getMethods()->begin(); method < this->getMethods()->end(); method++)
        if( (*method)->getName() == name )
            return *method;

    throw Failure(_METHOD_ERROR + name);
}

Namespace *Structure::getNamespace()
{
    return this->ns;
}

void setStructureValues(Structure *structure, const std::string &sibling, const std::string &specification)
{
    structure->sibling = sibling;
    structure->specification = specification;
}

void setStructureByteSize(Structure *structure, unsigned int byteSize)
{
    if( byteSize > 0 )
        structure->byteSize = byteSize;
}

std::string getStructureSibling(Structure *structure)
{
    return structure->sibling;
}

std::string getStructureSpecification(Structure *structure)
{
    return structure->specification;
}

std::string getStructureValue(Structure *structure)
{
    return structure->value;
}

Inheritance::Inheritance(Structure *structure, const std::string &name, unsigned int offset) :
    structure(structure), name(name), offset(offset)
{}

Inheritance::Inheritance()
{}

std::string Inheritance::getName()
{
    if( this->name[0] == '<' )
    {
        unsigned int stop = 10;
        do
        {
            for(unsigned int n=0; n < this->getClass()->getNamespace()->getReflection()->baseTypes.size(); n++)
                if( this->getClass()->getNamespace()->getReflection()->baseTypes[n]->value == this->name )
                {
                    std::string value = "";
                    if( this->getClass()->getNamespace()->getReflection()->baseTypes[n]->ns != "" && this->getClass()->getNamespace()->getReflection()->baseTypes[n]->ns != this->getClass()->getNamespace()->getName() )
                        value += this->getClass()->getNamespace()->getReflection()->baseTypes[n]->ns + "::";
                    value += this->getClass()->getNamespace()->getReflection()->baseTypes[n]->name; 

                    this->name = value;
                }

            stop--;
        }
        while( this->name[0] != '<' && stop > 0 );

        if( this->name[0] == '<' )
           return _UNKNOWN_TYPE;
    }

    return this->name;
}

unsigned int Inheritance::getOffset()
{
    return this->offset;
}

Structure *Inheritance::getStructure()
{
    return this->structure;
}

Structure *Inheritance::getClass()
{
    return getStructure();
}

}
