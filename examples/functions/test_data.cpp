#include "test_data.hpp"

void test_data::private_func()
{
}

void test_data::change_var(int new_var)
{
//    private_func();
    this->int_var = new_var;
}

int test_data::get_int_var() { return int_var; }

int test_data::param(int a, int b, std::string c)
{
    std::cout << "PARAMETES: |" << a << "|" << b << "|" << c.c_str() << "|" << std::endl;

    return a+b;
}

int test_data::min_param(int a, int b)
{
    std::cout << "PARAMETES: |" << a << "|" << b << "|" << std::endl;

    return a+b;
}

double test_data::double_test(double a, int b)
{
//    printf("%f %d\n", a, b);

    return a * 2;
}
