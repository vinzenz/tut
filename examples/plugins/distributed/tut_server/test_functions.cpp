#include <tut/tut.hpp>

namespace tut {

/**
 * if is declarated USE_TUT_FUNCTIONS, must be this function
 */
void set_reflection()
{
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
    struct _tmp
    {
        static void my_show(function_parameters *)
        {
std::cout << "REWOUEWROUREOUREWOUEWRO" << std::endl;
//exit(0);
        }
    };
//     tut_functions::set_show_parameters_function(get_pointer( &get_user_functions(show_parameters_function_tree)) );
     tut_functions::set_show_parameters_function(get_pointer( &_tmp::my_show));

    //add functions order

//    run_script();//do something on client or run client or wait and do ...
    //wait for client or
    //simple run client

    //system("telnet.sh "

    //pytaj o haslo itp.
    //run_test_script("telnet.sh localhost")

    //maksymalny czas oczekiwania na odpowiedz od klienta, jako ustawienie
    //lub oczekiwanie na funkcje, ktora skonczy wszystko
    
    //sprawdzenie kolejnosci, parametrow
//    std::cout << "NQ: " << tut::tut_functions::__show_parameters_function << std::endl;

    //rejestruj tylko funkcje danego klienta

    while(1){}  
    
}

}
