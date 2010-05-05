#include <cassert>
#include "LhTimer.hpp"

namespace Lh
{
namespace Timers
{

Timer::Timer(unsigned int p_seconds, unsigned long p_nanoSeconds, bool p_reload)
    : m_seconds(p_seconds), m_nanoSeconds(p_nanoSeconds), m_reload(p_reload), m_running(false)
{
    assert( (p_seconds >= 0) && "p_seconds >= 0");
    assert( (p_nanoSeconds >= 0) && "p_nanoSeconds >= 0");
}

void Timer::stop()
{
    memset(&m_ts, 0, sizeof(m_ts));
    if( timer_settime(m_time, 0, &m_ts, NULL) )
    {
        throw TimerException("timer delete");
    }

    m_running = false;
}

} // namespace Timers
} // namespace Lh

