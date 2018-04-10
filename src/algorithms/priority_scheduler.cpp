#include "algorithms/priority_scheduler.h"
#include "types/process.h"

using namespace std;


PriorityScheduler::PriorityScheduler() {
  // create the queues for each priority level
  for (int i = 0; i < NUM_PRIORITIES; i++) {
    queues.push_back(new FcfsScheduler());
  }
}


SchedulingDecision* PriorityScheduler::get_next_thread(const Event* event) {
  // search through all the queues and get the decision of the first one that isn't empty
  SchedulingDecision* dec;
  for (int i = 0; i < NUM_PRIORITIES; i++) {
    if (!queues[i]->empty()) {
      dec = queues[i]->get_next_thread(event);
      dec->explanation = "Selected from queue " + to_string(i)
                       + " [S:" + to_string(queues[0]->size())
                       + " I:" + to_string(queues[1]->size())
                       + " N:" + to_string(queues[2]->size())
                       + " B:" + to_string(queues[3]->size())
                       + "]";
      return dec;
    }
  }
  return nullptr;
}


void PriorityScheduler::enqueue(const Event* event, Thread* thread) {
  // add the thread into the queue with the priority of the process it belongs to
  queues[thread->process->type]->enqueue(event, thread);
}


bool PriorityScheduler::should_preempt_on_arrival(const Event* event) const {
  return false; // does not preempt on arrival
}


size_t PriorityScheduler::size() const {
  // sum the size of all the contained queues
  int size = 0;
  for (int i = 0; i < NUM_PRIORITIES; i++) {
    size += queues[i]->size();
  }
  return size;
}
