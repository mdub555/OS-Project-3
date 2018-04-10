#pragma once
#include "algorithms/scheduler.h"
#include "types/event.h"
#include "types/scheduling_decision.h"
#include "types/thread.h"
#include "algorithms/fcfs_scheduler.h"


/**
 * Represents a scheduling queue that gives threads of each type a different
 * priority.
 */
class PriorityScheduler : public Scheduler {
public:

  PriorityScheduler();


  virtual SchedulingDecision* get_next_thread(const Event* event) override;


  virtual void enqueue(const Event* event, Thread* thread) override;


  virtual bool should_preempt_on_arrival(const Event* event) const override;


  virtual size_t size() const override;

private:
  // make the number of priority levels into a variable
  const int NUM_PRIORITIES = 4;

  // create a vector of FCFS schedulers
  std::vector<FcfsScheduler*> queues;
};
