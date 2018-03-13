#include "round_robin_scheduler.h"

using namespace std;


SchedulingDecision* RoundRobinScheduler::get_next_thread(const Event* event) {
  // TODO: implement me
  return nullptr;
}


void RoundRobinScheduler::enqueue(const Event* event, Thread* thread) {
  // TODO: implement me
}


bool RoundRobinScheduler::should_preempt_on_arrival(const Event* event) const {
  // TODO: implement me
  return false;
}


size_t RoundRobinScheduler::size() const {
  // TODO: implement me
  return 0;
}
