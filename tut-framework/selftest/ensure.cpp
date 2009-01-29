#include <tut.h>

namespace tut
{
  /**
   * Testing ensure() method.
   */
  struct ensure_test
  {   
  };

  typedef test_group<ensure_test> tf;
  typedef tf::object object;
  tf ensure_test("ensure()");

  /**
   * Checks positive ensure
   */
  template<>
  template<>
  void object::test<1>()
  {
    ensure("OK",1==1);
    ensure(1==1);
  }

  /**
   * Checks negative ensure
   */
  template<>
  template<>
  void object::test<2>()
  {
    try
    {
      ensure("ENSURE",1==2);

      // we cannot relay on fail here; we haven't tested it yet ;)
      throw std::runtime_error("passed below");
    }
    catch( const std::logic_error& ex )
    {
      std::string msg = ex.what();
      if( msg.find("ENSURE") == std::string::npos )
      {
        throw std::runtime_error("ex.what has no ENSURE");
      }
    }

    try
    {
      ensure(1==2);
      throw std::runtime_error("passed below");
    }
    catch( const std::logic_error& ex )
    {
    }
  }

  /**
   * Checks ensure with various "constructed" messages 
   */
  template<>
  template<>
  void object::test<3>()
  {
    const char* ok1 = "OK";
    ensure(ok1,1==1);
  }

  template<>
  template<>
  void object::test<4>()
  {

    char* ok2 = "OK";
    ensure(ok2,1==1);
  }

  template<>
  template<>
  void object::test<5>()
  {
    std::string msg = "OK";
    ensure(msg,1==1);
  }

  template<>
  template<>
  void object::test<6>()
  {
    std::ostringstream oss;
    oss << "OK";
    ensure(oss.str(),1==1);
  }
}

