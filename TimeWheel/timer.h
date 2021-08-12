#pragma once 

#include <functional>
#include <memory>
#include <cstdio>

class Timer {
public:
    using TaskCallBack = std::function<void()>;

    Timer(size_t id, size_t when_ms, size_t interval_ms, const TaskCallBack& cb);
    ~Timer();

    void Start();

    size_t id() const { return m_id; }

    size_t when_ms() const { return m_when_ms; }

    bool repeated() const { return m_repeated; }

    void UpdataWhenTime() {
        m_when_ms += m_interval_ms;
    }

private:
    TaskCallBack m_cb;
    size_t m_id;
    size_t m_when_ms;
    size_t m_interval_ms;
    bool m_repeated;
}