#pragma once

#include <mutex>
#include <vector>
#include <thread>
#include <unordered_set>

#include "TimeWheel.h"

template<int BaseScale = 50>
class TimeWheelManager {
public:
  static TimeWheelManager* GetTimerWheelManager();

  uint32_t CreateTimerAt(int64_t trigger_time, const TaskCallback& task);
  uint32_t CreateTimerAfter(int64_t delay_time, const TaskCallback& task);
  uint32_t CreateTimerEvery(int64_t interval_time, const TaskCallback& task);

  void CancelTimer(uint32_t timer_id);

  bool Start();
  void Stop();

  void AppendTimeWheel(uint32_t scales, uint32_t scale_unit_ms, const std::string& name = "");



private:
  void Run();

  TimeWheelPtr GetGreatestTimeWheel();
  TimeWheelPtr GetLeastTimeWheel();

// 单例相关
private:
  TimeWheelManager(uint32_t basescale = BaseScale);
  ~TimeWheelManager();

  static TimeWheelManager* ptimemanager_;

  struct GC {
    ~GC() {
      if (ptimemanager_) delete ptimemanager_;
    }
  };

  static GC gc;

private:
  std::mutex mutex_;
  std::thread thread_;

  bool stop_;

  std::unordered_set<uint32_t> cancel_timer_ids_;

  uint32_t base_scale_ms_;
  std::vector<TimeWheelPtr> time_wheels_;
};
