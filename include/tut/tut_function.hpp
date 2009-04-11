#ifndef TUT_H_FUNCTIONS
#define TUT_H_FUNCTIONS

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <stdio.h>
#include <tut/tut_reflection.hpp>
#include <tut/tut_parameter.hpp>
#include <tut/tut_stub.hpp>

/**
 * macro to get pointer for any function
 * use this one when you want pointer to public method
 *
 * example:
 * get_pointer( &tut::class::my_method );
 */
#define get_pointer(function)\
    union_cast<void *>(function)

/**
 * macro to get pointer for any function
 * use this one when you want pointer to private method
 * or overloading method, ... (if get_pointer don't works)
 * you can use get_private for every function which
 * pointer you can get by get_pointer macro
 *
 * example:
 * get_private( &tut::class::my_method(int, double) );
 * get_private( &tut::class::my_method(int) );
 * get_private( &tut::class::my_method );
 */
#define get_private(function)\
    (void *)tut::tut_reflection::get_function_ptr(#function)

namespace tut {

/**
 * \brief Keep functions executions order
 * @author Krzysztof Jusiak
 * @date 03/02/2009
 */
class functions_order
{
public:
    enum tshow_mode {
        _SHOW_MODE_NAME                     = 0x01,
        _SHOW_MODE_POINTER                  = 0x02,
        _SHOW_MODE_TIME                     = 0x04,
        _SHOW_MODE_DEFAULT                  = (_SHOW_MODE_NAME | _SHOW_MODE_TIME),
        _SHOW_MODE_ALL                      = (_SHOW_MODE_NAME | _SHOW_MODE_POINTER | _SHOW_MODE_TIME)
    };

    #define _ANY_FUNCTION                   (void *)0
    #define _ANY_FUNCTIONS                  (void *)-1

    struct function_order {
        std::vector<void *> __expected_functions_ptrs_on_enter;     ///keep expected functions pointers on enter to function
        std::vector<void *> __expected_functions_ptrs_on_exit;      ///keep expected functions pointers on exit for function
    };

    static std::vector<void *> __got_functions_ptrs_on_enter;       ///keep registering functions pointers on enter to function
    static std::vector<void *> __got_functions_ptrs_on_exit;        ///keep expected functions pointers on exit for function
    static std::vector<unsigned int> __time_functions_execute;      ///keep time of functions execute
    static void *__start_registering;                               ///pointer to start registering function
    static void *__end_registering;                                 ///pointer to end registering function

private:
    std::vector<function_order *> order;                            ///keep user declared order (expected)
    tshow_mode showMode;                                            ///how to show results, see tshow_mode

    /**
     * \brief have declared any functions in expected functions
     * @param expBegin begin of vector to verify
     * @param exEnd end of vector to verify
     * @return true if have any functions
     */
    bool have_ANY_FUNCTIONS(std::vector<void *>::iterator expBegin, std::vector<void *>::iterator expEnd)
    {
        for(; expBegin != expEnd; expBegin++)
            if( *expBegin == _ANY_FUNCTIONS )
                return true;

        return false;
    }

    /**
     * \brief compare if vectors have 'the same' functions includes specially any functions
     * @param expBegin begin of expected vector to compare
     * @param exEnd end of expected vector to compare
     * @param expBegin begin of got vector to compare
     * @param exEnd end of got vector to compare
     * @return true if 'are the same'
     */
    bool compare(std::vector<void *>::iterator expBegin, std::vector<void *>::iterator expEnd,
		 std::vector<void *>::iterator gotBegin, std::vector<void *>::iterator gotEnd)
    {
        if( (expEnd - expBegin != gotEnd - gotBegin) && have_ANY_FUNCTIONS(expBegin, expEnd) == false )
    	    return false;
	
        bool anyState = false, first = true;
        for(; expBegin != expEnd, gotBegin != gotEnd; gotBegin++)
        {
            if( anyState == true )
                if( *gotBegin == *expBegin )
                    anyState = false;

            if( *expBegin == _ANY_FUNCTIONS )
            {
                anyState = true;
                expBegin++;
                if( first == false )
                    gotBegin--;
            }

            if( anyState == false )
            {
    	        if( *expBegin != *gotBegin && *expBegin != _ANY_FUNCTION )
    	        	return false;

                expBegin++; first = false;
            }
        }
	
        return !(anyState == true);
    }

public:
    functions_order(tshow_mode showMode = _SHOW_MODE_DEFAULT) : showMode(showMode)
    {
        functions_order::__got_functions_ptrs_on_enter.clear();
        functions_order::__got_functions_ptrs_on_exit.clear();
        functions_order::__time_functions_execute.clear();

        //set pointer to start and end registering function
        //for this reason system knew where start and end registering
    	add( get_pointer(&tut::functions_order::start_registering), function_parameters::_FUNCTION_START_REGISTERING );
 	    add( get_pointer(&tut::functions_order::end_registering), function_parameters::_FUNCTION_END_REGISTERING );

        order.push_back( new function_order() );
    }

    ~functions_order() {
        order.clear();
    }

    /**
     * \brief new series of functions order
     * this function create new series, thanks to that you can do simple regular expression
     */
    void new_series() {
        order.push_back( new function_order() );
    }
	
    /**
     * \brief add expected function
     * you can use here also any_function, any_functions
     * @param function pointer to function
     * @param mode registering mode (registergin begin or end of functions)
     */
    void add(void *function, function_parameters::tfmode mode = function_parameters::_FUNCTION_ENTER)
    {
        switch(mode)
        {
	    case function_parameters::_FUNCTION_ENTER:
            order[order.size()-1]->__expected_functions_ptrs_on_enter.push_back(function); break;
	
   	    case function_parameters::_FUNCTION_EXIT:
            order[order.size()-1]->__expected_functions_ptrs_on_exit.push_back(function); break;
		
   	    case function_parameters::_FUNCTION_START_REGISTERING:
            functions_order::__start_registering = function; break;
		
        case function_parameters::_FUNCTION_END_REGISTERING:
	        functions_order::__end_registering = function; break;
        }
    }

    /**
     * \brief overload method to add the same function couple times
     * you can use also here any_function, any_functions
     * @param function pointer to function
     * @param count how many times
     * @param mode registering mode (registergin begin or end of functions)
     */
    void add(void *function, unsigned int count, function_parameters::tfmode mode = function_parameters::_FUNCTION_ENTER)
    {
        for(unsigned int i=0; i < count; i++)
            add(function, mode);
    }

    /**
     * \brief start registering
     * this function have no body (sic), inform system where registering begin
     */
    void start_registering() {}

    /**
     * \brief end registering
     * this function have no body (sic), inform system where registering end
     */
    void end_registering() {}

    /**
     * \brief ensure order is correct
     * @param mode registering mode (registergin begin or end of functions)
     * throw failure exception on order is now correct
     */
    void ensure_order(function_parameters::tfmode mode = function_parameters::_FUNCTION_ENTER)
    {
    	std::vector<void *>::iterator expBegin;
	    std::vector<void *>::iterator expEnd;
    	std::vector<void *>::iterator gotBegin;
	    std::vector<void *>::iterator gotEnd;
  	    std::stringstream ss;
        bool isCorrect = false;

        for(std::vector<function_order *>::iterator it = order.begin(); it < order.end(); it++)
        {
        	if( mode == function_parameters::_FUNCTION_EXIT )
	        {
	            expBegin = (*it)->__expected_functions_ptrs_on_exit.begin();
        	    expEnd = (*it)->__expected_functions_ptrs_on_exit.end();
	            gotBegin = functions_order::__got_functions_ptrs_on_exit.begin();
	            gotEnd = functions_order::__got_functions_ptrs_on_exit.end();
        	}
	        else
    	    {
	            expBegin = (*it)->__expected_functions_ptrs_on_enter.begin();
        	    expEnd = (*it)->__expected_functions_ptrs_on_enter.end();
	            gotBegin = functions_order::__got_functions_ptrs_on_enter.begin();
	            gotEnd = functions_order::__got_functions_ptrs_on_enter.end();
        	}

    	    if( !compare(expBegin, expEnd, gotBegin, gotEnd) )
            {
    	        std::string exp, got;

    	        ss << (mode == function_parameters::_FUNCTION_ENTER ? "on enter to functions:" : "on exit from functions:");

        	    bool showExp = true;
	            bool showGot = true;
                unsigned int i = 0;
    	        while(true)
        	    {
	        	    if( expBegin == expEnd )
		                showExp = false;
		
            		if( gotBegin == gotEnd )
	    	            showGot = false;
	
            		if( showExp == false && showGot == false )
		                break;
            		else
	    	            ss << "\n";
	
            		ss << "\t\t\t";

                    exp = "";
		            if( showExp )
                        if( *expBegin == _ANY_FUNCTION || *expBegin == _ANY_FUNCTIONS )
                            exp = "...";
                        else
            			    exp = tut_reflection::get_function_name( reinterpret_cast<long long>(*expBegin) );

                    got = "";
	    	        if( showGot )
            			got = tut_reflection::get_function_name( reinterpret_cast<long long>(*gotBegin) );

    		        if( exp == "" || got == "" || (exp != got && exp != "...") )
            			ss << " ERROR | ";
            		else
		            	ss << "       | ";

                    if( (showMode & _SHOW_MODE_TIME) != 0 )
                    {
                        ss.width(8);
            		    if( showGot )
                             ss << __time_functions_execute[i] << " ms | ";
                        else
                             ss << "          - | ";
                    }

        	    	ss << "expected: ";

                    if( (showMode & _SHOW_MODE_NAME) != 0 )
	    	            if( showExp )
                    	    ss << exp;

                    if( (showMode & _SHOW_MODE_POINTER) != 0 )
        		        if( showExp )
                    	    ss << " [0x" << *expBegin << "]";
		
            		ss << " --> got: ";

                    if( (showMode & _SHOW_MODE_NAME) != 0 )
        		        if( showGot )
	        		        ss << got;

                    if( (showMode & _SHOW_MODE_POINTER) != 0 )
            	    	if( showGot )
            	    	    ss << " [0x" << *gotBegin << "]";

            		if( showExp )
    		            expBegin++;
		
            		if( showGot )
                    {
    	    	        gotBegin++; i++;
                    }
    	        }

                ss << std::endl;
   		    	ss << "\t\t\t        -------------";
                unsigned int sum = 0;
                for(unsigned int i=0; i < __time_functions_execute.size(); i++)
                    sum += __time_functions_execute[i];

                ss << std::endl;
   	    		ss << "\t\t\t         ";
                ss.width(8); ss << sum << " ms ";

                if( order.size() > 1 )
           			ss << "--------------------------------------";
                ss << std::endl;
    	    }
            else
            {
                isCorrect = true;
                break;
            }
        }

        if( isCorrect == false )
       	    throw failure(ss.str().c_str());
    }

    void set_show_mode(tshow_mode showMode)
    {
        this->showMode = showMode;
    }

    tshow_mode get_show_mode()
    {
        return showMode;
    }

    std::string to_string()
    {
   	    std::stringstream ss;
        unsigned int i = 1, nr;

        for(std::vector<function_order *>::iterator it = order.begin(); it < order.end(); it++, i++)
        {

            if( (*it)->__expected_functions_ptrs_on_enter.size() > 0 )
            {
                ss << "\n[" << i << "] -> begin functions" << std::endl;
                ss << "--------------------------" << std::endl;

                nr = 1;
                for(std::vector<void *>::iterator io = (*it)->__expected_functions_ptrs_on_enter.begin(); io < (*it)->__expected_functions_ptrs_on_enter.end(); io++, nr++)
                {
                    ss.width(3); ss << std::left << nr;
                    if( *io == _ANY_FUNCTION )
                        ss << ".";
                    else
                    if( *io == _ANY_FUNCTIONS )
                        ss << "...";
                    else
                        ss << tut_reflection::get_function_name( reinterpret_cast<long long>(*io) );
                    ss << std::endl;
                }
            }

            if( (*it)->__expected_functions_ptrs_on_exit.size() > 0 )
            {
                ss << "\n[" << i << "] -> end functions" << std::endl;
                ss << "--------------------------" << std::endl;

                nr = 1;
                for(std::vector<void *>::iterator io = (*it)->__expected_functions_ptrs_on_exit.begin(); io < (*it)->__expected_functions_ptrs_on_exit.end(); io++, nr++)
                {
                    ss.width(3); ss << std::left << nr;
                    if( *io == _ANY_FUNCTION )
                        ss << ".";
                    else
                    if( *io == _ANY_FUNCTIONS )
                        ss << "...";
                    else
                        ss << tut_reflection::get_function_name( reinterpret_cast<long long>(*io) );
                    ss << std::endl;

                }
            }
        }

        return ss.str();
    }
};

static void ensure_functions_order(functions_order *functions, function_parameters::tfmode mode = function_parameters::_FUNCTION_ENTER)
{
    functions->ensure_order(mode);
}


/**
 * \brief Keep functions new body (Function wrapper)
 * @author Krzysztof Jusiak
 * @date 03/02/2009
 */
class functions_body
{
    /**
     * macro to simple define new body function
     *
     * example:
     *
     *   __set_body(functions.body, min_param, get_pointer(&test_data::min_param),
     *       void min_param(int a, int b)
     *       {
     *           ...
     *       }
     *   );
     *
     * it's the same as define new function and then do functions_body->set( old_functoin, new_function );
     */
    #define __set_body(functions_body, name, old_function, new_function)\
        struct __TMP##name\
        {\
            static new_function\
        };\
        functions_body->set( old_function, get_pointer(&__TMP##name::name) )

    /**
     * macro to simple define new body function with numbers of time to use (after run number of times system use default function)
     *
     * example:
     *
     *   __set_body_times(functions.body, min_param, get_pointer(&test_data::min_param),
     *       void min_param(int a, int b)
     *       {
     *           ...
     *       }, 3
     *   );
     *
     * it's the same as define new function and then do functions_body->set( old_functoin, new_function, times );
     */
    #define __set_body_times(functions_body, name, old_function, new_function, times)\
        struct __TMP##name\
        {\
            static new_function\
        };\
        functions_body->set( old_function, get_pointer(&__TMP##name::name), times )

    /**
     * macro to simple define new input parameters, first of all you should define new body for that function
     *
     * example:
     *
     *   __set_body(functions.body, min_param, get_pointer(&test_data::min_param),
     *       void min_param(int a, int b)
     *       {
     *          __set_input_parameters( get_pointer(&test_data::min_param), void, 4, 3);
     *       }
     *   );
     */
    #define __set_input_parameters(function_ptr, return_type, ...)\
        return __invoke_set_return(function_ptr, return_type, __VA_ARGS__)

    /**
     * macro to simple define new return parameters, first of all you should define new body for that function
     *
     * example:
     *
     *   __set_body(functions.body, min_param, get_pointer(&test_data::min_param),
     *       int min_param(int a, int b)
     *       {
     *          __set__return_parameters( get_pointer(&test_data::min_param), int, 9);
     *       }
     *   );
     *
     * it's the same as:
     *
     *   __set_body(functions.body, min_param, get_pointer(&test_data::min_param),
     *       int min_param(int a, int b)
     *       {
     *          return __invoke_set_return( get_pointer(&test_data::min_param), int, 9);
     *          //you can you also use __invoke macro -> return void * -> and then convert
     *          //result to type you want by using get_int, get_double, ... macros
     *       }
     *   );
     */
    #define __set_return_parameters(function_ptr, return_type, ret, ...)\
        __invoke_set_return(function_ptr, return_type, __VA_ARGS__);\
        return ret

    enum {
        _INFINITY   = -1
    };

    class set_function
    {
        void *originalPtr;                                  ///original pointer
        void *newPtr;                                       ///new pointer
        unsigned int times;                                 ///how many times change body

    public:
        set_function(void *originalPtr, void *newPtr, unsigned int times) : originalPtr(originalPtr), newPtr(newPtr), times(times) {}
        set_function() : originalPtr(NULL), newPtr(NULL), times(0x00) {}

        void *get_original_ptr(){ return originalPtr; }
        void *get_new_ptr(){ return newPtr; }
        void init(void *originalPtr, void *newPtr, unsigned int times)
        {
            this->originalPtr = originalPtr;
            this->newPtr = newPtr;
            this->times = times;
        }
        unsigned int get_times(){ return times; }
        unsigned int dec_times(){ return (times > 0) ? --times : times; }
    };

public:
    static std::vector<set_function *> __set_functions;     ///keep pointers to new bodys for functions

    functions_body() {
        functions_body::__set_functions.clear();
    }

    /**
     * \brief set new body of function
     * this function set new function body, if was earlier set new body than will be executed last definied body
     * @param originalFunction pointer to function which body you want change
     * @param newFunction pointer to new function which will be executed instead of originalFunction
     * @param how many times change the function body, then function have normal body
     */
    void set(void *originalFunction, void *newFunction, unsigned int times = _INFINITY)
    {
        for(std::vector<set_function *>::iterator it = functions_body::__set_functions.begin(); it < functions_body::__set_functions.end(); it++)
            if( (*it)->get_original_ptr() == originalFunction )
            {
                (*it)->init( (*it)->get_original_ptr(), newFunction, times);
                return;
            }

        functions_body::__set_functions.push_back( new set_function(originalFunction, newFunction, times) );
    }

    /**
     * \brief unset new function body
     * this function cause that orignalFunction will be have normal body
     * @param originalFunction pointer to function which body you want change
     */
    bool unset(void *originalFunction)
    {
        for(std::vector<set_function *>::iterator it = functions_body::__set_functions.begin(); it < functions_body::__set_functions.end(); it++)
            if( (*it)->get_original_ptr() == originalFunction )
            {
                functions_body::__set_functions.erase(it);
                return true;
            }

        return false;
    }

    /**
     * \brief unset all functions new bodys
     * this functoin case that all functions will be have normal bodys
     */
    void unset_all() {
        functions_body::__set_functions.clear();
    }

    std::string to_string()
    {
   	    std::stringstream ss;

        for(std::vector<set_function *>::iterator it = functions_body::__set_functions.begin(); it < functions_body::__set_functions.end(); it++)
        {
   		    ss << tut_reflection::get_function_name( reinterpret_cast<long long>( (*it)->get_original_ptr() ) );
            ss << " -> ";
            ss << tut_reflection::get_function_name( reinterpret_cast<long long>( (*it)->get_new_ptr() ) );
        }

        return ss.str();
    }

    /**
     * \brief have new body
     * this function check if for this function was declared new body
     * @param originalFunction pointer to function which body you want change
     * @return NULL if not else pointer to function
     */
    static void *have_new_body(void *originalFunction)
    {
        for(std::vector<set_function *>::iterator it = functions_body::__set_functions.begin(); it < functions_body::__set_functions.end(); it++)
            if( (*it)->get_original_ptr() == originalFunction )
                return (*it)->get_new_ptr();

        return NULL;
    }

    /**
     * \brief function was run in system
     * this function check if originalFunction wa run in system
     * if yes then check if times of change function body is not 0 and can run next time normal body
     * @param originalFunction pointer to function which body you want change
     */
    static void was_run(void *originalFunction)
    {
        for(std::vector<set_function *>::iterator it = functions_body::__set_functions.begin(); it < functions_body::__set_functions.end(); it++)
            if( (*it)->get_original_ptr() == originalFunction && (*it)->dec_times() == 0 )
            {
                functions_body::__set_functions.erase(it);
                return;
            }
    }
};

/**
 * \brief Keep many functions orders
 * @author Krzysztof Jusiak
 * @date 03/02/2009
 */
class tut_functions
{
    static bool __first;                        ///keep information about reset defaults show, registering function at creating new object

public:
    #include <tut/tut_user_function.hpp>

    static void *__show_parameters_function;    ///pointer to show parameters function
    static void *__register_function;           ///pointer to register functoin

    functions_body *body;                       ///functions new bodys
    functions_order *order;                     ///functions order

    tut_functions() {
        body = new functions_body();
        order = new functions_order();

        if( __first == true )
        {
            tut_functions::set_show_parameters_function( get_pointer(&tut_user_functions::show_parameters_function_empty) );
            tut_functions::set_register_function( get_pointer(&tut_user_functions::register_function_all) );

            __first = false; //don't reset defaults function for every new object
        }
    }

    ~tut_functions() {
        if( body )
            delete body;

        if( order )
            delete order;
    }

    /**
     * \brief register function
     * this function is exececuted by __register_function and add register got function
     * @param function executed function
     * @param mode registering mode (registergin begin or end of functions)
     */
    static void register_function(long long function, function_parameters::tfmode mode = function_parameters::_FUNCTION_ENTER)
    {
        switch(mode)
        {
        case function_parameters::_FUNCTION_ENTER:
            functions_order::__got_functions_ptrs_on_enter.push_back( reinterpret_cast<void *>(function) ); break;

        case function_parameters::_FUNCTION_EXIT:
            functions_order::__got_functions_ptrs_on_exit.push_back( reinterpret_cast<void *>(function) ); break;

        default: break;
        }
    }

    static void set_show_parameters_function(void *function)
    {
        __show_parameters_function = function;
    }

    static void set_register_function(void *function)
    {
        __register_function = function;
    }
};

}

#endif
