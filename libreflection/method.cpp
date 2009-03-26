#include <reflection/method.h>
#include <cxxabi.h>

namespace reflection {

void *Call::ptr = NULL;

Method::Method(Structure *structure, const std::string &name, const std::string &mangle, const std::string &returnType, long long ptr, unsigned int accessibility) :
    structure(structure), name(name), mangle(mangle), returnType(returnType), ptr(ptr), accessibility(accessibility), type(_METHOD)
{}

Method::Method(Structure *structure, const std::string &name, const std::string &mangle, const std::string &returnType, unsigned int accessibility, unsigned short type) :
    structure(structure), name(name), mangle(mangle), returnType(returnType), ptr(0x00), accessibility(accessibility), type(type)
{}

Method::Method(Structure *structure, const std::string &name, const std::string &mangle, const std::string &returnType) :
    structure(structure), name(name), mangle(mangle), returnType(returnType), ptr(0x00), accessibility(0x01), type(_METHOD)
{}

Method::Method(const std::string &mangle, long long ptr) :
    structure(NULL), name(""), mangle(mangle), returnType(""), ptr(ptr), accessibility(0x01), type(_METHOD)
{}

Method::Method() :
    structure(NULL), name(""), mangle(""), returnType(""), ptr(0x00), accessibility(0x01), type(_METHOD)
{}

void Method::setType()
{
    for(std::vector<Method *>::iterator pointer = this->getClass()->getNamespace()->getReflection()->methodsPointers.begin(); pointer < this->getClass()->getNamespace()->getReflection()->methodsPointers.end(); pointer++)
    {
        if( (*pointer)->getMangle() == this->mangle ||
          ( this->mangle[0] == _SPECIAL_MANGLE && this->mangle.length()-1 <= (*pointer)->getMangle().length() && (*pointer)->getMangle().substr(0, this->mangle.length()-1) == this->mangle.substr(1) ) )
        {
            this->ptr = (*pointer)->getPtr();

            if( this->mangle[0] == _SPECIAL_MANGLE )
            {
                this->mangle = (*pointer)->getMangle();
                setMethodName(*pointer, this->name);
            }

            break;
        }
    }

    std::string tmp; bool isOk = false; int stop = 5; unsigned short pointerType = _NO_POINTER;
    tmp = this->returnType;

    if( tmp != _NO_TYPE )
    {
        do
        {
           unsigned int n;
           for(n=0; n < this->getClass()->getNamespace()->getReflection()->baseTypes.size(); n++)
           if( tmp  == this->getClass()->getNamespace()->getReflection()->baseTypes[n]->value )
           {
                if( (this->getClass()->getNamespace()->getReflection()->baseTypes[n]->pointerType == _NO_POINTER && this->getClass()->getNamespace()->getReflection()->baseTypes[n]->name[0] != '<') ||
                    (this->getClass()->getNamespace()->getReflection()->baseTypes[n]->pointerType == _IS_POINTER && this->getClass()->getNamespace()->getReflection()->baseTypes[n]->name == _VOID_TYPE)
                  )
                {
                    std::string value = "";
                    if( this->getClass()->getNamespace()->getReflection()->baseTypes[n]->ns != "" && this->getClass()->getNamespace()->getReflection()->baseTypes[n]->ns != this->getStructure()->getNamespace()->getName() )
                        value += this->getClass()->getNamespace()->getReflection()->baseTypes[n]->ns + "::";
                    value += this->getClass()->getNamespace()->getReflection()->baseTypes[n]->name;

                    if( this->getClass()->getNamespace()->getReflection()->baseTypes[n]->pointerType == _IS_POINTER && this->getClass()->getNamespace()->getReflection()->baseTypes[n]->name == _VOID_TYPE )
                        pointerType = _IS_POINTER;

                    this->returnType = value;
                    this->returnPointerType = pointerType;
                    isOk = true;
                }
                else
                {
                    tmp = this->getClass()->getNamespace()->getReflection()->baseTypes[n]->name;

                    if( this->getClass()->getNamespace()->getReflection()->baseTypes[n]->pointerType == _IS_POINTER )
                        pointerType = _IS_POINTER;
                }

                break;
           }

           if( n >= this->getClass()->getNamespace()->getReflection()->baseTypes.size() - 1 )
               break;

           stop--;
        }
        while( isOk == false && stop > 0 );
    }
}

std::string Method::getName()
{
    return this->name;
}

std::vector<std::string> *Method::getParameters(const std::string &demangle)
{
    std::vector<std::string> *result = new std::vector<std::string>();

    if( demangle != "" )
    {
        std::string param = "";

        bool ok = false; int par = 0;
        for(unsigned int i=0; i < demangle.length(); i++)
        {
            if( ok == true )
            {
                if( demangle[i] != '(' )
                {
                    if( demangle[i] == '<' )
                        par++;
                    else
                    if( demangle[i] == '>' )
                        par--;

                    if( demangle[i] == ',' && par == 0 )
                    {
                        if( param != "" )
                        {
                            result->push_back( getShortType(trim(param)) );
                            param = "";
                        }
                    }
                    else
                        param += demangle[i];
                }
                else
                    param += demangle[i];
            }

            if( demangle[i] == '(' && i+1 < demangle.length() && demangle[i+1] != ')' )
                ok = true;
        }

        if( param != "" )
        {
            size_t found = param.find_last_of(')');

            if( found != std::string::npos )
                result->push_back( getShortType(trim(param.substr(0, found))) );
        }
    }

    return result;
}

std::vector<std::string> *Method::getParameters()
{
    return  Method::getParameters(getDemangle());
}

std::string Method::getMangle()
{
    if( this->getClass() != NULL && this->mangle[0] == _SPECIAL_MANGLE )
        this->setType();

    if( this->mangle[0] == _SPECIAL_MANGLE )
        return _NO_TYPE;

    return this->mangle;
}

std::string Method::getDemangle(const std::string &mangle)
{
    try
    {
        return mangle[0] != _SPECIAL_MANGLE ? __cxxabiv1::__cxa_demangle(mangle.c_str(), 0, 0, 0) : _NO_TYPE;
    }
    catch(...)
    {
        return _NO_TYPE;
    }
}

std::string Method::getDemangle()
{
    return Method::getDemangle(this->mangle);
}

std::string Method::getFullReturnType()
{
    if( !this->ptr )
        this->setType();

    return
        this->returnType[0] == '<' ? _UNKNOWN_TYPE : this->returnType;
}

std::string Method::getReturnType()
{
    if( this->getClass() != NULL && !this->ptr )
        this->setType();

    return
        this->returnType[0] == '<' ? _UNKNOWN_TYPE : getShortType(this->returnType);
}

unsigned short Method::getType()
{
    return this->type;
}

unsigned short Method::getReturnPointerType()
{
    if( this->getClass() != NULL && !this->ptr )
        this->setType();

    return returnPointerType;
}

long long Method::getPtr()
{
    if( this->getClass() != NULL && !this->ptr )
        this->setType();

    return this->ptr;
}

unsigned short Method::getAccessibility()
{
    return this->accessibility;
}

Structure *Method::getStructure()
{
    return structure;
}

Structure *Method::getClass()
{
    return getStructure();
}

void setMethodName(Method *method, const std::string &name)
{
    method->name = name;
}

}
