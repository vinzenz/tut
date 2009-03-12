#ifndef __TEST_DATA_H
#define __TEST_DATA_H

#include <iostream>

class test_data
{
private:
    int int_var;
    
    void private_func();
        
public:
    void change_var(int new_var);
    int get_int_var();
    int param(int a, int b, std::string c);
    int min_param(int a, int b);
    double double_test(double a, int b);
};

#endif
