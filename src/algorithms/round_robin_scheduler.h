#pragma once
#include "algorithms/scheduler.h"
#include "types/event.h"
#include "types/scheduling_decision.h"
#include "types/thread.h"
#include "algorithms/fcfs_scheduler.h"


/**
 * Represents a scheduling queue that uses simple round-robin.
 */
class RoundRobinScheduler : public Scheduler {
public:

  RoundRobinScheduler(size_t time_slice) : time_slice(time_slice) {}


  virtual SchedulingDecision* get_next_thread(const Event* event) override;


  virtual void enqueue(const Event* event, Thread* thread) override;


  virtual bool should_preempt_on_arrival(const Event* event) const override;


  virtual size_t size() const override;

private:

  /**
   * The length of the time slice for this round-robin queue.
   */
  const size_t time_slice;

  // since RR is just like FCFS, but with a time slice, I can use all the methods that
  // have already been made in a FCFS scheduler
  FcfsScheduler scheduler;
};
