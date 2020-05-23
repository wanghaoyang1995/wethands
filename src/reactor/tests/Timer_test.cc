#include "src/reactor/Timer.h"
#include "src/utils/Timestamp.h"
#include <cstdio>
#include <set>
#include <vector>

using namespace wethands;
namespace wethands { class TimerQueue; }

class wethands::TimerQueue {  // friend of TimerIndex
 public:
  TimerQueue() : timers_() {}
  ~TimerQueue() {
    for (const TimerIndex& timer : timers_) {
      delete timer.timer_;
    }
  }

  TimerIndex AddTimer(Timer::TimerCallback callback,
                   Timestamp when,
                   double interval) {
    Timer* p = new Timer(std::move(callback), when, interval);  // 记得 delete
    TimerIndex timerIndex(p, p->Expiration(), p->Sequence());
    timers_.insert(timerIndex);
    return timerIndex;
  }

  bool RemoveTimer(TimerIndex timerIndex) {
    size_t num = timers_.erase(timerIndex);
    if (num > 0) {
      delete timerIndex.timer_;  // 在这里 delete
      return true;
    }
    return false;
  }

  void PrintQueue() {
    for (const TimerIndex& timerIndex : timers_) {
      printf("exp: %s, itvl: %f, rept: %d, seq: %lu\n",
             timerIndex.timer_->Expiration().ToFormattedString().c_str(),
             timerIndex.timer_->Interval(),
             timerIndex.timer_->IsRepeated(),
             timerIndex.timer_->Sequence());
    }
  }

 private:
  std::set<TimerIndex> timers_;
};

void print() {
  printf("timer callback\n");
}

void test() {
  TimerQueue queue;
  queue.PrintQueue();

  std::vector<TimerIndex> vec;  // 记录已添加的 Timer
  for (int i = 0; i < 10; ++i) {
    Timestamp expiration = AddSeconds(Timestamp::Now(), i * 3600);
    vec.push_back(queue.AddTimer(print, expiration, i));
  }
  queue.PrintQueue();

  queue.RemoveTimer(vec[0]);
  queue.PrintQueue();

}

int main() {
  test();
  return 0;
}
