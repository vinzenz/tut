#include "ref_namespace.hpp"

namespace reflection {

Namespace::Namespace(Namespace *parent, Reflection *reflection, const std::string &name) :
    ReflectionBasic(""), parent(parent), reflection(reflection), name(name)
{}

Namespace::Namespace(Namespace *parent, Reflection *reflection, const std::string &name, const std::string &sibling) :
    ReflectionBasic(sibling), parent(parent), reflection(reflection), name(name)
{}

Namespace::Namespace() :
    ReflectionBasic(sibling), parent(NULL), reflection(NULL), name("")
{}

std::string Namespace::getName()
{
    return this->name;
}

std::vector<Structure *> *Namespace::getTypes()
{
    return &this->structures;
}

Structure *Namespace::getStructure(const std::string &name)
{
    std::vector<Structure *>::iterator structure;
    for(structure = this->getTypes()->begin(); structure < this->getTypes()->end(); structure++)
        if( (*structure)->getName() == name )
            return *structure;

    throw Failure(_STRUCTURE_ERROR + name);
}

Structure *Namespace::getClass(const std::string &name)
{
    return getStructure(name);
}

Structure *Namespace::getType(const std::string &name)
{
    return getStructure(name);
}

Reflection *Namespace::getReflection()
{
    return this->reflection;
}

std::string getNamespaceSibling(Namespace *ns)
{
    return ns->sibling;
}

void setNamespaceSibling(Namespace *ns, const std::string &sibling)
{
    ns->sibling = sibling;
}

}
