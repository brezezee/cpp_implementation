#include "TimeWheel.h"
#include <iostream>

TimeWheel::TimeWheel(uint32_t scales, uint32_t scale_unit_ms, const std::string& name)
    : name_(name)
    , current_index_(0)
    , scales_(scales)
    , ms_pre_scale_(scale_unit_ms)
    , slots_(scales)
    , greater_level_tw_(nullptr)
    , less_level_tw_(nullptr) {
}

int64_t TimeWheel::GetWheelTime() const {
  int64_t time = current_index_ * ms_pre_scale_;
  if (less_level_tw_ != nullptr) {  // 加上下级更精细的time
    time += less_level_tw_->GetWheelTime();
  }

  return time;
}

void TimeWheel::AddTimer(spTimer timer) {
  int64_t less_tw_time = 0;
  if (less_level_tw_ != nullptr) {
    less_tw_time = less_level_tw_->GetWheelTime();
  }
  // 触发时间与当前时间的差值
  int64_t diff = timer->trigger_time() + less_tw_time - GetNowTimestamp();

  // 当触发间隔大于当前时间轮的刻度时，直接哈希到当前时间轮(因为暂时不会触发)
  if (diff >= ms_pre_scale_) {
    size_t idx = (current_index_ + diff / ms_pre_scale_) % scales_;
    slots_[idx].push_back(timer);
    return;
  }

  // 如果小于当前时间轮的刻度，则递归寻找满足它精度的时间轮
  if (less_level_tw_ != nullptr) {
    less_level_tw_->AddTimer(timer);
    return;
  }

  slots_[current_index_].push_back(timer);
}

void TimeWheel::Increase() {
  // Increase the time wheel.
  ++current_index_;
  if (current_index_ < scales_) {
    return;
  }

  current_index_ = current_index_ % scales_;
  // 向上进位  当前轮已经转了一圈， 需要把上一级中进位后的刻度里的定时器取出，重新放入下级
  // 精细刻度的时间轮中，因为可能之前定时器因为相对触发时间太长，导致未放入精细刻度
  if (greater_level_tw_ != nullptr) {
    greater_level_tw_->Increase();
    std::list<spTimer> slot = std::move(greater_level_tw_->TakeoutSlot());
    for (auto&& timer : slot) {  // 重新放入
      AddTimer(timer);
    }
  }
}

std::list<spTimer> TimeWheel::TakeoutSlot() {
  std::list<spTimer> slot;
  slot = std::move(slots_[current_index_]);
  return slot;
}
