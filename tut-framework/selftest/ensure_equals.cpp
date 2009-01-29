#include <tut.h>
#include <string>
#include <vector>

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
    ensure_equals("1==n",1,n);
  }

  /**
   * Checks positive ensure_equals with complex non-matching types
   */
  template<>
  template<>
  void object::test<2>()
  {
    ensure_equals("string(foo)==foo",std::string("foo"),"foo");
    ensure_equals("foo==string(foo)","foo",std::string("foo"));
  }

  /**
   * Checks positive ensure_equals with complex matching types
   */
  template<>
  template<>
  void object::test<3>()
  {
    ensure_equals("string==string",std::string("foo"),std::string("foo"));
  }

  /**
   * Checks negative ensure_equals with simple types
   */
  template<>
  template<>
  void object::test<10>()
  {
    volatile int n = 1; // to stop optimization
    try
    {
       ensure_equals("2!=n",2,n);
       throw std::runtime_error("ensure_equals failed");
    }
    catch( const std::logic_error& ex )
    {
      if( std::string(ex.what()).find("2!=n") == std::string::npos )
      {
        throw std::runtime_error("contains wrong message");   
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
    try
    {
      ensure_equals("string(foo)!=boo",std::string("foo"),"boo");
      throw std::runtime_error("ensure_equals failed");
    }
    catch( const std::logic_error& ex )
    {
      if( std::string(ex.what()).find("string(foo)!=boo") == std::string::npos )
      {
        throw std::runtime_error("contains wrong message");   
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
    try
    {
      ensure_equals("string(foo)!=string(boo)",std::string("foo"),std::string("boo"));
      throw std::runtime_error("ensure_equals failed");
    }
    catch( const std::logic_error& ex )
    {
      if( std::string(ex.what()).find("string(foo)!=string(boo)") == std::string::npos )
      {
        throw std::runtime_error("contains wrong message");   
      }
    }
  }
}

