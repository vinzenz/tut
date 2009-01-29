#include <tut_restartable.h>
#include <tut_reporter.h>

namespace tut
{
  test_runner_singleton runner;
}

int main()
{
  std::cerr << 
  "NB: this application will be terminated by OS four times\n"
  "before you'll get test results, be patient restarting it.\n";

  try
  {
    tut::reporter visi;
    tut::restartable_wrapper restartable;
   
    restartable.set_callback(&visi);     
    restartable.run_tests();
  }
  catch( const std::exception& ex )
  {
    std::cerr << "tut raised ex: " << ex.what() << std::endl;
  }

  return 0;
}


