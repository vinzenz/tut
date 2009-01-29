#include <tut.h>

namespace tut
{
  /**
   * Testing fail() method.
   */
  struct fail_test
  {   
  };

  typedef test_group<fail_test> tf;
  typedef tf::object object;
  tf fail_test("fail()");

  template<>
  template<>
  void object::test<1>()
  {
    try
    {
      fail("A Fail");
      throw std::runtime_error("fail doesn't work");
    }
    catch( const std::logic_error& ex )
    {
      if( std::string(ex.what()).find("A Fail") == std::string::npos )
      {
        throw std::runtime_error("fail doesn't contain proper message");
      }
    }
  }

  template<>
  template<>
  void object::test<2>()
  {
    try
    {
      fail();
      throw std::runtime_error("fail doesn't work");
    }
    catch( const std::logic_error& ex )
    {
    }
  }
}

