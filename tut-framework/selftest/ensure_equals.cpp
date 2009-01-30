#include <tut.h>
#include <string>
#include <stdexcept>

using std::string;
using std::runtime_error;

namespace tut
{

/**
 * Testing ensure_equals() method.
 */
struct ensure_eq_test
{
};

typedef test_group<ensure_eq_test> tf;
typedef tf::object object;
tf ensure_eq_test("ensure_equals()");

/**
 * Checks positive ensure_equals with simple types
 */
template<>
template<>
void object::test<1>()
{
    volatile int n = 1; // to stop optimization
    ensure_equals("1==n", 1, n);
}

/**
 * Checks positive ensure_equals with complex non-matching types
 */
template<>
template<>
void object::test<2>()
{
    set_test_name("checks positive ensure_equals with complex non-matching"
        " types");

    ensure_equals("string(foo)==foo", string("foo"), "foo");
    ensure_equals("foo==string(foo)", "foo", string("foo"));
}

/**
 * Checks positive ensure_equals with complex matching types
 */
template<>
template<>
void object::test<3>()
{
    set_test_name("checks positive ensure_equals with complex matching types");
    
    ensure_equals("string==string", string("foo"), string("foo"));
}

/**
 * Checks negative ensure_equals with simple types
 */
template<>
template<>
void object::test<10>()
{
    set_test_name("checks negative ensure_equals with simple types");
    
    volatile int n = 1; // to stop optimization
    try
    {
        ensure_equals("2!=n", 2, n);
        throw runtime_error("ensure_equals failed");
    }
    catch (const failure& ex)
    {
        if (string(ex.what()).find("2!=n") == string::npos)
        {
            throw runtime_error("contains wrong message");
        }
    }
}

/**
 * Checks negative ensure_equals with complex non-matching types
 */
template<>
template<>
void object::test<11>()
{
    set_test_name("checks negative ensure_equals with complex non-matching"
        " types");
    
    try
    {
        ensure_equals("string(foo)!=boo", string("foo"), "boo");
        throw runtime_error("ensure_equals failed");
    }
    catch (const failure& ex)
    {
        if (string(ex.what()).find("string(foo)!=boo") == string::npos)
        {
            throw runtime_error("contains wrong message");
        }
    }
}

/**
 * Checks negative ensure_equals with complex matching types
 */
template<>
template<>
void object::test<12>()
{
    set_test_name("checks negative ensure_equals with complex matching types");
    
    try
    {
        ensure_equals("string(foo)!=string(boo)", string("foo"), string("boo"));
        throw runtime_error("ensure_equals failed");
    }
    catch (const failure& ex)
    {
        if (string(ex.what()).find("string(foo)!=string(boo)") == string::npos)
        {
            throw runtime_error("contains wrong message");
        }
    }
}

/**
 * Checks positive ensure_equals with float type
 */
template<>
template<>
void object::test<13>()
{
    set_test_name("checks positive ensure_equals with float type");
    float f1 = 1.0f, f2 = 3.0f;
    ensure_equals("f1/f2 * 10 == 10/3", f1 / f2 * 10, 10 / f2); 
}

/**
 * Checks positive ensure_equals with double type
 */
template<>
template<>
void object::test<14>()
{
    set_test_name("checks positive ensure_equals with double type");
    double d1 = 1.0, d2 = 3.0;
    ensure_equals("d1/d2 * 10 == 10/3", d1 / d2 * 10, 10 / d2); 
}

/**
 * Checks positive ensure_equals with long double type
 */
template<>
template<>
void object::test<15>()
{
    set_test_name("checks positive ensure_equals with long double type");
    long double d1 = 1.0, d2 = 3.0;
    ensure_equals("d1/d2 * 10 == 10/3", d1 / d2 * 10, 10 / d2); 
}

}
