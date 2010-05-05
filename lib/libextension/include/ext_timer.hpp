#ifndef LH_TIMER_HPP
#define LH_TIMER_HPP

#include <iostream>
#include <cassert>
#include <signal.h>
#include <ctime>
#include <cstring>
#include <pthread.h>

namespace Lh
{
namespace Timers
{

/** Asynchronous timer
 */
class Timer
{
    /** Exception class for timer
     */
    class TimerException : public std::exception
    {
    public:
        /** Timer exception constructor
         * @param[in] p_errMsg error message
         */
        TimerException(const std::string &p_errMsg)
            : m_errMsg(p_errMsg)
        {
        }

        /** Destructor
         */
        virtual ~TimerException() throw()
        {
        }

        /** Exception message
         * @return error message
         */
        const char* getMessage() const
        {
            return m_errMsg.c_str();
        }

    private:
        std::string m_errMsg;
    };

public:
    /** Timer constructonr
     * @param[in] p_seconds
     * @param[in] p_nanoSeconds
     * @param[in] p_reload
     */
    Timer(unsigned int p_seconds = 1, unsigned long p_nanoSeconds = 0, bool p_reload = false);

    /** Start timer
     * Template function have to be in header file
     * @param[in] p_callBack
     */
    template<typename Function>
    void start(Function p_callBack)
    {
        assert((p_callBack != NULL) && "p_callBack != NULL");

        if( this->m_running == true )
        {
            throw TimerException("timer is running");
        }

        memset(&m_notifyEvp, 0, sizeof(struct sigevent));
        m_notifyEvp.sigev_notify_attributes = NULL;
        m_notifyEvp.sigev_value.sival_ptr = &m_time;
        m_notifyEvp.sigev_notify = SIGEV_THREAD;
        m_notifyEvp.sigev_notify_function = reinterpret_cast<sigev_notify>(*reinterpret_cast<void **>(&p_callBack));

        if( timer_create(CLOCK_REALTIME, &m_notifyEvp, &m_time) )
        {
            throw TimerException("timer create");
        }

        if( m_reload == true )
        {
            m_ts.it_interval.tv_sec = m_seconds;
            m_ts.it_interval.tv_nsec = m_nanoSeconds;
        }
        else
        {
            m_ts.it_interval.tv_sec = 0;
            m_ts.it_interval.tv_nsec = 0;
        }

        m_ts.it_value.tv_sec = m_seconds;
        m_ts.it_value.tv_nsec = m_nanoSeconds;

        if( timer_settime(m_time, 0, &m_ts, NULL) )
        {
            throw TimerException("timer settime");
        }

        this->m_running = true;
    }

    /** Stop timer
     */
    void stop();

private:
    struct sigevent m_notifyEvp;
    struct itimerspec m_ts;
    timer_t m_time;
    unsigned int m_seconds;
    unsigned int m_nanoSeconds;
    unsigned int m_reload;
    bool m_running;

    typedef void (*sigev_notify)(union sigval);
};

} // namespace Timers
} // namespace Lh

#endif

