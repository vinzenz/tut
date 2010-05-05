#include <memory>

#include <ftrace.hpp>
#include <ftrace_events.hpp>

void __cyg_profile_func_enter(void *this_fn, void *call_site)
{
    static bool s_instrumentEnter = true;

    if( s_instrumentEnter )
    {
        s_instrumentEnter = false;
    }
    else
    {
        return;
    }

    //event func enter
    std::unique_ptr< event<event_func_enter, int> >(new event<event_func_enter, int>(1232));

    s_instrumentEnter = true;
}

void __cyg_profile_func_exit(void *this_fn, void *call_site)
{
    static bool s_instrumentExit = true;

    if( s_instrumentExit )
    {
        s_instrumentExit = false;
    }
    else
    {
        return;
    }

    //event func exit
    std::unique_ptr< event<event_func_exit, int> >(new event<event_func_exit, int>(1232));

    s_instrumentExit = true;
}

