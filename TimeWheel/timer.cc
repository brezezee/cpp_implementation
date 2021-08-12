#include "timer.h"

Timer::Timer(size_t id, size_t when_ms, size_t interval_ms, const TaskCallBack& cb) 
    : m_id(id)
    , m_when_ms(when_ms)
    , m_interval_ms(interval_ms)
    , m_cb(cb)
    , m_repeated(interval_ms > 0)
{

}

void Timer::Start() {
    if (m_cb) {
        m_cb();
    }
}