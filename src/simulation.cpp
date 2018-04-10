#include "simulation.h"
#include "types/event.h"
#include <cassert>
#include <fstream>
#include <iostream>

using namespace std;


void Simulation::run(const string& filename) {
  read_file(filename);

  // While their are still events to process, invoke the corresponding methods
  // to handle them.
  while (!events.empty()) {
    const Event* event = events.top();
    events.pop();

    // Invoke the appropriate method on the scheduler for the given event type.
    switch (event->type) {
    case Event::THREAD_ARRIVED:
      handle_thread_arrived(event);
      break;

    case Event::THREAD_DISPATCH_COMPLETED:
      handle_thread_dispatch_completed(event);
      break;

    case Event::PROCESS_DISPATCH_COMPLETED:
      handle_process_dispatch_completed(event);
      break;

    case Event::CPU_BURST_COMPLETED:
      handle_cpu_burst_completed(event);
      break;

    case Event::IO_BURST_COMPLETED:
      handle_io_burst_completed(event);
      break;

    case Event::THREAD_COMPLETED:
      handle_thread_completed(event);
      break;

    case Event::THREAD_PREEMPTED:
      handle_thread_preempted(event);
      break;

    case Event::DISPATCHER_INVOKED:
      handle_dispatcher_invoked(event);
      break;
    }

    // change some of the stats in SystemStats
    stats.total_time = event->time;

    // print out for verbose output
    // output on a non-null event that changed state
    if (event->thread) {
      if (event->thread->current_state != event->thread->previous_state) {
        logger.print_state_transition(event,
                                      event->thread->previous_state,
                                      event->thread->current_state);
      }
    }

    // Free the event's memory.
    delete event;
  }

  for (pair<int, Process*> entry : processes) {
    logger.print_process_details(entry.second);
  }

  logger.print_statistics(calculate_statistics());
}


//==============================================================================
// Event-handling methods
//==============================================================================


void Simulation::handle_thread_arrived(const Event* event) {
  // this is probably handled correctly (done in class)
  assert(event->thread->current_state == Thread::State::NEW);
  // set the thread state to ready
  event->thread->set_state(Thread::State::READY, event->time);
  assert(event->thread->current_state == Thread::State::READY);

  // add the thread to the queue
  scheduler->enqueue(event, event->thread);

  // create a new event to put on the queue
  invoke_dispatcher(event->time);
}


void Simulation::handle_thread_dispatch_completed(const Event* event) {
  assert(event->thread->current_state == Thread::State::READY);
  // set the thread running
  event->thread->set_state(Thread::State::RUNNING, event->time);
  // update the previously running thread
  prev_thread = active_thread;
  active_thread = event->thread;

  // create a new event based on the time slice and thread length
  assert(event->thread->bursts.front()->type == Burst::Type::CPU);
  size_t burst_length = event->thread->bursts.front()->length;
  // make a copy of the scheduling decision since the old one will be deleted
  SchedulingDecision* dec = new SchedulingDecision();
  dec->thread = event->scheduling_decision->thread;
  dec->time_slice = event->scheduling_decision->time_slice;
  dec->explanation = event->scheduling_decision->explanation;
  size_t time_slice = dec->time_slice;

  if (time_slice < burst_length) { // thread gets preempted
    Event* e = new Event(Event::Type::THREAD_PREEMPTED,
                         event->time + time_slice,
                         event->thread,
                         dec);
    add_event(e);
    stats.service_time += time_slice;
  } else {
    Event* e = new Event(Event::Type::CPU_BURST_COMPLETED,
                         event->time + burst_length,
                         event->thread);
    add_event(e);
    stats.service_time += burst_length;
  }
}


void Simulation::handle_process_dispatch_completed(const Event* event) {
  // a process dispatch does the same thing as a thread dispatch, so we call
  // that function here. This function is still used in order for the
  // output to work correctly.
  handle_thread_dispatch_completed(event);
}


void Simulation::handle_cpu_burst_completed(const Event* event) {
  // pop burst from queue
  assert(event->thread->bursts.front()->type == Burst::Type::CPU);
  event->thread->bursts.pop();
  // unset current_thread
  prev_thread = active_thread;
  active_thread = nullptr;

  // invoke the dispatcher
  invoke_dispatcher(event->time);

  // add new event based on if this is the last CPU burst
  Event* e;
  if (event->thread->bursts.size() == 0) { // last CPU burst
    e = new Event(Event::Type::THREAD_COMPLETED, event->time, event->thread);
  } else {
    event->thread->set_state(Thread::State::BLOCKED, event->time);
    e = new Event(Event::Type::IO_BURST_COMPLETED,
                  event->time + event->thread->bursts.front()->length,
                  event->thread);
  }
  add_event(e);
}


void Simulation::handle_io_burst_completed(const Event* event) {
  assert(event->thread->current_state == Thread::State::BLOCKED);
  // set corresponding thread to ready
  event->thread->set_state(Thread::State::READY, event->time);

  // pop the io burst
  assert(event->thread->bursts.front()->type == Burst::Type::IO);
  // change the system stats first
  stats.io_time += event->thread->bursts.front()->length;
  event->thread->bursts.pop();

  // enqueue the thread in the scheduler
  scheduler->enqueue(event, event->thread);

  // invoke the dispatcher
  invoke_dispatcher(event->time);
}


void Simulation::handle_thread_completed(const Event* event) {
  // set the thread state to exit
  assert(event->thread->current_state == Thread::State::RUNNING);
  event->thread->set_state(Thread::State::EXIT, event->time);
  // the dispatcher has already been invoked by this time (in handle_cpu_burst_completed), there is
  // no need to call it again
}


void Simulation::handle_thread_preempted(const Event* event) {
  // set the thread to ready
  assert(event->thread->current_state == Thread::State::RUNNING);
  event->thread->set_state(Thread::State::READY, event->time);

  // decrease cpu burst
  assert(event->thread->bursts.front()->type == Burst::Type::CPU);
  assert((size_t)event->thread->bursts.front()->length > event->scheduling_decision->time_slice);
  event->thread->bursts.front()->length -= event->scheduling_decision->time_slice;

  // enqueue the thread in the scheduler
  scheduler->enqueue(event, event->thread);

  prev_thread = active_thread;
  active_thread = nullptr;
  invoke_dispatcher(event->time);
}


void Simulation::handle_dispatcher_invoked(const Event* event) {
  // get current desicion and set the current thread
  SchedulingDecision* dec = scheduler->get_next_thread(event);
  // check for decision
  if (dec == nullptr) return;
  Thread* next_thread = dec->thread;
  // check for next thread
  if (next_thread == nullptr) return;
  Event* e;
  if (prev_thread == nullptr || next_thread->process != prev_thread->process) { // process switch
    e = new Event(Event::Type::PROCESS_DISPATCH_COMPLETED,
                 event->time + process_switch_overhead,
                 next_thread,
                 dec);
    // change the system stats
    stats.dispatch_time += process_switch_overhead;
  } else { // thread switch
    e = new Event(Event::Type::THREAD_DISPATCH_COMPLETED,
                 event->time + thread_switch_overhead,
                 next_thread,
                 dec);
    stats.dispatch_time += thread_switch_overhead;
  }
  add_event(e);

  // the logger won't print for DISPATCHER_INVOKED since it is called with a nullptr thread,
  // call it in this function for the custom message
  logger.print_verbose(event, e->thread, dec->explanation);

  active_thread = next_thread; // set here to show that the processor is busy
}


void Simulation::invoke_dispatcher(const int time) {
  // if the provessor is idle, add a dispatch event
  if (active_thread == nullptr) {
    Event* e = new Event(Event::Type::DISPATCHER_INVOKED, time, nullptr);
    add_event(e);
  }
}


//==============================================================================
// Utility methods
//==============================================================================


void Simulation::add_event(Event* event) {
  if (event != nullptr) {
    events.push(event);
  }
}


void Simulation::read_file(const string& filename) {
  ifstream file(filename.c_str());

  if (!file) {
    cerr << "Unable to open simulation file: " << filename << endl;
    exit(EXIT_FAILURE);
  }

  size_t num_processes;

  // Read the total number of processes, as well as the dispatch overheads.
  file >> num_processes >> thread_switch_overhead >> process_switch_overhead;

  // Read in each process.
  for (size_t p = 0; p < num_processes; p++) {
    Process* process = read_process(file);

    processes[process->pid] = process;
  }
}


Process* Simulation::read_process(istream& in) {
  int pid, type;
  size_t num_threads;

  // Read in the process ID, its type, and the number of threads.
  in >> pid >> type >> num_threads;

  // Create the process and register its existence in the processes map.
  Process* process = new Process(pid, (Process::Type) type);

  // Read in each thread in the process.
  for (size_t tid = 0; tid < num_threads; tid++) {
    process->threads.push_back(read_thread(in, tid, process));
  }

  return process;
}


Thread* Simulation::read_thread(istream& in, int tid, Process* process) {
  int arrival_time;
  size_t num_cpu_bursts;

  // Read in the thread's arrival time and its number of CPU bursts.
  in >> arrival_time >> num_cpu_bursts;

  Thread* thread = new Thread(arrival_time, tid, process);

  // Read in each burst in the thread.
  for (size_t n = 0, burst_length; n < num_cpu_bursts * 2 - 1; n++) {
    in >> burst_length;

    Burst::Type burst_type = (n % 2 == 0)
        ? Burst::CPU
        : Burst::IO;

    thread->bursts.push(new Burst(burst_type, burst_length));
  }

  // Add an arrival event for the thread.
  events.push(new Event(Event::THREAD_ARRIVED, thread->arrival_time, thread));

  return thread;
}


SystemStats Simulation::calculate_statistics() {
  stats.total_cpu_time = stats.service_time + stats.dispatch_time;
  stats.total_idle_time = stats.total_time - stats.total_cpu_time;
  stats.cpu_utilization = (double)stats.total_cpu_time / (double)stats.total_time * 100.0;
  stats.cpu_efficiency = (double)stats.service_time / (double)stats.total_time * 100.0;

  // sum up all the threads and their reponse and turnaround times
  for (pair<int, Process*> entry : processes) {
    Process* proc = entry.second;
    stats.thread_counts[proc->type] += proc->threads.size();

    double res_time = 0.0;
    double ta_time = 0.0;
    for (Thread* t : proc->threads) {
      res_time += t->response_time();
      ta_time += t->turnaround_time();
    }
    stats.avg_thread_response_times[proc->type] += res_time;
    stats.avg_thread_turnaround_times[proc->type] += ta_time;
  }
  
  // make the stats averages
  for (int i = 0; i < 4; i++) {
    // don't divide by zero
    if (stats.thread_counts[i] > 0) {
      stats.avg_thread_response_times[i] /= stats.thread_counts[i];
      stats.avg_thread_turnaround_times[i] /= stats.thread_counts[i];
    }
  }

  return stats;
}
