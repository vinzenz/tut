#ifndef __NAMESPACE_H
#define __NAMESPACE_H

#include <iostream>
#include <vector>
#include "reflection.h"
#include "structure.h"
#include "type.h"

namespace reflection {

class Reflection;
class Structure;

/**  
 * \brief Keep Namespaces information
 * @author Krzysztof Jusiak
 * @date 03/02/2009
 */
class Namespace : public ReflectionType
{
    Namespace *parent;
    Reflection *reflection;
    std::string name;
    std::vector<Structure *> structures;
    typedef std::vector<Structure *>::iterator iterator;

public:
    Namespace(Namespace *, Reflection *, const std::string &, const std::string &);
    Namespace(Namespace *, Reflection *, const std::string &);
    Namespace();
                                                    
    std::string getName();
    std::vector<Structure *> *getTypes();
    Structure *getStructure(const std::string &);
    Structure *getClass(const std::string &);
    Structure *getType(const std::string &);
    Reflection *getReflection();
    iterator begin() { return structures.begin(); }
    iterator end() { return structures.end(); }

    friend std::string getNamespaceSibling(Namespace *);
    friend void setNamespaceSibling(Namespace *, const std::string &);
};

}

#endif
