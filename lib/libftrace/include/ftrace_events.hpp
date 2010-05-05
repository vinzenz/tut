#ifndef FTRACE_EVENTS_HPP
#define FTRACE_EVENTS_HPP

#include <ext_event.hpp>

/** event to handle enter to function
 */
class event_func_enter : public ext::ievent
{
public:
    event_func_enter()
        : m_name("event_func_enter")
    {}

private:
    void onBeginImpl()
    {
    }

    void onEndImpl()
    {
    }

    const std::string& identifyImpl() const { return m_name; }
    const std::string m_name;
};

/** event to handle exit from function
 */
class event_func_exit : public ext::ievent
{
public:
    event_func_exit()
        : m_name("event_func_exit")
    {}

private:
    void onBeginImpl()
    {
    }

    void onEndImpl()
    {
    }

    const std::string& identifyImpl() const { return m_name; }
    const std::string m_name;
};

#endif

