#include <reflection/member.h>

namespace reflection {

Member::Member(Structure *structure, const std::string &name, const std::string &type, unsigned int accessibility, unsigned int offset) :
    structure(structure), name(name), type(type), accessibility(accessibility), offset(offset)
{}

Member::Member(Structure *structure, const std::string &name) :
    structure(structure), name(name), type(""), accessibility(0x01), offset(0x00), byteSize(0x00), pointerType(_NO_POINTER), constType(_NO_CONST)
{}

Member::Member() :
    structure(NULL), name(""), type(""), accessibility(0x01), offset(0x00), byteSize(0x00), pointerType(_NO_POINTER), constType(_NO_CONST)
{}

void Member::setType()
{
    std::string tmp; bool isOk = false; int stop = 10; unsigned short pointerType = _NO_POINTER; unsigned short constType = _NO_CONST;
    tmp = this->type;
    do
    {
        unsigned int n;
        for(n=0; n < this->getClass()->getNamespace()->getReflection()->baseTypes.size(); n++)
            if( tmp  == this->getClass()->getNamespace()->getReflection()->baseTypes[n]->value )
            {
                if( this->getClass()->getNamespace()->getReflection()->baseTypes[n]->pointerType == _NO_POINTER && this->getClass()->getNamespace()->getReflection()->baseTypes[n]->name[0] != '<' )
                {
                    std::string value = "";
                    if( this->getClass()->getNamespace()->getReflection()->baseTypes[n]->ns != "" && this->getClass()->getNamespace()->getReflection()->baseTypes[n]->ns != this->getStructure()->getNamespace()->getName() )
                        value += this->getClass()->getNamespace()->getReflection()->baseTypes[n]->ns + "::";
                    value += this->getClass()->getNamespace()->getReflection()->baseTypes[n]->name;

                    setValues(value, pointerType == _IS_POINTER ? _POINTER_SIZE : this->getClass()->getNamespace()->getReflection()->baseTypes[n]->byteSize, pointerType, constType);
                    isOk = true;
                }
                else
                {
                    tmp = this->getClass()->getNamespace()->getReflection()->baseTypes[n]->name;

                    if( this->getClass()->getNamespace()->getReflection()->baseTypes[n]->pointerType == _IS_POINTER )
                        pointerType = _IS_POINTER;

                    if( this->getClass()->getNamespace()->getReflection()->baseTypes[n]->constType == _IS_CONST )
                        constType = _IS_CONST;
                }

                break;
            }

            if( n >= this->getClass()->getNamespace()->getReflection()->baseTypes.size() - 1 )
                break;

            stop--;
    }
    while( isOk == false && stop > 0 );
}

std::string Member::getName()
{
    return this->name;
}

std::string Member::getFullType()
{
    if( this->type[0] == '<' )
        this->setType();

    return
        this->type[0] == '<' ? _UNKNOWN_TYPE : this->type;
}

std::string Member::getType()
{
    if( this->type[0] == '<' )
        this->setType();

    return
        this->type[0] == '<' ? _UNKNOWN_TYPE : getShortType(this->type);
}

unsigned int Member::getAccessibility()
{
    return this->accessibility;
}

unsigned int Member::getOffset()
{
    return this->offset;
}

unsigned int Member::getByteSize()
{
    if( this->type[0] == '<' )
        this->setType();

    return this->byteSize;
}

unsigned short Member::getPointerType()
{
    if( this->type[0] == '<' )
        this->setType();

    return this->pointerType;
}

unsigned short Member::getConstType()
{
    if( this->type[0] == '<' )
        this->setType();

    return this->constType;
}

Structure *Member::getStructure()
{
    return structure;
}

Structure *Member::getClass()
{
    return getStructure();
}

void Member::setValues(const std::string &type, unsigned int byteSize, unsigned short pointerType, unsigned short constType)
{
    this->type = type;
    this->byteSize = byteSize;
    this->pointerType = pointerType;
    this->constType = constType;
}

};
