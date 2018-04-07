#include "fcfs_scheduler.h"

using namespace std;


SchedulingDecision* FcfsScheduler::get_next_thread(const Event* event) {
  if (empty()) return nullptr; // return null if there is no thread to run
  // Scheduling Decision, keep default time slice
  SchedulingDecision* dec = new SchedulingDecision();

  // get size before popping thread
  dec->explanation = "Selected from " + to_string(size()) + " threads; will run to completion of burst";

  dec->thread = threads.front(); // add thread to decision
  threads.pop();                 // remove thread from queue
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
