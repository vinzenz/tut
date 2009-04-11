#include <tut/tut.hpp>
#include <tut/tut_function.hpp>

#include "test_data.hpp"

namespace tut {



class Test
{
public:
       Test() {};
       int MethodToStub(int a,int b,int c);
};
//stub("M",int,Test,MethodToStub,int,int,int);


/**
 * if is declarated USE_TUT_FUNCTIONS, must be this function
 */
void set_reflection()
{
    //allow all classes with methods which you wont to do something
    //! if you don't wont to register for example private methods, you can change register_function
    tut_reflection::reflect.allow("test_data.*"); //allow class test_functions with all methods (private and public)
}

struct test_functions
{
};

typedef test_group<test_functions> tf;
typedef tf::object object;
tf test_functions_group("test functions extends");

/**
 * Checks if reflection systems works / should show all types pass to allow types in set_reflection()
 */
template<>
template<>
void object::test<1>()
{
    std::cout << "--------------------------------\n";
    std::cout << tut_reflection::reflect.toString();
    std::cout << "--------------------------------\n";
}

/**
 * Checks
 */
template<>
template<>
void object::test<2>()
{
    tut_functions functions;

    tut_functions::set_show_parameters_function(get_pointer( &get_user_functions(show_parameters_function_tree)) );
    //tut_functions::set_register_function(get_pointer( &get_user_functions(register_function_public)) );

    functions.order->add( get_pointer(&test_data::change_var) );
    functions.order->add( get_private(&test_data::private_func) );

    std::cout << std::endl;

    functions.order->start_registering();
    test_data t_f;
    t_f.change_var(32);
    functions.order->end_registering();

    ensure_functions_order(functions.order);

}

/**
 * Checks
 */
template<>
template<>
void object::test<3>()
{
    tut_functions functions;

    tut_functions::set_show_parameters_function(get_pointer( &get_user_functions(show_parameters_function_tree)) );
    __set_body(functions.body, change_var, get_pointer(&test_data::change_var),
            void change_var()
            {
                //std::cout << "NEW BODY FUNCTION" << std::endl;
                __set_value(get_this, "test_data", "int_var", 123); //setting private value in new body function for old object
            }
        );


    std::cout << std::endl;
    functions.order->start_registering();
        test_data t_f;
        t_f.change_var(32);
    functions.order->end_registering();

    ensure_equals("t_f.get_int_var() == 123", t_f.get_int_var(), 123);

}

/**
 * Checks
 */
template<>
template<>
void object::test<4>()
{
    tut_functions functions;

    struct _tmp
    {
        static void my_show(function_parameters *)
        {
        }
    };
//    tut_functions::set_show_parameters_function(get_pointer(&_tmp::my_show) );

    tut_functions::set_show_parameters_function(get_pointer( &get_user_functions(show_parameters_function_tree)) );

    __set_body(functions.body, min_param, get_pointer(&test_data::min_param),
            int min_param(int a, int b)
            {
                std::cout << "NEW PARAMETERS:" << a << ":" << b << std::endl;
          //      __set_parameters( get_pointer(&test_data::min_param), int, 4, 3, "hej");
                return a+b;
            }
        );


    std::cout << std::endl;
    functions.order->start_registering();
    test_data t_f;
t_f.min_param(76, 89);
//    ensure_equals("t_f.param(1,1)==7", t_f.param(1, 1, "param"), 7);
    functions.order->end_registering();
}

/**
 * Checks double
 */
template<>
template<>
void object::test<5>()
{
    tut_functions functions;

    __set_body_times(functions.body, double_test, get_pointer(&test_data::double_test),
            double double_test(double a, int b)
            {
                std::cout << "NEW PARAMETERS:" << a << ":" << b << std::endl;

                return 4.0;
            }, 1
        );


    std::cout << std::endl;
    functions.order->start_registering();
    test_data t_f;
        t_f.double_test(3.0, 87);
        t_f.double_test(3.0, 87);
//    ensure_equals("t_f.param(1,1)==7", t_f.param(1, 1, "param"), 7);
    functions.order->end_registering();
}

/**
 * Checks double
 */
template<>
template<>
void object::test<6>()
{
    tut_functions functions;

    struct _class {
        static double function(double a, int b) {
            return a;
        }
    };

    functions.body->set( get_pointer(&test_data::double_test), get_pointer(&_class::function) );

    std::cout << std::endl;
    functions.order->start_registering();
    test_data t_f;
    double ret = t_f.double_test(11.0, 87);
    ensure_equals("t_f.param(11,87)==11", ret, 11.0);
    functions.order->end_registering();
}

}
