#ifndef TUT_H_STUB
#define TUT_H_STUB
 
#include <stdarg.h> 
#include <vector>
#include <map>
#include <string>
#include <iterator>
#include <typeinfo>

namespace tut
{

//TODO: don't work correctly for now !

/**
 * Struct descriptor of stubbed method
 * @author Maciek Kalwak
 */
template <class type>
class stub_method 
{
    type value;
    int method;
    void *function;

public:
    stub_method(int method, type value) : value(value), method(method) {}       
    stub_method(int method, void *function) : method(method), function(function) {}
       
    /**
     *  Method called in stub body
     */
    type stub_return()
    {
        return value; 
    }

    void *stub_function()
    {
        return function;     
    }

    int stub_type() { return method; }
};
 
/**
 * Static class which handle stub calling
 * @author Maciek Kalwak
 */
template <class type> 
class stub_call
{
    #define METHOD_VALUE 0
    #define METHOD_EXPRESSION 1
    #define METHOD_FUNCTION 2

public:
       /**
        * Vector holding values of functions used to stub methods returning
        * type specyfied in template
        */
       static std::map<std::string, stub_method<type>*> stubs;

       /**
        * Register stub method value
        */
       static void stub_register(const std::string &stub_name, type value) 
       {
              stubs[stub_name] = new stub_method<type>(METHOD_VALUE, value);
       }

       /**
        * Register stub method value
        */
       static void stub_register(const std::string &stub_name, void *function) 
       {
              stubs[stub_name] = new stub_method<type>(METHOD_FUNCTION, function);
       }

       /**
        * Unregister stub method value
        */
       static void stub_unregister() {};
};

}
//template <class type>
//std::map<std::string, stub_method<type>*> tut::stub_call<type>::stubs = std::map<std::string, stub_method<type>*>();

/**
 * Macro create stub of defined function. It call static function which find
 * declared values to return as function result.
 *
 * @type - type of returned value
 * @class - name of class
 */

#define stub(name, type, class, method,...) \
        type class::method(__VA_ARGS__) \
        { \
            switch( tut::stub_call<type>::stubs[name]->stub_type() ) {\
            case METHOD_VALUE:\
                return tut::stub_call<type>::stubs[name]->stub_return();\
            case METHOD_FUNCTION:\
                __invoke(tut::stub_call<type>::stubs[name]->stub_function(), type, __VA_ARGS__);\
            }\
        }
 
#endif
