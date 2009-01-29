#include <tut.h>

namespace tut
{
  /**
   * Testing exceptions in setup of test;
   * first run issue an integer 0 exception, 
   * second -- std::exception
   */
  struct setup_ex
  {   
    test_runner tr;
    struct dummy
    { 
      dummy()
      {
        static int n = 0;
#if defined(TUT_USE_SEH)
        static int d = 3;
#else
        static int d = 2;
#endif
        n++;
        if( n % d == 1 )
        {
          // at test 2
          throw 0;
        }
#if defined(TUT_USE_SEH)
        else if( n % d == 0 )
        {
          // at test 3
          *((char*)0) = 0;
        }
#endif
        else
        {
          // at test 1
          throw std::runtime_error("dummy");
        }
      };
    };
    typedef test_group<dummy> tf;
    typedef tf::object object;
    tf factory;

    setup_ex() : factory("internal",tr)
    {
    }
  };

  typedef test_group<setup_ex> tf;
  typedef tf::object object;
  tf setup_ex("exceptions at test setup time");

  /**
   * Checks getting unknown exception in setup.
   */
  template<>
  template<>
  void object::test<1>()
  {
    test_result r = tr.run_test("internal",1);
    ensure(r.result == test_result::ex_ctor);
  }

  /**
   * Checks getting std exception in setup.
   */
  template<>
  template<>
  void object::test<2>()
  {
    test_result r = tr.run_test("internal",2);
    ensure_equals("r.result",r.result,test_result::ex_ctor);
  }

#if defined(TUT_USE_SEH)
  /**
   * Checks getting segfault in setup under Win32
   */
  template<>
  template<>
  void object::test<3>()
  {
    test_result r = tr.run_test("internal",3);
    ensure(r.result == test_result::ex_ctor);
  }
#endif

  /**
   * Running all in turn.
   */
  template<>
  template<>
  void object::test<4>()
  {
    tr.run_tests("internal");
  }

  /**
   * Running all in turn.
   */
  template<>
  template<>
  void object::test<5>()
  {
    tr.run_tests("internal");
  }

  /**
   * Running all in turn.
   */
  template<>
  template<>
  void object::test<6>()
  {
    tr.run_tests();
  }

  /**
   * Running all in turn.
   */
  template<>
  template<>
  void object::test<7>()
  {
    tr.run_tests();
  }
}

