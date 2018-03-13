#include "algorithms/multilevel_feedback_scheduler.h"

using namespace std;


SchedulingDecision* MultilevelFeedbackScheduler::get_next_thread(
    const Event* event) {
  // TODO: implement me
  return nullptr;
}


void MultilevelFeedbackScheduler::enqueue(const Event* event, Thread* thread) {
  // TODO: implement me
}


bool MultilevelFeedbackScheduler::should_preempt_on_arrival(
    const Event* event) const {
  // TODO: implement me
  return false;
}


size_t MultilevelFeedbackScheduler::size() const {
  // TODO: implement me
  return 0;
}
