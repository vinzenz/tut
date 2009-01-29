#include <tut.h>
#include <tut_reporter.h>
#include <iostream>

using std::exception;
using std::cerr;
using std::endl;

namespace tut
{
    
test_runner_singleton runner;

}

int main()
{
    tut::reporter reporter;
    tut::runner.get().set_callback(&reporter);

    try
    {
        tut::runner.get().run_tests();
    }
    catch (const std::exception& ex)
    {
        cerr << "tut raised ex: " << ex.what() << endl;
        return 1;
    }

    return 0;
}
