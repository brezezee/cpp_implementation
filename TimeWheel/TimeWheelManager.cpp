#include "TimeWheelManager.h"
#include <algorithm>
#include <cassert>
#include <iostream>

static uint32_t s_inc_id = 1;
TimeWheelManager* TimeWheelManager::ptimemanager_ = nullptr;
TimeWheelManager::GC TimeWheelManager::gc;


TimeWheelManager* TimeWheelManager::GetTimerWheelManager() {
  if (!ptimemanager_) {
    ptimemanager_ = new TimeWheelManager();
  }
  return ptimemanager_;
}

TimeWheelManager::TimeWheelManager(uint32_t timer_step_ms)
    : timer_step_ms_(timer_step_ms)
    , stop_(false) {
  // 至少需要一个刻度为最小精度的时间轮
  AppendTimeWheel(1000 / timer_step_ms, timer_step_ms, "ScaleTimeWheel");
}

TimeWheelManager::~TimeWheelManager() {
  Stop();
}

bool TimeWheelManager::Start() {
  if (timer_step_ms_ < 10) {
    return false;
  }
  
  if (time_wheels_.empty()) {
    return false;
  }

  thread_ = std::thread(std::bind(&TimeWheelManager::Run, this));

  return true;
}

void TimeWheelManager::Run() {
  while (true) {
    // 计时 tick
    std::this_thread::sleep_for(std::chrono::milliseconds(timer_step_ms_));

    std::lock_guard<std::mutex> lock(mutex_);
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

void TimeWheelManager::Stop() {
  {
    std::lock_guard<std::mutex> lock(mutex_);
    stop_ = true;
  }

  thread_.join();
}

TimeWheelPtr TimeWheelManager::GetGreatestTimeWheel() {
  if (time_wheels_.empty()) {
    return TimeWheelPtr();
  }

  return time_wheels_.front();
}

TimeWheelPtr TimeWheelManager::GetLeastTimeWheel() {
  if (time_wheels_.empty()) {
    return TimeWheelPtr();
  }

  return time_wheels_.back();
}

void TimeWheelManager::AppendTimeWheel(uint32_t scales, uint32_t scale_unit_ms, const std::string& name) {
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

uint32_t TimeWheelManager::CreateTimerAt(int64_t trigger_time, const TaskCallback& task) {
  if (time_wheels_.empty()) {
    return 0;
  }

  std::lock_guard<std::mutex> lock(mutex_);
  ++s_inc_id;
  GetGreatestTimeWheel()->AddTimer(std::make_shared<Timer>(s_inc_id, trigger_time, 0, task));

  return s_inc_id;
}

uint32_t TimeWheelManager::CreateTimerAfter(int64_t delay_time, const TaskCallback& task) {
  int64_t when = GetNowTimestamp() + delay_time;
  return CreateTimerAt(when, task);
}

uint32_t TimeWheelManager::CreateTimerEvery(int64_t interval_time, const TaskCallback& task) {
  if (time_wheels_.empty()) {
    return 0;
  }

  std::lock_guard<std::mutex> lock(mutex_);
  ++s_inc_id;
  int64_t when = GetNowTimestamp() + interval_time;
  GetGreatestTimeWheel()->AddTimer(std::make_shared<Timer>(s_inc_id, when, interval_time, task));

  return s_inc_id;
}

void TimeWheelManager::CancelTimer(uint32_t timer_id) {
  std::lock_guard<std::mutex> lock(mutex_);
  cancel_timer_ids_.insert(timer_id);
}
