#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <iostream>
#include <sstream>
#include <string.h>
#include <cxxabi.h>
#include "ref_reflection.hpp"

extern const char *__progname;

namespace reflection {

bool Reflection::shortMode = false;

bool Reflection::checkELF(const std::string &fileName)
{
    Elf32_Ehdr hdr;
    int fd;

    if( (fd = open(fileName.c_str(), O_RDONLY)) < 0 )
        throw Failure(_IO_ERROR + fileName);

    if( read(fd, &hdr, sizeof(Elf32_Ehdr) ) != sizeof(Elf32_Ehdr) )
        throw Failure(_IO_ERROR + fileName);

    bool result =
       hdr.e_ident[0] == 0x7f && hdr.e_ident[1] == 'E' && hdr.e_ident[2] == 'L'  && hdr.e_ident[3] == 'F';

    close(fd);

    return result;
}

Reflection::Reflection() : fileName("") {
}

Reflection::Reflection(bool shortMode) : fileName("") {
    Reflection::shortMode = shortMode;
}

Reflection::Reflection(const std::string &name) : fileName(name) {
    load(name);
}

Reflection::Reflection(const std::string &name, bool shortMode) : fileName(name) {
    Reflection::shortMode = shortMode;
    load(name);
}

Reflection::~Reflection() {
}

void Reflection::allow(const std::string &reg)
{
    elements.allow(reg);
}

void Reflection::deny(const std::string &reg)
{
    elements.deny(reg);
}

Namespace *Reflection::getType(const std::string &name)
{
    for(std::vector<Namespace *>::iterator _namespace = this->getTypes()->begin(); _namespace < this->getTypes()->end(); _namespace++)
        if( (*_namespace)->getName() == name )
            return *_namespace;

    throw Failure(_NAMESPACE_ERROR + name);
}

Namespace *Reflection::getNamespace(const std::string &name)
{
    return getType(name);
}

Structure *Reflection::getClass(const std::string &name)
{
    for(std::vector<Namespace *>::iterator in = this->getTypes()->begin(); in < this->getTypes()->end(); in++)
    for(std::vector<Structure *>::iterator is = (*in)->getTypes()->begin(); is < (*in)->getTypes()->end(); is++)
        if( (*is)->getName() == name )
            return *is;

//klasy templetowe
//moze typedefs poszukac
//getshort powinien byc static, zeby w tucie mozna bylo zmieniac
//Uwaga. basci_string<fdsaf, fdsa>
    throw Failure(_STRUCTURE_ERROR + name);
}

Structure *Reflection::isClass(const std::string &name)
{
    try
    {
        return getClass(name);
    }
    catch(...)
    {
        return NULL;
    }

    return NULL;
}

Method *Reflection::getMethodByPtr(long long ptr)
{
    for(std::vector<Namespace *>::iterator in = this->getTypes()->begin(); in < this->getTypes()->end(); in++)
    for(std::vector<Structure *>::iterator is = (*in)->getTypes()->begin(); is < (*in)->getTypes()->end(); is++)
    for(std::vector<Method *>::iterator im = (*is)->getMethods()->begin(); im < (*is)->getMethods()->end(); im++)
        if( (*im)->getPtr() == ptr )
            return *im;

    return NULL;
}

Method *Reflection::getFunctionByPtr(long long ptr)
{
    for(std::vector<Method *>::iterator im = methodsPointers.begin(); im < methodsPointers.end(); im++)
        if( (*im)->getPtr() == ptr )
                return *im;

        return NULL;
}

Structure *Reflection::getStructure(const std::string &name)
{
    return getClass(name);
}

std::string Reflection::getFileName()
{
    return fileName;
}

void Reflection::load(const std::string &name, ttype type)
{
    this->fileName = name;

    if( checkELF(this->fileName) == false )
        throw Failure(_ELF_ERROR + this->fileName);

    if( type & _LOAD_FUNCTIONS )
    {
        methodsPointers = Reflection::loadMethods(name);
    }

    if( elements.size() > 0 )
    {
        if( type & _LOAD_STRUCTURES )
        {
            loadStructures(name);
        }
    }
}

void Reflection::load(ttype type)
{
    load(std::string(__progname), type);
}

std::vector<Reflection::Tag> *Reflection::initTags()
{
    std::vector<Tag> *tags = new std::vector<Tag>();

    Tag _namespace;
    _namespace.name = _DW_TAG_NAMESPACE;
    _namespace.tags.push_back(_DW_AT_NAME);
    _namespace.tags.push_back(_DW_AT_SIBLING);
    tags->push_back(_namespace);

    Tag _structure;
    _structure.name = _DW_TAG_STRUCTURE_TYPE;
    _structure.tags.push_back(_DW_AT_NAME);
    _structure.tags.push_back(_DW_AT_SIBLING);
    _structure.tags.push_back(_DW_AT_SPECIFICATION);
    _structure.tags.push_back(_DW_AT_BYTE_SIZE);
    tags->push_back(_structure);

    Tag _member;
    _member.name = _DW_TAG_MEMBER;
    _member.tags.push_back(_DW_AT_NAME);
    _member.tags.push_back(_DW_AT_TYPE);
    _member.tags.push_back(_DW_AT_ACCESSIBILITY);
    _member.tags.push_back(_DW_OP_PLUS_UCONST);
    tags->push_back(_member);

    Tag _method;
    _method.name = _DW_TAG_SUBPROGRAM;
    _method.tags.push_back(_DW_AT_NAME);
    _method.tags.push_back(_DW_AT_TYPE);
    _method.tags.push_back(_DW_AT_ACCESSIBILITY);
    _method.tags.push_back(_DW_AT_MIPS_LINKAGE_NAME);
    tags->push_back(_method);

    Tag _inheritance;
    _inheritance.name = _DW_TAG_INHERITANCE;
    _inheritance.tags.push_back(_DW_AT_TYPE);
    _inheritance.tags.push_back(_DW_AT_DATA_MEMBER_LOCATION);
    tags->push_back(_inheritance);

    Tag _baseType;
    _baseType.name = _DW_TAG_BASE_TYPE;
    _baseType.tags.push_back(_DW_AT_NAME);
    _baseType.tags.push_back(_DW_AT_BYTE_SIZE);
    tags->push_back(_baseType);

    Tag _pointerType;
    _pointerType.name = _DW_TAG_POINTER_TYPE;
    _pointerType.tags.push_back(_DW_AT_TYPE);
    tags->push_back(_pointerType);

    Tag _typedef;
    _typedef.name = _DW_TAG_TYPEDEF;
    _typedef.tags.push_back(_DW_AT_TYPE);
    tags->push_back(_typedef);

    Tag _arrayType;
    _arrayType.name = _DW_TAG_ARRAY_TYPE;
    _arrayType.tags.push_back(_DW_AT_TYPE);
    tags->push_back(_arrayType);

    Tag _referenceType;
    _referenceType.name = _DW_TAG_REFERENCE_TYPE;
    _referenceType.tags.push_back(_DW_AT_TYPE);
    tags->push_back(_referenceType);

    Tag _soubroutineType;
    _soubroutineType.name = _DW_TAG_SUBROUTINE_TYPE;
    _soubroutineType.tags.push_back(_DW_AT_TYPE);
    tags->push_back(_soubroutineType);

    Tag _enumerationType;
    _enumerationType.name = _DW_TAG_ENUMERATION_TYPE;
    _enumerationType.tags.push_back(_DW_AT_NAME);
    tags->push_back(_enumerationType);

    Tag _unionType;
    _unionType.name = _DW_TAG_UNION_TYPE;
    _unionType.tags.push_back(_DW_AT_NAME);
    tags->push_back(_unionType);

    Tag _subrangeType;
    _subrangeType.name = _DW_TAG_SUBRANGE_TYPE;
    _subrangeType.tags.push_back(_DW_AT_TYPE);
    tags->push_back(_subrangeType);

    Tag _constType;
    _constType.name = _DW_TAG_CONST_TYPE;
    _constType.tags.push_back(_DW_AT_TYPE);
    tags->push_back(_constType);

    return tags;
}

bool Reflection::setTmpValues(const std::string &parse, std::vector<Tag> tags, int type, std::string *tmpValues)
{
    size_t found;

    if( parse.substr(0, 2) == _SUB_LINE )
    {
        for(unsigned int i=0; i < tags[type].tags.size(); i++)
        {
            found = parse.find(tags[type].tags[i]);

            if( found != std::string::npos )
            {
                std::string name = "";
                unsigned int n;

                if( tags[type].tags[i] == _DW_OP_PLUS_UCONST || tags[type].tags[i] == _DW_AT_DATA_MEMBER_LOCATION )
                {
                    for(n=parse.length()-1; n >= 0; n--)
                        if( parse[n] == ':' )
                            break;

                    if( n > 0 )
                    {
                        for(unsigned int i=n+2; i < parse.length(); i++)
                            if( !isdigit(parse[i]) )
                                break;
                            else
                                name += parse[i];
                    }
                }
                else
                {
                    for(n=0; n < parse.length(); n++)
                        if( parse[n] == ':' )
                            break;

                    if( n < parse.length() - 1 )
                    {
                        int par1 = 0, par2 = 0;
                        n += 2;

                        unsigned int tmpN;
                        for(tmpN = n; tmpN < parse.length(); tmpN++)
                        {
                            if( parse[tmpN] == '(' )
                                par1++;
                            else
                            if( parse[tmpN] == ')' )
                                par1--;
                            else
                            if( parse[tmpN] == '<' )
                                par2++;
                            else
                            if( parse[tmpN] == '>' )
                                par2--;

                            if( par1 == 0 && par2 == 0 && parse[tmpN] == ':' )
                                break;
                        }

                        if( tmpN < parse.length() - 1 )
                            n = tmpN+2;

                        name = parse.substr(n, parse.length()-n-2);
                    }
                }

                if( tags[type].tags[i] == _DW_AT_ACCESSIBILITY )
                {
                    for(int n=0; i < name.length(); n++)
                        if( !isdigit(name[n]) )
                        {
                            name = name.substr(0, n);
                            break;
                        }
                }

                if( !isEmpty(name) )
                    tmpValues[i] = name;
            }
        }
    }
    else
        return false;

    return true;
}

bool Reflection::isEmpty(const std::string &name)
{
    for(unsigned int i=0; i < name.length(); i++)
        if( name[i] != ' ')
            return false;

    return true;
}

bool Reflection::isAll(std::vector<Tag> tags, int type, const std::string *tmpValues)
{
    for(unsigned int i=0; i < tags[type].tags.size(); i++)
        if( tmpValues[i] == _EMPTY_LINE )
            return false;

    return true;
}

int Reflection::getLineNr(const std::string &parse)
{
    size_t f1, f2;

    f1 = parse.find_first_of('<');
    f2 = parse.find_first_of('>');

    if( f1 != std::string::npos && f2 != std::string::npos )
        return (int)atoi( parse.substr(f1+1, f2-2).c_str() );

    return 0;
}

std::string Reflection::getParseValue(const std::string &parse, const std::string &name)
{
    size_t found;

    found = parse.find(name);
    if( found != std::string::npos )
    {
        size_t begin = parse.find_first_of('<', 2);
        size_t end = parse.find_first_of(':');

        if( begin != std::string::npos && end != std::string::npos )
            return parse.substr(begin, end - begin);
    }

    return _EMPTY_LINE;
}

void Reflection::loadStructures(const std::string &name)
{
    std::vector<Tag> tags = *initTags();

    FILE *fp = popen( (std::string(_READ_ELF) + " " + name).c_str(), "r");
    if( !fp )
        throw Failure( _IO_ERROR + name );

    try
    {
        std::map<Structure *, unsigned int> tmpStructures;
        char *line = NULL; size_t len = 0;
        bool isSub = false, firstStructure = false;
        int nr = 0, activeNr = -1, structureNr = -1;
        int type = 0, activeNamespace = 0, oldActiveNamespace = -1, activeStructure = -1;
        std::string *tmpValues = NULL;

        //default namespace and structure
        this->types.push_back( new Namespace(NULL, this, "") );
        this->types[0]->getTypes()->push_back( new Structure(this->types[0], _NO_STRUCTURE) );//for structural programming and functions without structures

        while( (getline(&line, &len, fp) ) != -1 )
        {
            std::string parse = std::string(line);

            if( isSub == true )
                if( !setTmpValues(parse, tags, type, tmpValues) )
                    isSub = false;

            if( isSub == false )
            {
                    if( tmpValues && (isAll(tags, type, tmpValues) == true || tags[type].name == _DW_TAG_STRUCTURE_TYPE || tags[type].name == _DW_TAG_SUBPROGRAM || tags[type].name == _DW_TAG_POINTER_TYPE) )
                    {

                        if( tags[type].name == _DW_TAG_NAMESPACE && elements.check(tmpValues[_AT_NAME] + "::") == true )
                        {
                            bool isOk = true;
                            for(unsigned int i=0; i < this->types.size(); i++)
                                if( tmpValues[_AT_NAME] == this->types[i]->getName() )
                                {
                                    isOk = false;
                                    setNamespaceSibling(this->types[i], tmpValues[_AT_SIBLING]);
                                    activeNamespace = i;
                                    break;
                                }

                            if( isOk == true )
                            {
                                this->types.push_back( new Namespace(NULL, this, tmpValues[_AT_NAME], tmpValues[_AT_SIBLING]) );
                                activeNamespace = this->types.size() - 1;

                                //for functions (structural programming)
                                this->types[activeNamespace]->getTypes()->push_back( new Structure(this->types[activeNamespace], _NO_STRUCTURE) );
                            }
                            activeStructure = -1;
                        }
                        else
                        if( tags[type].name == _DW_TAG_STRUCTURE_TYPE )
                        {
                            structureNr = nr;
                            firstStructure = true;

                            if( tmpValues[_AT_SPECIFICATION] != "" && tmpValues[_AT_SIBLING] != "" )
                            {
                                activeStructure = -1; bool end = false;
                                std::vector<Namespace *>::iterator ns; int s = 0;
                                for(ns = this->types.begin(); ns < this->types.end(); ns++, s++)
                                {
                                    std::vector<Structure *>::iterator is; int n = 0;
                                    for(is = (*ns)->getTypes()->begin(); is < (*ns)->getTypes()->end(); is++, n++)
                                    {
                                        if( getStructureValue(*is) == tmpValues[_AT_SPECIFICATION] )
                                        {
                                            activeStructure = n;
                                            oldActiveNamespace = activeNamespace; activeNamespace = s;
                                            setStructureValues(*is, tmpValues[_AT_SIBLING], tags[type].value);
                                            setStructureByteSize(*is, (unsigned int)atoi(tmpValues[_AT_STRUCTURE_BYTE_SIZE].c_str()));
                                            end = true;

                                            baseTypes.push_back( new BaseType((*this->types[activeNamespace]->getTypes())[activeStructure]->getName(), tags[type].value, this->types[activeNamespace]->getName(), 0, _NO_POINTER, _NO_CONST) );

                                            std::vector<Structure *>::iterator is2; int n2 = 0;
                                            for(is2 = (*ns)->getTypes()->begin(); is2 < (*ns)->getTypes()->end(); is2++, n2++)
                                            {
                                                if( is2 != is && (*is2)->getName() == (*is)->getName() )
                                                {
                                                    (*ns)->getTypes()->erase(is);

                                                    setStructureValues(*is2, tmpValues[_AT_SIBLING], tags[type].value);//you must know when change namespace to old | if this structure is end
                                                    setStructureByteSize(*is2, (unsigned int)atoi(tmpValues[_AT_STRUCTURE_BYTE_SIZE].c_str()));
                                                    activeStructure = n2;

                                                    break;
                                                }
                                            }

                                            break;
                                        }
                                    }

                                    if( end == true )
                                        break;
                                }

                                if( end == false )
                                {
                                    std::map<Structure *, unsigned int>::iterator is; int n = 0;
                                    for(is = tmpStructures.begin(); is != tmpStructures.end(); is++, n++)
                                        if( getStructureValue(is->first) == tmpValues[_AT_SPECIFICATION] )
                                        {
                                            baseTypes.push_back( new BaseType(is->first->getName(), tags[type].value, this->types[is->second]->getName(), 0, _NO_POINTER, _NO_CONST) );

                                            is->first->~Structure();
                                            tmpStructures.erase(is);

                                            break;
                                        }
                                }

                                activeNr = nr;
                            }
                            else
                            {
                                std::string checkValue = (types[activeNamespace]->getName() == "" ? "" : (types[activeNamespace]->getName() + "::")) + tmpValues[_AT_NAME];

                                if( tmpValues[_AT_NAME] != "" && elements.check(checkValue) == true )
                                {
                                    baseTypes.push_back( new BaseType(tmpValues[_AT_NAME], tags[type].value, this->types[activeNamespace]->getName(), 0, _NO_POINTER, _NO_CONST) );

                                    if( tmpValues[_AT_SIBLING] == "" )//activeNr not change in this
                                        structureNr = -1;

                                    bool end = false;
                                    std::vector<Structure *>::iterator is; int n = 0;
                                    for(is = this->types[activeNamespace]->getTypes()->begin(); is < this->types[activeNamespace]->getTypes()->end(); is++, n++)
                                    {
                                       if( (*is)->getName() == tmpValues[_AT_NAME] )
                                       {
                                           end = true;
                                           if( tmpValues[_AT_SIBLING] != "" )
                                           {
                                                activeStructure = n; activeNr = nr;
                                           }
                                           break;
                                       }
                                    }

                                    if( end == false )
                                    {
                                        if( tmpValues[_AT_SIBLING] != "" )
                                            this->types[activeNamespace]->getTypes()->push_back( new Structure(this->types[activeNamespace], tmpValues[_AT_NAME], tags[type].value, tmpValues[_AT_SIBLING] ) );
                                        else
                                            this->types[activeNamespace]->getTypes()->push_back( new Structure(this->types[activeNamespace], tmpValues[_AT_NAME], tags[type].value) );

                                        activeStructure = (*this->types[activeNamespace]->getTypes()).size() - 1;
                                        setStructureByteSize((*this->types[activeNamespace]->getTypes())[activeStructure], (unsigned int)atoi(tmpValues[_AT_STRUCTURE_BYTE_SIZE].c_str()));
                                        activeNr = nr;
                                    }
                                    else
                                       tmpStructures.insert( std::pair<Structure *, unsigned int>(new Structure(this->types[activeNamespace], tmpValues[_AT_NAME], tags[type].value, tmpValues[_AT_SIBLING] ), activeNamespace) );
                                }
                                else{
                                    activeNr = nr;}
                            }
                        }
                        else
                        if( activeStructure > 0 && tags[type].name == _DW_TAG_INHERITANCE )
                        {
                            (*this->types[activeNamespace]->getTypes())[activeStructure]->getInheritances()->push_back(
                             new Inheritance((*this->types[activeNamespace]->getTypes())[activeStructure], tmpValues[_AT_NAME], (unsigned int)atoi(tmpValues[_AT_DATA_MEMBER_LOCATION].c_str() )));
                        }
                        else
                        if( activeStructure > 0 && tags[type].name == _DW_TAG_MEMBER )
                        {
                            bool isMember = false;
                            for(std::vector<Member *>::iterator member = (*this->types[activeNamespace]->getTypes())[activeStructure]->getMembers()->begin(); member < (*this->types[activeNamespace]->getTypes())[activeStructure]->getMembers()->end(); member++)
                                if( (*member)->getName() == tmpValues[_AT_NAME] )
                                {
                                    isMember = true;
                                    break;
                                }

                            if( isMember == false )
                                (*this->types[activeNamespace]->getTypes())[activeStructure]->getMembers()->push_back(
                                 new Member((*this->types[activeNamespace]->getTypes())[activeStructure], tmpValues[_AT_NAME], tmpValues[_AT_TYPE], (unsigned int)atoi(tmpValues[_AT_ACCESSIBILITY].c_str()), (unsigned int)atoi(tmpValues[_AT_OP_PLUS_UCONST].c_str())) );
                        }
                        else
                        if( tags[type].name == _DW_TAG_SUBPROGRAM )
                        {
                            if( tmpValues[_AT_NAME] != "" && (tmpValues[_AT_MIPS_LINKAGE_NAME] == "" || elements.check( getDemangle(tmpValues[_AT_MIPS_LINKAGE_NAME]) ) == true) )
                            {
                                if( activeStructure > 0 )
                                {
                                    //FIXME constructors <- but first fix namespaces in namespaces

                                    if( tmpValues[_AT_MIPS_LINKAGE_NAME] == "" )//constructor, destructor -> no type, no mangle
                                    {
                                        std::stringstream tmp;
                                        if( this->types[activeNamespace]->getName() != "" )
                                            tmp << this->types[activeNamespace]->getName().length();
                                        tmp << this->types[activeNamespace]->getName();
                                        tmp << (*this->types[activeNamespace]->getTypes())[activeStructure]->getName().length();

                                        if( tmpValues[_AT_NAME][0] == _SPECIAL_DESTRUCTOR && tmpValues[_AT_NAME].substr(1) == (*this->types[activeNamespace]->getTypes())[activeStructure]->getName() )
                                            (*this->types[activeNamespace]->getTypes())[activeStructure]->getMethods()->push_back(
                                             new Method((*this->types[activeNamespace]->getTypes())[activeStructure], tmpValues[_AT_NAME], _SPECIAL_MANGLE_BEGIN + tmp.str() + tmpValues[_AT_NAME].substr(1) + _MANGLE_DESTRUCTOR, _NO_TYPE, (unsigned int)0, _DESTRUCTOR) );
                                        else
                                        if( tmpValues[_AT_NAME] == (*this->types[activeNamespace]->getTypes())[activeStructure]->getName() )
                                            (*this->types[activeNamespace]->getTypes())[activeStructure]->getMethods()->push_back(
                                             new Method((*this->types[activeNamespace]->getTypes())[activeStructure], tmpValues[_AT_NAME], _SPECIAL_MANGLE_BEGIN + tmp.str() + tmpValues[_AT_NAME] + _MANGLE_CONSTRUCTOR, _NO_TYPE, (unsigned int)0, _CONSTRUCTOR) );
                                    }
                                    else
                                    {
                                        std::vector<Method *>::iterator method; bool isMethod = false;
                                        for(method = (*this->types[activeNamespace]->getTypes())[activeStructure]->getMethods()->begin(); method < (*this->types[activeNamespace]->getTypes())[activeStructure]->getMethods()->end(); method++)
                                            if( (*method)->getMangle() == tmpValues[_AT_MIPS_LINKAGE_NAME] )
                                            {
                                                isMethod = true;
                                                break;
                                            }

                                        if( isMethod == false )
                                        {
                                            if( tmpValues[_AT_TYPE] == "" )//void method
                                                (*this->types[activeNamespace]->getTypes())[activeStructure]->getMethods()->push_back(
                                                 new Method((*this->types[activeNamespace]->getTypes())[activeStructure], tmpValues[_AT_NAME], tmpValues[_AT_MIPS_LINKAGE_NAME], _VOID_TYPE, (unsigned int)atoi(tmpValues[_AT_ACCESSIBILITY].c_str()), _METHOD) );
                                            else
                                                (*this->types[activeNamespace]->getTypes())[activeStructure]->getMethods()->push_back(
                                                 new Method((*this->types[activeNamespace]->getTypes())[activeStructure], tmpValues[_AT_NAME], tmpValues[_AT_MIPS_LINKAGE_NAME], tmpValues[_AT_TYPE], (unsigned int)atoi(tmpValues[_AT_ACCESSIBILITY].c_str()), _METHOD) );
                                        }
                                    }
                                }
                                else
                                if( tmpValues[_AT_MIPS_LINKAGE_NAME] != "" && structureNr == -1 )
                                {
                                        std::vector<Method *>::iterator method; bool isMethod = false;
                                        for(method = (*this->types[activeNamespace]->getTypes())[0]->getMethods()->begin(); method < (*this->types[activeNamespace]->getTypes())[0]->getMethods()->end(); method++)
                                            if( (*method)->getMangle() == tmpValues[_AT_MIPS_LINKAGE_NAME] )
                                            {
                                                isMethod = true;
                                                break;
                                            }

                                        if( isMethod == false )
                                        {
                                            if( tmpValues[_AT_TYPE] == "" )//void method
                                                (*this->types[activeNamespace]->getTypes())[0]->getMethods()->push_back(
                                                 new Method((*this->types[activeNamespace]->getTypes())[0], tmpValues[_AT_NAME], tmpValues[_AT_MIPS_LINKAGE_NAME], _VOID_TYPE, (unsigned int)atoi(tmpValues[_AT_ACCESSIBILITY].c_str()), _METHOD) );
                                            else
                                                (*this->types[activeNamespace]->getTypes())[0]->getMethods()->push_back(
                                                 new Method((*this->types[activeNamespace]->getTypes())[0], tmpValues[_AT_NAME], tmpValues[_AT_MIPS_LINKAGE_NAME], tmpValues[_AT_TYPE], (unsigned int)atoi(tmpValues[_AT_ACCESSIBILITY].c_str()), _METHOD) );
                                        }
                                }
                            }
                        }
                        else
                        if( tags[type].name == _DW_TAG_BASE_TYPE )
                        {
                            baseTypes.push_back( new BaseType(tmpValues[_AT_NAME], tags[type].value, "", (unsigned int)atoi(tmpValues[_AT_BYTE_SIZE].c_str()), _NO_POINTER, _NO_CONST) );
                        }
                        else
                        if( tags[type].name == _DW_TAG_POINTER_TYPE || tags[type].name == _DW_TAG_ARRAY_TYPE )
                        {
                            if( tmpValues[_AT_POINTER_TYPE] == "" )
                                baseTypes.push_back( new BaseType(_VOID_TYPE, tags[type].value, "", 0, _IS_POINTER, _NO_CONST) );
                            else
                                baseTypes.push_back( new BaseType(tmpValues[_AT_POINTER_TYPE], tags[type].value, "", 0, _IS_POINTER, _NO_CONST) );
                        }
                        else
                        if( tags[type].name == _DW_TAG_CONST_TYPE )
                        {
                            baseTypes.push_back( new BaseType(tmpValues[_AT_POINTER_TYPE], tags[type].value, "", 0, _NO_POINTER, _IS_CONST) );
                        }
                        else
                        if( tags[type].name == _DW_TAG_ENUMERATION_TYPE || tags[type].name == _DW_TAG_UNION_TYPE )
                        {
                            baseTypes.push_back( new BaseType(tmpValues[_AT_NAME], tags[type].value, "", 0, _NO_POINTER, _NO_CONST) );
                        }
                        else
                        if(
                            tags[type].name == _DW_TAG_TYPEDEF          ||
                            tags[type].name == _DW_TAG_REFERENCE_TYPE   ||
                            tags[type].name == _DW_TAG_SUBROUTINE_TYPE  ||
                            tags[type].name == _DW_TAG_SUBRANGE_TYPE
                          )
                        {
                            baseTypes.push_back( new BaseType(tmpValues[_AT_POINTER_TYPE], tags[type].value, "", 0, _NO_POINTER, _NO_CONST) );
                        }

                    delete[] tmpValues; tmpValues = NULL;
                }

                if( (structureNr > 0 && nr < structureNr) || (firstStructure == false && activeNr == nr && nr == 1) )
                    structureNr = -1;

                size_t found;
                if( activeNamespace > 0 )
                {
                    found = parse.find(">" + getNamespaceSibling(this->types[activeNamespace]) + ":");
                    if( found != std::string::npos )
                    {
                        activeNamespace = 0;
                        activeStructure = -1;
                    }
                }

                if( activeStructure > 0 )
                {
                    if( getStructureSibling((*this->types[activeNamespace]->getTypes())[activeStructure]) != "" || (activeNr != -1 && nr < activeNr) || (firstStructure == false && activeNr == nr && nr == 1) )
                    {
                        found = parse.find(">" + getStructureSibling((*this->types[activeNamespace]->getTypes())[activeStructure]) + ":");
                        if( found != std::string::npos || (activeNr != -1 && nr < activeNr) || (firstStructure == false && activeNr == nr && nr == 1) )
                        {
                            activeStructure = -1;
                            if( oldActiveNamespace != -1 )
                                { activeNamespace = oldActiveNamespace; oldActiveNamespace = -1; }
                            activeNr = -1;
                            structureNr = -1;
                        }
                    }
                }

                firstStructure = false;

                type = 0;
                for(unsigned int i=0; i < tags.size(); i++)
                {
                    std::string tmpTagName;
                    if( (tmpTagName = getParseValue(parse, tags[i].name)) != _EMPTY_LINE )
                    {
                        tags[i].value = tmpTagName;

                        type = i;
                        isSub = true;
                        tmpValues = new std::string[tags[type].tags.size()];

                        for(unsigned int i=0; i < tags[type].tags.size(); i++)
                            if( tags[type].tags[i] == _DW_AT_ACCESSIBILITY )
                            {
                                std::stringstream tmp;
                                tmp << _PUBLIC;
                                tmpValues[i] = tmp.str();
                            }
                            else
                                tmpValues[i] = _EMPTY_LINE;

                        break;
                    }
                }

                if( isSub == true && parse[1] != ' ' )
                    nr = getLineNr(parse);
            }
        }

        if( line )
        {
            free(line);
        }

        if( fp )
        {
            pclose(fp);
        }
    }
    catch(std::exception &e)
    {
        if( fp )
        {
            pclose(fp);
        }

        throw Failure(_PARSE_ERROR + name + " - " + e.what());
    }
}

std::vector<Method*> Reflection::loadMethods(const std::string& name)
{
    FILE *fp = popen( (std::string(_NM) + " " + name).c_str(), "r");
    if( !fp )
    {
        throw Failure( _IO_ERROR + name );
    }

    char *line = NULL;
    size_t len = 0;
    std::vector<Method*> result;

    try
    {
        long long value;
        while( (getline(&line, &len, fp) ) != -1 )
        {
            value = 0;
            strtok(line, " ");
            value = (long long)strtol(("0x" + std::string(line)).c_str(), NULL, 0);//convert to hex

            if( value > 0 )
            {
                strtok(NULL, " ");
                result.push_back( new Method(std::string(strtok(NULL, "\r\n")), value) );
            }
        }

        if( line )
        {
            free(line);
        }

        if( fp )
        {
            pclose(fp);
        }
    }
    catch(...)
    {
        if( line )
        {
            free(line);
        }

        if( fp )
        {
            pclose(fp);
        }

        throw Failure(_IO_ERROR + name);
    }

    return result;
}

std::vector<Namespace *> *Reflection::getTypes()
{
    return &this->types;
}

std::string Reflection::toString()
{
    return this->toString(Reflection::_SHOW_ALL);
}

std::string Reflection::toString(tshow type)
{
    std::stringstream result;

    std::vector<Namespace *>::iterator _namespace;
    for(_namespace = this->getTypes()->begin(); _namespace < this->getTypes()->end(); _namespace++)
    {
        result << (*_namespace)->getName() << std::endl;

        std::vector<Structure *>::iterator structure;
        for(structure = (*_namespace)->getTypes()->begin(); structure < (*_namespace)->getTypes()->end(); structure++)
        {
            result << "\t" << (*structure)->getName();
            if( type & _SHOW_SIZE )
                result << " <" << (*structure)->getByteSize() << ">";
            result << std::endl;

            if( (*structure)->getInheritances()->size() > 0 )
            {
                if( type & _SHOW_TEXT )
                    result << "\t\t\t" << "Inheritances:";
                result << std::endl;

                std::vector<Inheritance *>::iterator inh;
                for(inh = (*structure)->getInheritances()->begin(); inh < (*structure)->getInheritances()->end(); inh++)
                {
                      result << "\t\t\t\t" << (*inh)->getName();
                      if( type & _SHOW_OFFSET )
                        result << " [" << (*inh)->getOffset() << "]";
                      result << std::endl;
                }
            }

            if( (*structure)->getMembers()->size() > 0 )
            {
                std::vector<Member *>::iterator member;
                for(member = (*structure)->getMembers()->begin(); member < (*structure)->getMembers()->end(); member++)
                {
                    result << "\t\t\t";

                    switch( (*member)->getAccessibility() )
                    {
                    case _PUBLIC:
                        result << "public "; break;

                    case _PROTECTED:
                        result << "protected "; break;

                    case _PRIVATE:
                        result << "private "; break;
                    }

                    if( (*member)->getConstType() == _IS_CONST )
                        result << "const ";

                    result << (*member)->getType() << " ";

                    if( (*member)->getPointerType() == _IS_POINTER )
                        result << "*";

                    result << (*member)->getName();
                    if( type & _SHOW_OFFSET )
                        result << " [" << (*member)->getOffset() << "]";
                    if( type & _SHOW_SIZE )
                        result << " <" << (*member)->getByteSize() << ">";
                    result << std::endl;
                }
            }

            if( (*structure)->getMethods()->size() > 0 )
            {
                std::vector<Method *>::iterator method;
                for(method = (*structure)->getMethods()->begin(); method < (*structure)->getMethods()->end(); method++)
                {
                    result << "\t\t\t";

                    switch( (*method)->getAccessibility() )
                    {
                    case _PUBLIC:
                        result << "public "; break;

                    case _PROTECTED:
                        result << "protected "; break;

                    case _PRIVATE:
                        result << "private "; break;
                    }

                    result << (*method)->getReturnType() << " ";
                    if( (*method)->getReturnPointerType() == _IS_POINTER )
                        result << "*";

                    result << (*method)->getName() ;
                    if( type & _SHOW_POINTER )
                        result << " [" << (*method)->getPtr() << "]";
                    result << std::endl;

                    if( type & _SHOW_PARAMETER )
                        for(unsigned int i=0; i < (*method)->getParameters()->size(); i++)
                            result << "\t\t\t\t" << i+1 << ". " << (*(*method)->getParameters())[i] << std::endl;
                }
            }

            result << std::endl;
        }

        result << std::endl;
    }

    return result.str();
}

std::string getDemangle(const std::string &mangle)
{
    return __cxxabiv1::__cxa_demangle(mangle.c_str(), 0, 0, 0);
}

std::string getShortType(std::string type)
{
    if( Reflection::shortMode == true )
    {
        std::string result = "";

        for(std::string::iterator it=type.begin(); it != type.end(); it++)
            if( !( *it > 0 && ( ((*(it-1)) == ',' && (*it) == ' ' ) || ( (*(it-1)) == '<' && (*it) == ' ' ) || ( (*(it-1)) == '>' && (*it) == ' ' ) ) ) )
                result += *it;

        result = replaceAll(result, _STD_SS, _STD_STRING_SS);
        result = replaceAll(result, _STD_SI, _STD_STRING_SI);
        result = replaceAll(result, _STD_SO, _STD_STRING_SO);
        result = replaceAll(result, _STD_SD, _STD_STRING_SD);

        return result;
    }

    return type;
}

std::string trim(const std::string &text)
{
    const char* chars = "\n\t\v\f\r ";
    std::string result = text;

    result.erase( result.find_last_not_of(chars) + 1 );
    result.erase( 0, result.find_first_not_of(chars) );

    return result;
}

std::string replaceAll(const std::string &s, const std::string &f, const std::string &r)
{
    if( s.empty() || f.empty() || f == r || s.find(f) == std::string::npos )
        return s;

    std::ostringstream build_it;
    size_t i = 0;

    for(size_t pos; ( pos = s.find( f, i ) ) != std::string::npos;)
    {
        build_it.write( &s[i], pos - i );
        build_it << r;
        i = pos + f.size();
    }

    if( i != s.size() )
        build_it.write( &s[i], s.size() - i );

    return build_it.str();
}

std::string eraseChar(std::string txt, char c)
{
    std::string result = "";
    for(std::string::iterator it = txt.begin(); it != txt.end(); it++)
        if( *it != c )
            result += *it;

    return result;
}

}
