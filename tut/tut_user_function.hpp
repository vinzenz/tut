#ifndef TUT_H_USER_FUNCTIONS
#define TUT_H_USER_FUNCTIONS

/**  
 * \brief User functions to showing function parameters
 * \brief and registering function order
 * @author Krzysztof Jusiak
 * @date 03/02/2009
 */
struct tut_user_functions
{
    /**
     * here is place where you can write own functions to shows function parameters and to registering functions
     * you can do this also in tests, by creating function and ...
     *
     * this functions should be static
     *
     * How to change show_parameters_function ?
     * tut_functions::set_show_parameters_function( POINTER_TO_FUNCTION );
     *
     * example:
     * tut_functions::set_show_parameters_function(get_pointer( &get_user_functions(show_parameters_function_tree)) );
     *
     * How to change register_function ?
     * tut_functions::set_register_function( POINTER_TO_FUNCTION );
     *
     * example:
     * tut_functions::set_register_function(get_pointer( &get_user_functions(register_function_public)) );
     */

    /**
     * example result
     * double function(int=3, double=4.3200, Class)
     */
    static std::string simple(function_parameters *func_parameters)
    {
        std::stringstream ss;

        if( func_parameters->get_mode() == function_parameters::_FUNCTION_ENTER )
        {
            ss << func_parameters->method->getReturnType() << " " << func_parameters->method->getName() << "(";
 
            for(std::vector<function_parameters::parameter *>::iterator it = func_parameters->begin(); it < func_parameters->end(); it++)
            {            
                ss << ((it != func_parameters->begin()) ? ", " : "") << (*it)->get_type();

                if( (*it)->is_class() == false )
                    ss << "=" << function_parameters::get_value((*it)->get_type(), (*it)->get_value());
            }

            ss << ")";
        }

        return ss.str();
    }

    /**
     * example result
     * ----test_data::change_var(int, Class)
     * |             1. int = 32
     * |             2. Class
     * |                1. Class.x (int) = 32
     * |                2. Class.y (int) = 1
     * |                 
     * |-------test_data::private_func()
     * |   |                   
     * |   \--> int = 23
     * |                       
     * \--<
     */
    static std::string tree(function_parameters *func_parameters)
    {
        std::stringstream ss;

        static unsigned int level = 0;
        static unsigned int recurency = 0; recurency++;

        if( func_parameters->get_mode() == function_parameters::_FUNCTION_ENTER )
        {
            for(unsigned int i=0; i < level * 4; i++)
               if( !(i % 4) && i <= level * 4)
                   ss << "|";
               else
               if( i >= (level-1) * 4 )
                   ss << "-";
               else
                   ss << " ";

            ss << "----" << func_parameters->get_name() << std::endl;
           
            unsigned int n = 1;
            for(std::vector<function_parameters::parameter *>::iterator it = func_parameters->begin(); it < func_parameters->end(); it++, n++)
            {
                for(unsigned int i=0; i < level * 4 + 4 + func_parameters->method->getName().length(); i++)
                    if( !(i % 4) && i <= level * 4)
                        ss << "|";
                    else
                        ss << " ";

                ss << n << ". " << (*it)->get_type();

                if( (*it)->is_class() == false )
                    ss << " = " << function_parameters::get_value((*it)->get_type(), (*it)->get_value()) << std::endl;
                else
                {
                    ss << std::endl;
                    for(std::vector<function_parameters::parameter *>::iterator ci = (*it)->begin(); ci < (*it)->end(); ci++)
                    {
                        for(unsigned int i=0; i < level * 4 + 4 + 4 + func_parameters->method->getName().length(); i++)
                            if( !(i % 4) && i <= level * 4)
                                ss << "|";
                            else
                                ss << " ";

                        if( (*ci)->is_table() == false )
                            ss << (*ci)->get_parent() << "." << (*ci)->get_name() << " (" << (*ci)->get_type() << ") = " << function_parameters::get_value((*ci)->get_type(), (*ci)->get_value()) << std::endl;
                    }
                }
            }

            for(unsigned int i=0; i < level * 4 + 4 + 4 + func_parameters->method->getName().length(); i++)
               if( !(i % 4) && i <= level * 4)
                   ss << "|";
               else
                   ss << " ";

            level++;
        }
        else
        {
            for(unsigned int i=0; i < level * 4 - 4; i++)
                if( !(i % 4) && i <= level * 4)
                    ss << "|";
                else
                    ss << " ";

            if( func_parameters->get_size() > 0 )
            {
                ss << "\\--> ";
                std::vector<function_parameters::parameter *>::iterator it = func_parameters->begin();
                ss << (*it)->get_type() << " = " << function_parameters::get_value((*it)->get_type(), (*it)->get_value()) << std::endl;
            }
            else
                ss << "\\--<" << std::endl;

            level--;

            for(unsigned int i=0; i < level * 4 + 4 + 4 + func_parameters->method->getName().length(); i++)
               if( !(i % 4) && i < level * 4)
                   ss << "|";
               else
                   ss << " ";
        }

        return ss.str();
    }

    static std::string empty(function_parameters *)
    {
        return "";
    }

    /**
     * place to other show parameters functions
     */


    /**
     * macro to simple add new show parameters function
     */
    #define add_new_show_parameters_function(func)\
        static void show_parameters_function_##func(function_parameters *func_parameters)\
        {\
            std::cout << func(func_parameters) << std::endl;\
        }

    /**
     * empty show parameters function <- default
     */
    static void show_parameters_function_empty(function_parameters *)
    {}

    /**
     * adds new show parameters functions
     * you can also simple write function definition with do the same
     */
    add_new_show_parameters_function(simple);
    add_new_show_parameters_function(tree);


    /**
     * simple registering all functions between start_registering() and end_registering()
     */
    static void register_function_all(function_parameters *func_parameters)
    {
        tut_functions::register_function(func_parameters->method->getPtr(), func_parameters->get_mode());
    }

    /**
     * registering only public methods
     */
    static void register_function_public(function_parameters *func_parameters)
    {
        if( func_parameters->method->getAccessibility() == reflection::Reflection::_PUBLIC )
            tut_functions::register_function(func_parameters->method->getPtr(), func_parameters->get_mode());
    }

    /**
     * place to other registering functions
     */
};

/**
 * macro to simple get namespaces to user_functions class
 */
#define get_user_functions(func) tut::tut_functions::tut_user_functions::func

#endif
