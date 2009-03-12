#include <iostream>
#include <sstream>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>

#include <tut/tut_reflection.hpp>
#include <tut/tut_function.hpp>
#include <tut/arch/x86/tut_extend.hpp>
#include <tut/arch/x86/tut_trace.hpp>

#if defined(TUT_PLUGIN)
#include <tut/tut_plugin.hpp>
#endif

#define _ARCH_X86   ///information about witch architecture is use

/**   
 * \brief X86 architecture for tut functions extend
 * @author Krzysztof Jusiak
 * @date 03/02/2009 
 */

#ifdef __cplusplus
extern "C"
{
    void __cyg_profile_func_enter(void *this_fn, void *call_site)
    __attribute__((no_instrument_function));
    
    void __cyg_profile_func_exit(void *this_fn, void *call_site)
    __attribute__((no_instrument_function));
}
#endif

namespace tut
{
unsigned int function_parameters::maxTableLength = 0;
std::vector<void *> functions_order::__got_functions_ptrs_on_enter;
std::vector<void *> functions_order::__got_functions_ptrs_on_exit;
std::vector<unsigned int> functions_order::__time_functions_execute;
void *functions_order::__start_registering = NULL;
void *functions_order::__end_registering = NULL;
std::vector<functions_body::set_function *> functions_body::__set_functions;
void *tut_functions::__show_parameters_function = (void *)0xaa;
void *tut_functions::__register_function;
bool tut_functions::__first = true;
void *tut_reflection::_this = NULL;
reflection::Reflection tut_reflection::reflect;
}

#if !defined(TUT_PLUGIN)
static bool __reg = false;                              ///if we are between start and end registering
static bool __no_instrument_function = true;            ///use to dont do all stuff with function which was called inside func_enter and func_exit
#else
static bool __no_instrument_function = false;           ///use to dont do all stuff with function which was called inside func_enter and func_exit
static bool __reg = true;                               ///if we are between start and end registering
static tut_plugin __plugin;                             ///plugin object
#endif

static unsigned long __function_order = 0;              ///every function which was called have order number
static bool __new_body_tackle_return = false;           ///if true then on function exit get what function return
static int *__new_body_tackle_return_value;             ///use to keep what function return (not float and double, they use coprocessor for that)
static void *__tmp_run = NULL;                          ///temporaray var to call new body
static bool __is_run = false;                           ///temporaray var to call new body
static double __double;                                 ///use to get float, double return value
static timespec __function_run_time;                    ///function executed time


#if defined(TUT_PLUGIN)
/** 
 * \brief debug output
 * this function is used to show local debug information
 * @param mode function enter or exit
 * @param name what will be writen as value header
 * @param text result to write
 */
void __debug_output(tut::function_parameters::tfmode mode, const std::string &name, const std::string &text, bool showParenthesis = true) {
    std::string complete = "";
    if( __plugin.options & tut_plugin::_OPTION_SHOW_NAMES )
        complete += "[" + name + "]";
    complete += ((showParenthesis == true || (showParenthesis == false && (__plugin.options & tut_plugin::_OPTION_SHOW_NAMES)) )
                ? "[" : "") + text + 
                ((showParenthesis == true || (showParenthesis == false && (__plugin.options & tut_plugin::_OPTION_SHOW_NAMES)) ) 
                ? "]" : "");

    if( ( (mode == tut::function_parameters::_FUNCTION_ENTER) && ((__plugin.options & tut_plugin::_OPTION_FUNC_MODE_ENTER) != 0 )) ||
        ( (mode == tut::function_parameters::_FUNCTION_EXIT) && ((__plugin.options & tut_plugin::_OPTION_FUNC_MODE_EXIT) != 0 )) || 
          !mode
      )
    {
        if( __plugin.options & tut_plugin::_OPTION_DEBUG_OUT_CERR )
            std::cerr << complete << std::endl;

        if( __plugin.options & tut_plugin::_OPTION_DEBUG_OUT_COUT )
            std::cout << complete << std::endl;
    }
}

/** 
 * \brief on first function
 * this function do all stuff which are in init() and init_reflection() functions
 */
void __on_first_function() {
    try {
        __plugin.init();
        __plugin.init_reflection(tut::tut_reflection::reflect);
        tut::tut_reflection::reflect.load();

        if( __plugin.debug & tut_plugin::_DEBUG_REFLECTION )
            __debug_output((tut::function_parameters::tfmode)0, _NAME_REFLECTION, tut::tut_reflection::reflect.toString());
    }
    catch(std::exception &e) {
        if( __plugin.options & tut_plugin::_OPTION_SHOW_WARNINGS )
            __debug_output(tut::function_parameters::_FUNCTION_ENTER, _NAME_WARNING, e.what() );
    }
}   
#endif

/** 
 * \brief on function enter
 * this function do all stuff before original function will be called
 * @param this_fn function which was executed
 * @param call_site function which from this_fn function was called
 */
void __cyg_profile_func_enter(void *this_fn, void *call_site)
{
    if( __is_run == true ) {
        __is_run = false;
        __no_instrument_function = false;
    }
    else
    if( this_fn == __tmp_run )
        __is_run = true;

    if( __no_instrument_function == true )
        return;

#if !defined(TUT_PLUGIN)
    if( this_fn == tut::functions_order::__end_registering ) {
    	__reg = false; __no_instrument_function = true; return;
    }
#endif
    __function_order++;

#if defined(TUT_PLUGIN)
    if( __function_order == 1 )
        __on_first_function();

    if( __plugin.debug & tut_plugin::_DEBUG_FUNCTIONS )
    {
        std::stringstream ptr; ptr << this_fn;
        if( __plugin.debug & tut_plugin::_DEBUG_FUNC_ORDER )
            ptr << "][" << __function_order;

        __debug_output(tut::function_parameters::_FUNCTION_ENTER, _NAME_FUNCTION, "enter: " + ptr.str());
    }
#endif
    if( __reg == true )
    {
        __no_instrument_function = true;

        reflection::Method *method = tut::tut_reflection::reflect.getMethodByPtr(reinterpret_cast<long long>(this_fn));
        if( method != NULL )
        {
            int *parameters_ptr; bool isMethod = (method->getStructure()->getName() != _NO_STRUCTURE);

            __x86_get_parameters__(parameters_ptr); //get whole functions parameters, with 'this'
            int *old_parameters_ptr = parameters_ptr;

            tut::tut_reflection::_this = NULL;
            if( isMethod == true )
            {
                tut::tut_reflection::_this = (void *)(*(parameters_ptr)); //set object 'this', you can use later get_this to get pointer to object
                *parameters_ptr++; //we don't want to write 'this' out
            }

            tut::function_parameters *func_parameters = new tut::function_parameters(method, tut::function_parameters::_FUNCTION_ENTER, __function_order);
            std::vector<std::string> *parameters = method->getParameters();

            if( parameters->size() > 0 )
            {
                for(std::vector<std::string>::iterator it = parameters->begin(); it < parameters->end(); it++)
                {
                    if( tut::tut_reflection::reflect.isClass(reflection::eraseChar(*it, '*') ) == NULL )
                        func_parameters->add_parameter(*it, parameters_ptr, _UNKNOWN_SIZE);
                    else
                    {
                        int *mem_ptr;
                        __x86_get_parameter_from_memory__(parameters_ptr - old_parameters_ptr, mem_ptr);
                        func_parameters->add_parameter(*it, mem_ptr, _UNKNOWN_SIZE);
                    }

                    if( *it == _TYPE_DOUBLE )
                        *parameters_ptr++;

                    *parameters_ptr++;
                }
            }

            #if !defined(TUT_PLUGIN)
                ((void (*)(tut::function_parameters *))tut::tut_functions::__register_function)(func_parameters);
                ((void (*)(tut::function_parameters *))tut::tut_functions::__show_parameters_function)(func_parameters);
            #else
                if( __plugin.debug & tut_plugin::_DEBUG_METHODS )
                {
                    std::stringstream ptr; ptr << method->getDemangle();
                    if( __plugin.debug & tut_plugin::_DEBUG_FUNC_ORDER )
                        ptr << "][" << __function_order;

                    __debug_output(tut::function_parameters::_FUNCTION_ENTER, _NAME_METHOD, "enter: " + ptr.str());
                }

                if( __plugin.debug & tut_plugin::_DEBUG_FUNC_PARAMETERS )
                    __debug_output(tut::function_parameters::_FUNCTION_ENTER, _NAME_FUNC_PARAMETER,
                        ((std::string (*)(tut::function_parameters *))tut::tut_functions::__show_parameters_function)(func_parameters), false);

                if( !(__plugin.options & tut_plugin::_OPTION_NO_SEND) )
                {
                    try {
                        __plugin.send(func_parameters);
                    }
                    catch(std::exception &e) {
                        if( __plugin.options & tut_plugin::_OPTION_SHOW_WARNINGS )
                            __debug_output(tut::function_parameters::_FUNCTION_ENTER, _NAME_WARNING, e.what() );
                    }
                }
            #endif

            if( func_parameters )
                delete func_parameters;

            void *newPtr = tut::functions_body::have_new_body(this_fn);

            if( newPtr != NULL || method->getReturnType() == _TYPE_FLOAT || method->getReturnType() == _TYPE_DOUBLE )
            {
                if( newPtr != NULL )
                    tut::functions_body::was_run(this_fn);
                else
                    newPtr = this_fn;

                if( method->getReturnType() == _TYPE_FLOAT || method->getReturnType() == _TYPE_DOUBLE )
                    __tmp_run = this_fn;

                std::vector<std::string> *parameters = reflection::Method::getParameters( reflection::Method::getDemangle(tut::tut_reflection::reflect.getFunctionByPtr(reinterpret_cast<long long>(newPtr))->getMangle()) );

                if( tut::tut_reflection::reflect.getMethodByPtr(reinterpret_cast<long long>(newPtr)) == NULL )
                    *old_parameters_ptr++; //if isn't method then we omit 'this' (static methods are functions)

                __x86_push_function_parameters__(parameters, *old_parameters_ptr);
                __x86_run_function__(newPtr);
                __x86_get_register_value__(__x86_eax__, __new_body_tackle_return_value);

                if( method->getReturnType() == _TYPE_FLOAT || method->getReturnType() == _TYPE_DOUBLE )
                    __x86_get_floating_value__();

                __new_body_tackle_return = true;
                __cyg_profile_func_exit(this_fn, call_site);
                __tmp_run = NULL;
                __x86_end_function__();
            }

            __function_run_time.tv_sec = 0;
            __function_run_time.tv_nsec = 0;
            clock_settime(CLOCK_PROCESS_CPUTIME_ID, &__function_run_time);
        }

        __no_instrument_function = false;
    }
}

/** 
 * \brief on function exit
  * this function do all stuff after original function was called
 * @param this_fn function which was executed
 * @param call_site function which from this_fn function was called
 */
void __cyg_profile_func_exit(void *this_fn, void *call_site)
{
#if !defined(TUT_PLUGIN)
    if( this_fn == tut::functions_order::__start_registering ) {
    	__reg = true; __no_instrument_function = false; return;
    }
#else
    if( __plugin.debug & tut_plugin::_DEBUG_FUNCTIONS )
    {
        std::stringstream ptr; ptr << this_fn;
        __debug_output(tut::function_parameters::_FUNCTION_EXIT, _NAME_FUNCTION, "exit: " + ptr.str());
    }
#endif

    if( (__no_instrument_function == true && __new_body_tackle_return == false) || (this_fn == __tmp_run && __new_body_tackle_return == false) )
        return;

    int *parameters_ptr; 
    __x86_get_register_value__(__x86_ebx__, parameters_ptr);

    if( __new_body_tackle_return == true )
    {
        parameters_ptr = __new_body_tackle_return_value;
        __new_body_tackle_return = false;
    }

    if( __reg == true )
    {
        __no_instrument_function = true;

        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &__function_run_time);

        tut::functions_order::__time_functions_execute.push_back(__function_run_time.tv_nsec / 1000000);
        reflection::Method *method = tut::tut_reflection::reflect.getMethodByPtr(reinterpret_cast<long long>(this_fn));
        std::string returnType;

        if( method != NULL )
        {
            tut::function_parameters *func_parameters = new tut::function_parameters(method, tut::function_parameters::_FUNCTION_EXIT, __function_order);

            if( method->getReturnType() != _TYPE_VOID && method->getReturnType().length() > 0 )
                if( tut::tut_reflection::reflect.isClass(reflection::eraseChar(method->getReturnType(), '*')) == NULL )
                {
                    if( method->getReturnType() == _TYPE_FLOAT || method->getReturnType() == _TYPE_DOUBLE )
                        func_parameters->add_parameter( method->getReturnType(), reinterpret_cast<void *>(&__double) );
                    else
                        func_parameters->add_parameter(method->getReturnType(), (int *)&parameters_ptr, _UNKNOWN_SIZE);
                }
                else
                    func_parameters->add_parameter(method->getReturnType(), parameters_ptr, _UNKNOWN_SIZE);

            #if !defined(TUT_PLUGIN)
                ((void (*)(tut::function_parameters *))tut::tut_functions::__register_function)(func_parameters);
                ((void (*)(tut::function_parameters *))tut::tut_functions::__show_parameters_function)(func_parameters);
            #else
                if( __plugin.debug & tut_plugin::_DEBUG_METHODS )
                    __debug_output(tut::function_parameters::_FUNCTION_EXIT, _NAME_METHOD, "exit: " + method->getDemangle());

                if( __plugin.debug & tut_plugin::_DEBUG_FUNC_PARAMETERS )
                    __debug_output(tut::function_parameters::_FUNCTION_EXIT, _NAME_FUNC_PARAMETER,
                        ((std::string (*)(tut::function_parameters *))tut::tut_functions::__show_parameters_function)(func_parameters), false);

                if( !(__plugin.options & tut_plugin::_OPTION_NO_SEND) )
                {
                    try {
                        __plugin.send(func_parameters);
                    }
                    catch(std::exception &e) {
                        if( __plugin.options & tut_plugin::_OPTION_SHOW_WARNINGS )
                            __debug_output(tut::function_parameters::_FUNCTION_EXIT, _NAME_WARNING, e.what() );
                    }
                }
            #endif

            if( func_parameters )
                delete func_parameters;
        }

        __no_instrument_function = false;
    }
}
