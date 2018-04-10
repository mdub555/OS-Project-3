#pragma once
#include "algorithms/scheduler.h"
#include "types/event.h"
#include "types/scheduling_decision.h"
#include "types/thread.h"
#include "algorithms/round_robin_scheduler.h"
#include <vector>
#include <map>


/**
 * Represents a scheduling queue that demotes threads after being preempted.
 */
class MultilevelFeedbackScheduler : public Scheduler {
public:

  MultilevelFeedbackScheduler();


  virtual SchedulingDecision* get_next_thread(const Event* event) override;


  virtual void enqueue(const Event* event, Thread* thread) override;


  virtual bool should_preempt_on_arrival(const Event* event) const override;


  virtual size_t size() const override;

private:
  // implement 8 levels of queues
  const int NUM_QUEUES = 8;
  // give them a constant time slice, the same as the default round robin scheduler
  const int TIME_SLICE = 3;

  // since a MLFQ is implemented with several RR queues, we can use those instead of making more
  // queues
  std::vector<RoundRobinScheduler*> queues;
  
  // use a map to save the queue level of the thread and update when it gets enqueued.
  // the levels start at 0 and go to NUM_QUEUES-1
  std::map<Thread*, int> level_map;
};
