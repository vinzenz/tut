#include <tut.h>
#include <tut_reporter.h>

#include <iostream>

namespace tut
{
  test_runner_singleton runner;
}

int main()
{
  tut::reporter reporter;

  try
  {
    tut::runner.get().set_callback(&reporter);
    tut::runner.get().run_tests();

    if( !reporter.all_ok() )
    {
      std::cout << std::endl;
      std::cout << "*********************************************************" << std::endl;
      std::cout << "WARNING: THIS VERSION OF TUT IS UNUSABLE DUE TO ERRORS!!!" << std::endl;
      std::cout << "*********************************************************" << std::endl;
    }
    else
    {
      std::cout << std::endl;
      std::cout << "THIS VERSION OF TUT IS CORRECT" << std::endl;
    }
  }
  catch( const std::exception& ex )
  {
    std::cerr << "tut raised ex: " << ex.what() << std::endl;
    return 1;
  }
  catch( ... )
  {
    std::cerr << "tut raised unknown exception" << std::endl;
    return 1;
  }

  return 0;
}
