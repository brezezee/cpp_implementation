#include <iostream>
#include <string>
#include <thread>
#include <ctime>
#include "TimeWheelManager.h"

std::string timetoStr(){
  // char tmp[64] = {0};
  // time_t t = time(NULL);
  // tm* tm = localtime(&t);
  // sprintf(tmp, "%02d:%02d:%02d", tm->tm_hour, tm->tm_min, tm->tm_sec);
  // return tmp;
  using namespace std::chrono;
  system_clock::time_point cur_time = system_clock::now();
  std::time_t tt;
  tt = system_clock::to_time_t ( cur_time );
  return ctime(&tt);
}
 
int main() {
  
  TimeWheelManager* tw_manager = TimeWheelManager::GetTimerWheelManager();
  // tw_manager->AppendTimeWheel(1000 / timer_step_ms, timer_step_ms, "MillisecondTimeWheel");
  tw_manager->AppendTimeWheel(60, 1000, "SecondTimeWheel");
  // Hour time wheel. 24 scales, 1 scale = 60 * 60 * 1000 ms.
  tw_manager->AppendTimeWheel(24, 60 * 60 * 1000, "HourTimeWheel");
  // Minute time wheel. 60 scales, 1 scale = 60 * 1000 ms.
  tw_manager->AppendTimeWheel(60, 60 * 1000, "MinuteTimeWheel");
  // Second time wheel. 60 scales, 1 scale = 1000 ms.
  
  // Millisecond time wheel. 1000/timer_step_ms scales, 1 scale = timer_step ms.
  
  
  tw_manager->Start();

  tw_manager->CreateTimerAt(GetNowTimestamp() + 10000, []() {
    std::cout << "At now+10s" << std::endl;
  });

  tw_manager->CreateTimerAfter(500, []() {
    std::cout << "After 0.5s" << std::endl;
  });

  std::cout << timetoStr() << std::endl;
  auto timer_id = tw_manager->CreateTimerEvery(5000, []() {
    std::cout << "Every 5s: " << timetoStr() << std::endl;
  });

  tw_manager->CreateTimerEvery(1000, []() {
    // just waste
    int a = 0;
    a = a + 1;
  });

  tw_manager->CreateTimerEvery(30 * 1000, []() {
    std::cout << "Every 30s: " << timetoStr() <<std::endl;
  });

  tw_manager->CreateTimerEvery(60 * 1000, []() {
    std::cout << "Every 1m: " << timetoStr() << std::endl;
  });

  tw_manager->CreateTimerEvery(90 * 1000, []() {
    std::cout << "Every 1.5m: " << timetoStr() << std::endl;
  });
 
  tw_manager->CreateTimerEvery(120 * 1000, []() {
    std::cout << "Every 2m: " << timetoStr() << std::endl;
  });

  std::this_thread::sleep_for(std::chrono::seconds(30));
  tw_manager->CancelTimer(timer_id);

  std::this_thread::sleep_for(std::chrono::minutes(2000));
  tw_manager->Stop();

  return 0;
}