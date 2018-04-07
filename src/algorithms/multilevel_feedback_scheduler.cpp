#include "algorithms/multilevel_feedback_scheduler.h"

using namespace std;


MultilevelFeedbackScheduler::MultilevelFeedbackScheduler() {
  for (int i = 0; i < NUM_QUEUES; i++) {
    queues.push_back(new RoundRobinScheduler(TIME_SLICE));
  }
}

SchedulingDecision* MultilevelFeedbackScheduler::get_next_thread(
    const Event* event) {
  // search through all the queues and get the decision of the first on that isn't empty
  SchedulingDecision* dec;
  for (int i = 0; i < NUM_QUEUES; i++) {
    if (!queues[i]->empty()) {
      dec = queues[i]->get_next_thread(event);
      return dec;
    }
  }
  return nullptr;
}


void MultilevelFeedbackScheduler::enqueue(const Event* event, Thread* thread) {
  // TODO: implement me
  // if the thread isn't in the map, add it to the highest level
  if (level_map.find(thread) == level_map.end()) {
    queues[0]->enqueue(event, thread);
    level_map.insert(std::pair<Thread*, int>(thread, 0));
  } else {
    int level = level_map.at(thread) + 1;
    if (level >= NUM_QUEUES) level = NUM_QUEUES - 1;
    queues[level]->enqueue(event, thread);
    level_map.at(thread) = level;
  }
}


bool MultilevelFeedbackScheduler::should_preempt_on_arrival(
    const Event* event) const {
  return false; // doesn't preempt on arrival
}


size_t MultilevelFeedbackScheduler::size() const {
  int size = 0;
  for (int i = 0; i < NUM_QUEUES; i++) {
    size += queues[i]->size();
  }
  return size;
}
