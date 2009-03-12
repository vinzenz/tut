#ifndef TUT_H_REFLECTION
#define TUT_H_REFLECTION

#include <iostream>
#include <stdexcept>
#include <vector>
#include <string> 
#include <sstream>
#include <stdio.h>
#include <tut/tut_exception.hpp>
#include <tut/reflection/reflection.h>

namespace tut {

/** 
 * \brief TUT reflection
 * @author Krzysztof Jusiak
 * @date 03/02/2009
 */
class tut_reflection
{
    /**
     * macro to invoke function by her pointer with parameters
     * return void *, then you can use get_int, get_double, ... macros
     *
     * example:
     * int result = get_int( __invoke( get_pointer(&function), 3, 4) );
     */
    #define __invoke(function_ptr, ...)\
        ((tut::tut_reflection::reflect.getMethodByPtr(reinterpret_cast<long long>(function_ptr)))->getStructure()->getName() != _NO_STRUCTURE) ?\
            _invoke(function_ptr, tut::tut_reflection::_this, __VA_ARGS__) : _invoke(function_ptr, __VA_ARGS__)
                                                 
    /**
     * macro to invoke function by her pointer with parameters 
     * and define return type
     *
     * example:
     * int result = __invoke_set_return( get_pointer(&function), int, 3, 4);
     */
    #define __invoke_set_return(function_ptr, return_type, ...)\
        ((tut::tut_reflection::reflect.getMethodByPtr(reinterpret_cast<long long>(function_ptr)))->getStructure()->getName() != _NO_STRUCTURE) ?\
            _invokeSetReturn(function_ptr, return_type, tut::tut_reflection::_this, __VA_ARGS__) : _invokeSetReturn(function_ptr, return_type, __VA_ARGS__);
                                                 
    /**
     * macro to set value object values (values could be private etc)
     *
     * example:
     * MyObject *object = ...;
     * __set_value(object, "MyObject", "some_var", 23); //some_var is private int in MyObject class
     * if you define new body, then you can use get_this macro to object
     *
     */
    #define __set_value(object, class, member, value)\
        tut::tut_reflection::reflect.getClass(class)->getMember(member)->setValue(object, value);

    /**
     * macro to set value object values (values could be private etc)
     *
     * example:
     * MyObject *object = ...;
     * __set_value(object, "Namespace", "MyObject", "some_var", 23); //Namespace::MyObject::some_var
     *
     */
    #define __set_value_ns(object, namespace, class, member, value)\
        tut::tut_reflection::reflect.getNamespace(namespace)->getClass(class)->getMember(member)->setValue(object, value);
            
    /**
     * macro to get get object values (values could be private etc)
     *
     * example:
     * MyObject *object = ...;
     * __get_value(object, "MyObject", "some_var");
     */
    #define __get_value(object, class, member, value)\
        tut::tut_reflection::reflect.getClass(class)->getMember(member)->getValue(object);

    /**
     * macro to get get object values (values could be private etc)
     *
     * example:
     * MyObject *object = ...;
     * __get_value(object, "Namespace", "MyObject", "some_var"); //Namespace::MyObject::some_var
     */
    #define __get_value_ns(object, namespace, class, member, value)\
        tut::tut_reflection::reflect.getNamespace(namespace)->getClass(class)->getMember(member)->getValue(object);

    /**
     * macro it's pointer to object which method was actual run
     * you can use this in macro to set new body for function
     */
    #define get_this (tut::tut_reflection::_this)

public:
    static reflection::Reflection reflect;  ///keep tut reflection
    static void *_this;                     ///pointer to object 'this' (this->... => tut_reflection::_this->)

    /**
     * \brief get pointer to function
     * @param name function filename e.g. &tut::Class::method
     * @ref reflection reflection system
     * @return number of pointer to function
     */
    static long long get_function_ptr(const std::string& name, reflection::Reflection ref = tut::tut_reflection::reflect)
    {
        std::string tmpName = name;
        try
        {
            if( tmpName[0] == '&' )
                tmpName = tmpName.substr(1, tmpName.length() - 1);

            tmpName = reflection::eraseChar(tmpName, ' ');
            bool ok = false;
            std::vector<reflection::Method *>::iterator im;
            for(im = ref.methodsPointers.begin(); im < ref.methodsPointers.end(); im++)
            {
                if( (*im)->getPtr() > 0 )
                {
                    std::string tmp = reflection::eraseChar(reflection::Method::getDemangle((*im)->getMangle()), ' ');
                    if( tmp.length() < tmpName.length() ) //for siur is not the same
                        continue;

                    ok = true;
                    unsigned int i = 0;
                    for(i=0; i < tmpName.length(); i++)
                    {
                        if( tmpName[i] != tmp[i] )
                        {
                            ok = false;
                            break;
                        }
                    }
    
                    if( i < tmp.length() - 1 )
                        if( tmp[i] != '('  )
                            ok = false;
                    
                    if( ok == true )
                        break;
                }
            }

            if( ok == true )
                return (*im)->getPtr();
        }
        catch(std::exception &ex)
        {
            throw std::runtime_error( ("fatal error: exception in tut function: '" + std::string(__PRETTY_FUNCTION__) + "' " + ex.what()) );
        }

        //bad pointer to function | reason -> person who defined tests -> functions: get_private
        //bad pointer in get_pointer function should give compilation error
        throw no_pointer_found( ("pointer to function not found: '" + name + "' Do you allow this function ? -> reflect->allow(\"" + tmpName + "\");"));
    }

    /**
     * \brief get function name
     * @param decPointer number of function pointer e.g. reinterpret_cast<long long>(&tut::Class::method);
     * @ref reflection reflection system
     * @return function name string e.g. "tut::Class::method"
     */
    static std::string get_function_name(long long decPointer, reflection::Reflection ref = tut::tut_reflection::reflect)
    {
        try
        {
            for(std::vector<reflection::Method *>::iterator im = ref.methodsPointers.begin(); im < ref.methodsPointers.end(); im++)
                if( (*im)->getPtr() > 0 && (*im)->getPtr() == decPointer )
                    return reflection::Method::getDemangle( (*im)->getMangle() );
        }
        catch(std::exception &ex)
        {
            throw std::runtime_error( ("fatal error: exception in tut function: '" + std::string(__PRETTY_FUNCTION__) + "' " + ex.what()) );
        }

        std::stringstream ptr;
        ptr << decPointer;

        throw no_pointer_found( ("function to pointer not found: [" + ptr.str() + "]" ));
    }

};

}

#endif
