#include "TimeWheelManager.h"
#include <algorithm>
#include <cassert>
#include <iostream>

static uint32_t s_inc_id = 1;

template<int BaseScale>
TimeWheelManager<BaseScale>* TimeWheelManager<BaseScale>::ptimemanager_ = nullptr;

template<int BaseScale>
typename TimeWheelManager<BaseScale>::GC TimeWheelManager<BaseScale>::gc;

template<int BaseScale>
TimeWheelManager<BaseScale>* TimeWheelManager<BaseScale>::GetTimerWheelManager() {
  if (!ptimemanager_) {
    ptimemanager_ = new TimeWheelManager();
  }
  return ptimemanager_;
}

template<int BaseScale>
TimeWheelManager<BaseScale>::TimeWheelManager(uint32_t base_scale_ms)
    : base_scale_ms_(base_scale_ms)
    , stop_(false) {
  // 至少需要一个刻度为最小精度的时间轮
  AppendTimeWheel(1000 / base_scale_ms, base_scale_ms, "ScaleTimeWheel");
}

template<int BaseScale>
TimeWheelManager<BaseScale>::~TimeWheelManager() {
  Stop();
}

template<int BaseScale>
bool TimeWheelManager<BaseScale>::Start() {
  if (base_scale_ms_ < 10) {
    return false;
  }
  
  if (time_wheels_.empty()) {
    return false;
  }

  thread_ = std::thread(std::bind(&TimeWheelManager::Run, this));

  return true;
}

template<int BaseScale>
void TimeWheelManager<BaseScale>::Run() {
  while (true) {
    // 计时 tick
    std::this_thread::sleep_for(std::chrono::milliseconds(base_scale_ms_));

    std::lock_guard<std::mutex> lock(mtx_);
    if (stop_) {
      break;
    }

    // 需要有一个和最小刻度保持一致的时间轮
    TimeWheelPtr least_time_wheel = GetLeastTimeWheel();
    least_time_wheel->Increase();
    // 最小刻度时间轮的定时器需要管家来负责处理，并且需要执行的定时器一定都是落到最小刻度的定时器
    std::list<spTimer> slot = std::move(least_time_wheel->TakeoutSlot());
    for (auto&& timer : slot) {
      auto it = cancel_timer_ids_.find(timer->id());
      if (it != cancel_timer_ids_.end()) {
        cancel_timer_ids_.erase(it);
        continue;
      }

      timer->execute();
      if (timer->repeated()) {
        timer->UpdateWhenTime();
        // 从最大刻度重新添加timer
        GetGreatestTimeWheel()->AddTimer(timer);
      }
    }
  }
}

template<int BaseScale>
void TimeWheelManager<BaseScale>::Stop() {
  {
    std::lock_guard<std::mutex> lock(mtx_);
    stop_ = true;
  }

  thread_.join();
}

template<int BaseScale>
TimeWheelPtr TimeWheelManager<BaseScale>::GetGreatestTimeWheel() {
  if (time_wheels_.empty()) {
    return TimeWheelPtr();
  }

  return time_wheels_.front();
}

template<int BaseScale>
TimeWheelPtr TimeWheelManager<BaseScale>::GetLeastTimeWheel() {
  if (time_wheels_.empty()) {
    return TimeWheelPtr();
  }

  return time_wheels_.back();
}

template<int BaseScale>
void TimeWheelManager<BaseScale>::AppendTimeWheel(uint32_t scales, uint32_t scale_unit_ms, const std::string& name) {
  TimeWheelPtr time_wheel = std::make_shared<TimeWheel>(scales, scale_unit_ms, name);
  if (time_wheels_.empty()) {
    time_wheels_.push_back(time_wheel);
    return;
  }

  assert(*time_wheel > *(time_wheels_.back()));  // 不能小于最小刻度

  int insert_idx = 0;
  while (insert_idx < time_wheels_.size() 
        && *time_wheel < *time_wheels_[insert_idx]){
    ++ insert_idx;
  }

  // 是否有上级时间轮
  if (insert_idx > 0) {
    TimeWheelPtr greater_time_wheel = time_wheels_[insert_idx - 1];
    greater_time_wheel->set_less_level_tw(time_wheel.get());
    time_wheel->set_greater_level_tw(greater_time_wheel.get());
  }

  TimeWheelPtr less_time_wheel = time_wheels_[insert_idx];
  time_wheel->set_less_level_tw(less_time_wheel.get());
  less_time_wheel->set_greater_level_tw(time_wheel.get());

  time_wheels_.push_back(time_wheel);
  sort(time_wheels_.begin(), time_wheels_.end(), 
      [&](const TimeWheelPtr& l, const TimeWheelPtr& r){
        return *l > *r;
      }
  );
}

template<int BaseScale>
uint32_t TimeWheelManager<BaseScale>::CreateTimerAt(int64_t trigger_time, const TaskCallback& task) {
  if (time_wheels_.empty()) {
    return 0;
  }

  std::lock_guard<std::mutex> lock(mtx_);
  ++s_inc_id;
  GetGreatestTimeWheel()->AddTimer(std::make_shared<Timer>(s_inc_id, trigger_time, 0, task));

  return s_inc_id;
}

template<int BaseScale>
uint32_t TimeWheelManager<BaseScale>::CreateTimerAfter(int64_t delay_time, const TaskCallback& task) {
  int64_t when = GetNowTimestamp() + delay_time;
  return CreateTimerAt(when, task);
}
template<int BaseScale>
uint32_t TimeWheelManager<BaseScale>::CreateTimerEvery(int64_t interval_time, const TaskCallback& task) {
  if (time_wheels_.empty()) {
    return 0;
  }

  std::lock_guard<std::mutex> lock(mtx_);
  ++s_inc_id;
  int64_t when = GetNowTimestamp() + interval_time;
  GetGreatestTimeWheel()->AddTimer(std::make_shared<Timer>(s_inc_id, when, interval_time, task));

  return s_inc_id;
}
template<int BaseScale>
void TimeWheelManager<BaseScale>::CancelTimer(uint32_t timer_id) {
  std::lock_guard<std::mutex> lock(mtx_);
  cancel_timer_ids_.insert(timer_id);
}
