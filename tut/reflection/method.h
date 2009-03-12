#ifndef __METHOD_H
#define __METHOD_H

#include <iostream>
#include <vector>
#include <stdarg.h> 

#include "reflection.h"
#include "structure.h"

#define _METHOD         (unsigned short)0x00
#define _CONSTRUCTOR    (unsigned short)0x01
#define _DESTRUCTOR     (unsigned short)0x02

template<typename _CastTo, typename _CastFrom>
_CastTo union_cast(_CastFrom _value);

namespace reflection {

/**  
 * \brief Keep method values
 * @author Krzysztof Jusiak 
 * @date 03/02/2009 
 */
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

/**  
 * \brief Calling methods
 * @author Krzysztof Jusiak 
 * @date 03/02/2009 
 */
class Call
{
    static void *ptr;

public:
    Call(void *ptr) { Call::ptr = ptr; }

    static void init() {
        Call::ptr = NULL;
    }

    static void *invoke(...) {
        __builtin_return(
            __builtin_apply( reinterpret_cast<void (*)(...)>(Call::ptr), __builtin_apply_args(), 1024 ) );
    }

    template<class _ret>
    static _ret invoke(...) {
        __builtin_return(
            __builtin_apply( reinterpret_cast<void (*)(...)>(Call::ptr), __builtin_apply_args(), 1024 ) );
    }
};

#define _NOTHING 0

#define _invoke(function_ptr, ...) (new reflection::Call(function_ptr))->invoke(__VA_ARGS__)
#define _invokeSetReturn(function_ptr, returnType, ...) (new reflection::Call(function_ptr))->invoke<returnType>(__VA_ARGS__)

}

#endif
