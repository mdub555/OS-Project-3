#pragma once
#include "burst.h"
#include <cassert>
#include <queue>


// Forward declaration (circular dependency resolution).
struct Process;


/**
 * Represents a thread of execution, with one or more CPU + I/O bursts.
 */
struct Thread {

  /**
   * Represents valid states in which threads / processes can exist.
   */
  enum State {
    NEW,
    READY,
    RUNNING,
    BLOCKED,
    EXIT
  };

  /**
   * The ID of this thread
   */
  int id;

  /**
   * The time at which this thread arrived
   */
  size_t arrival_time = -1;

  /**
   * The time at which this thread was first executed.
   */
  size_t start_time = -1;

  /**
   * The time at which this thread finished executing.
   */
  size_t end_time = -1;

  /**
   * The total amount of time spent executing on the CPU for this thread.
   */
  size_t service_time = 0;

  /**
   * The total amount of time spent doing I/O for this thread.
   */
  size_t io_time = 0;

  /**
   * The absolute time at which the last state change occurred.
   */
  size_t state_change_time = -1;

  /**
   * The current state of the thread.
   */
  State current_state = NEW;

  /**
   * The previous state of the thread.
   */
  State previous_state;

  /**
   * All bursts that are a part of this thread.
   */
  std::queue<Burst*> bursts;

  /**
   * The process associated with this thread.
   */
  Process* process;

  /**
   * Constructor.
   */
  Thread(size_t arrival, int id, Process* process) :
      id(id),
      arrival_time(arrival),
      process(process) {}

  size_t response_time() const {
    assert(current_state == EXIT);
    assert(start_time > arrival_time);
    return start_time - arrival_time;
  }
  
  
  size_t turnaround_time() const {
    assert(current_state == EXIT);
    assert(end_time > arrival_time);
    return end_time - arrival_time;
  }

  void set_state(State state, size_t time);
};
