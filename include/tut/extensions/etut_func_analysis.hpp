#ifndef ETUT_FUNC_ANALYSIS_HPP
#define ETUT_FUNC_ANALYSIS_HPP

#include <ctime>
#include <sstream>

#include <ext_extension.hpp>
#include <test_events.hpp>
#include <ftrace_type.hpp>

namespace ext
{

//forward declaration
template<typename T> class etut_func_analysis;
class func_analysis;

namespace detail
{
typedef func_analysis ExtensionParameterType;
typedef ftrace::fun_type EventParameterType;
} // namespace detail

//extension declaration
template std::shared_ptr< etut_func_analysis<detail::ExtensionParameterType> > EXTENSION();

/** Extension example
 */
template<typename T>
class etut_func_analysis : public extension<T>
{
    typedef typename extension<T>::template event_handler<event_func_enter, detail::EventParameterType> EventFuncEnter;
    typedef typename extension<T>::template event_handler<event_func_exit, detail::EventParameterType> EventFuncExit;

public:
    /** Default Ctor
     */
    etut_func_analysis()
        : m_id("etut_func_analysis")
    {
        init( std::shared_ptr<EventFuncEnter>(new EventFuncEnter(*this, true, detail::PRIORITY_HIGH)) );
        init( std::shared_ptr<EventFuncEnter>(new EventFuncEnter(*this, true, detail::PRIORITY_HIGH)) );
    }

private:
    /** Set value
     * @param p_data value
     */
    void setImpl(T p_data)
    {
        m_data = p_data;
    }

    /** Get value
     * @return value
     */
    T& getImpl()
    {
        return m_data;
    }

    /**
     *
     *
     */
    void onEnableImpl()
    {
    }

    /**
     *
     *
     */
    void onDisableImpl()
    {
    }

    T m_data;
    const std::string& identifyImpl() const { return m_id; }
    const std::string m_id;
};

/** Keep function analysis information
 */
class func_analysis
{
public:
    enum t_time
    {
        TIME_NS = 1,
        TIME_MS = 1000000
    };

    /**
     *
     *
     */
    func_analysis()
    {
        m_time.tv_sec = 0;
        m_time.tv_nsec = 0;
    }

    /**
     *
     *
     */
    void setTime()
    {
        clock_settime(CLOCK_PROCESS_CPUTIME_ID, &m_time);
    }

    /**
     * @param p_base
     *
     * @return
     */
    unsigned long getTime(t_time p_base = TIME_MS)
    {
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &m_time);
        return m_time.tv_nsec / p_base;
    }

private:
    timespec m_time;
};

/**
 *
 * @param p_param
 */
template<>
template<>
void etut_func_analysis<detail::ExtensionParameterType>::event_handler<event_func_enter, detail::EventParameterType>::
     handleImpl(detail::EventParameterType p_param)
{
    m_base.get<detail::EventParameterType>.setTime();
}

/**
 *
 * @param p_param
 */
template<>
template<>
void etut_func_analysis<detail::ExtensionParameterType>::event_handler<event_func_exit, detail::EventParameterType>::
     handleImpl(detail::EventParameterType p_param)
{
    std::stringstream l_output;
    l_output << m_base.get<detail::EventParameterType>.getTime();
}

} // namespace ext

#endif

