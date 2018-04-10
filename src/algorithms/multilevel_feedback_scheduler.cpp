#include "algorithms/multilevel_feedback_scheduler.h"
#include "types/process.h"

using namespace std;


MultilevelFeedbackScheduler::MultilevelFeedbackScheduler() {
  // create all the levels of queues
  for (int i = 0; i < NUM_QUEUES; i++) {
    queues.push_back(new RoundRobinScheduler(TIME_SLICE));
  }
}

SchedulingDecision* MultilevelFeedbackScheduler::get_next_thread(
    const Event* event) {
  // search through all the queues and get the decision of the first one that isn't empty
  SchedulingDecision* dec;
  for (int i = 0; i < NUM_QUEUES; i++) {
    if (!queues[i]->empty()) {
      dec = queues[i]->get_next_thread(event);
      dec->explanation = "Selected from " + to_string(queues[i]->size()+1)
                       + " threads in level " + to_string(i+1) + "/"
                       + to_string(NUM_QUEUES) + "; will run for at most "
                       + to_string(dec->time_slice) + " ticks";
      return dec;
    }
  }
  return nullptr;
}


void MultilevelFeedbackScheduler::enqueue(const Event* event, Thread* thread) {
  // if the thread isn't in the map, it's new so add it to the level corresponding
  // to it's priority
  if (level_map.find(thread) == level_map.end()) {
    int level = thread->process->type;
    queues[level]->enqueue(event, thread);
    level_map.insert(std::pair<Thread*, int>(thread, level));
  } else {
    // increment the level
    int level = level_map.at(thread) + 1;
    // check if the level is still in the bounds of the scheduler
    if (level >= NUM_QUEUES) level = NUM_QUEUES - 1;
    // enqueue the thread in the corresponding level
    queues[level]->enqueue(event, thread);
    // update the level map
    level_map.at(thread) = level;
  }
}


bool MultilevelFeedbackScheduler::should_preempt_on_arrival(
    const Event* event) const {
  return false; // doesn't preempt on arrival
}


size_t MultilevelFeedbackScheduler::size() const {
  // sum up the size of all the queues we have
  int size = 0;
  for (int i = 0; i < NUM_QUEUES; i++) {
    size += queues[i]->size();
  }
  return size;
}
