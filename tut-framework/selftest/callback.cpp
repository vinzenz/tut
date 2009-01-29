#include <tut.h>
#include <vector>

namespace tut
{
  /**
   * Tests order and validity of calling callback methods
   * for various test results.
   */
  struct callback_test
  {   
    std::vector<int> called;
    std::vector<std::string> grps;
    std::string msg;

    test_runner tr;
    struct dummy{};
    typedef test_group<dummy> tf;
    typedef tf::object object;
    tf factory;
    tf factory2;

    struct chk_callback : public tut::callback
    {
      callback_test& ct;
      chk_callback(callback_test& c) : ct(c){};

      void run_started(){ ct.called.push_back(1); };
      void test_completed(const tut::test_result& tr)
      { 
        if( ct.grps.size() == 0 || tr.group != ct.grps[ct.grps.size()-1] )
        {
          ct.grps.push_back(tr.group);
        }

        ct.called.push_back(3); ct.msg = tr.message; 
      };
      void run_completed(){ ct.called.push_back(4); };
    } callback;

    callback_test();
  };

  // ==================================
  // tests of internal runner
  // ==================================
  template<>
  template<>
  void callback_test::object::test<1>()
  {
    // OK
  }

  template<>
  template<>
  void callback_test::object::test<3>()
  {
    throw std::runtime_error("an error");
  }

  callback_test::callback_test() : 
    factory("internal",tr),factory2("0copy",tr), callback(*this)
  {
  }

  // ==================================
  // tests of controlling runner
  // ==================================
  typedef test_group<callback_test> tf;
  typedef tf::object object;
  tf callback_test("callback");

  /**
   * Checks running OK test
   */
  template<>
  template<>
  void object::test<1>()
  {
    tr.set_callback(&callback);
    tr.run_test("internal",1);
    ensure_equals("size",called.size(),3U);
    ensure("1",called[0]==1);
    ensure("3",called[1]==3);
    ensure("4",called[2]==4);
    ensure_equals("msg",msg,"");
    ensure_equals("grp",grps[0],"internal");
  }

  /**
   * Checks running test throwing exception
   */
  template<>
  template<>
  void object::test<2>()
  {
    tr.set_callback(&callback);
    tr.run_test("internal",3);
    ensure_equals("size",called.size(),3U);
    ensure("1",called[0]==1);
    ensure("3",called[1]==3);
    ensure("4",called[2]==4);
    ensure_equals("msg",msg,"an error");
    ensure_equals("grp",grps[0],"internal");
  }

  /**
   * Checks running all tests.
   */
  template<>
  template<>
  void object::test<3>()
  {
    tr.set_callback(&callback);
    tr.run_tests();
    ensure_equals("size",called.size(),6U);
    ensure("1",called[0]==1);
    ensure("3",called[1]==3);
    ensure("4",called[2]==3);
    ensure("6",called[3]==3);
    ensure("7",called[4]==3);
    ensure("8",called[5]==4);
    ensure_equals("msg",msg,"an error");
    ensure_equals("grp[0]",grps[0],"0copy");
    ensure_equals("grp[1]",grps[1],"internal");
  }
}

