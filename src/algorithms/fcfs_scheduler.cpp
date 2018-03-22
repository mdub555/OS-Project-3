#include "fcfs_scheduler.h"

using namespace std;


SchedulingDecision* FcfsScheduler::get_next_thread(const Event* event) {
  // Scheduling Decision, keep default time slice
  SchedulingDecision* dec = new SchedulingDecision();
  dec->thread = threads.front(); // add thread to decision
  threads.pop();                 // remove thread from queue
  dec->explanation = "FCFS, this is the next thread in the queue";
  return dec;
}


void FcfsScheduler::enqueue(const Event* event, Thread* thread) {
  if (thread) threads.push(thread); // add it to the back of the queue
}


bool FcfsScheduler::should_preempt_on_arrival(const Event* event) const {
  return false; // FCFS doesn't reempt
}


size_t FcfsScheduler::size() const {
  return threads.size(); // get the size of the queue
}
