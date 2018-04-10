#include "round_robin_scheduler.h"

using namespace std;


SchedulingDecision* RoundRobinScheduler::get_next_thread(const Event* event) {
  // get the next thread based on the underlying fcfs queue
  SchedulingDecision* dec = scheduler.get_next_thread(event);
  // set the time slice if a desicion has been made
  if (dec != nullptr) {
    dec->time_slice = time_slice;
    dec->explanation = "Selected from " + to_string(size())
                     + " threads; will run for at most " + to_string(time_slice)
                     + " ticks";
  }
  return dec;
}


void RoundRobinScheduler::enqueue(const Event* event, Thread* thread) {
  scheduler.enqueue(event, thread);
}


bool RoundRobinScheduler::should_preempt_on_arrival(const Event* event) const {
  return false; // RR doesn't preempt on arrival
}


size_t RoundRobinScheduler::size() const {
  return scheduler.size();
}
