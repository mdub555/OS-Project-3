#include "types/thread.h"
#include <fstream>


// set the thread's state where the change in state happens at `time`
void Thread::set_state(State state, size_t time) {
  // make sure the state is actually changing
  assert(current_state != state);
  if (current_state == NEW && state == READY) {
  } else if (current_state == READY && state == RUNNING) {
    if (start_time > time) start_time = time;
  } else if (current_state == RUNNING && state == READY) {
    service_time += (time - state_change_time);
  } else if (current_state == RUNNING && state == BLOCKED) {
    service_time += (time - state_change_time);
  } else if (current_state == BLOCKED && state == READY) {
    io_time += (time - state_change_time);
  } else if (current_state == RUNNING && state == EXIT) {
    end_time = time;
    service_time += (time - state_change_time);
  } else { // anything else
    printf("Unexpected state change from %i to %i\n", current_state, state);
    return;
  }
 
  // set when the state change occurred
  state_change_time = time;
  // update the previous state
  previous_state = current_state;
  // change the state
  current_state = state;
}
