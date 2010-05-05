#ifndef TEST_EVENTS_HPP
#define TEST_EVENTS_HPP

#include <ext_event.hpp>

/** event to handle enter to function
*/
class event_func_enter : public ext::ievent
{
public:
    event_func_enter()
        : m_name("event_func_enter"), i(0)
    {
    }

private:
    void onEnterImpl(const ext::iextension_manager& p_ext)
    {
        std::clog << "Extension: " << p_ext.identify() << ++i << std::endl;
    }

    void onExitImpl(const ext::iextension_manager& p_ext)
    {
    }

    const std::string& identifyImpl() const { return m_name; }
    const std::string m_name;
    int i;
};

/** event to handle exit from function
 */
class event_func_exit : public ext::ievent
{
public:
    event_func_exit()
        : m_name("event_func_exit")
    {
    }

private:
    void onEnterImpl(const ext::iextension_manager& p_ext)
    {
    }

    void onExitImpl(const ext::iextension_manager& p_ext)
    {
    }

    const std::string& identifyImpl() const { return m_name; }
    const std::string m_name;
};

#endif

