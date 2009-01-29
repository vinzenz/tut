#include <tut.h>
#include <string>
#include <vector>

namespace tut
{
  /**
   * Testing ensure_distance() method.
   */
  struct ensure_distance_test
  {   
  };

  typedef test_group<ensure_distance_test> tf;
  typedef tf::object object;
  tf ensure_distance_test("ensure_distance()");

  /**
   * Checks positive ensure_distance with simple types
   */
  template<>
  template<>
  void object::test<1>()
  {
    ensure_distance("2~=1",1,2,2);
    ensure_distance("0~=1",1,0,2);
    ensure_distance(1,2,2);
    ensure_distance(1,0,2);
  }

  /**
   * Checks positive ensure_distance with doubles.
   */
  template<>
  template<>
  void object::test<2>()
  {
    ensure_distance("1.0~=1.01",1.0,1.01,0.011);
    ensure_distance("1.0~=0.99",1.0,0.99,0.011);
    ensure_distance(1.0,1.01,0.011);
    ensure_distance(1.0,0.99,0.011);
  }

  /**
   * Checks negative ensure_distance with simple types
   */
  template<>
  template<>
  void object::test<10>()
  {
    try
    {
       ensure_distance("2!~1",2,1,1);
       throw std::runtime_error("ensure_distance failed");
    }
    catch( const std::logic_error& ex )
    {
    }

    try
    {
       ensure_distance("0!~1",0,1,1);
       throw std::runtime_error("ensure_distance failed");
    }
    catch( const std::logic_error& ex )
    {
    }
  }

  /**
   * Checks negative ensure_distance with simple types
   */
  template<>
  template<>
  void object::test<11>()
  {
    try
    {
       ensure_distance(2,1,1);
       throw std::runtime_error("ensure_distance failed");
    }
    catch( const std::logic_error& ex )
    {
    }

    try
    {
       ensure_distance(0,1,1);
       throw std::runtime_error("ensure_distance failed");
    }
    catch( const std::logic_error& ex )
    {
    }
  }

  /**
   * Checks negative ensure_distance with doubles.
   */
  template<>
  template<>
  void object::test<12>()
  {
    try
    {
      ensure_distance("1.0!=1.02",1.02,1.0,0.01);
      throw std::runtime_error("ensure_distance failed");
    }
    catch( const std::logic_error& ex )
    {
    }

    try
    {
      ensure_distance("1.0!=0.98",0.98,1.0,0.01);
      throw std::runtime_error("ensure_distance failed");
    }
    catch( const std::logic_error& ex )
    {
    }
  }
}

