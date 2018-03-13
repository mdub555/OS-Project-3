#include "algorithms/priority_scheduler.h"

using namespace std;


SchedulingDecision* PriorityScheduler::get_next_thread(const Event* event) {
  // TODO: implement me
  return nullptr;
}


void PriorityScheduler::enqueue(const Event* event, Thread* thread) {
  // TODO: implement me
}


bool PriorityScheduler::should_preempt_on_arrival(const Event* event) const {
  // TODO: implement me
  return false;
}


size_t PriorityScheduler::size() const {
  // TODO: implement me
  return 0;
}
