//
// Created by Nick on 5/29/2026.
//

#ifndef CONANFILE_PY_THREAD_HPP
#define CONANFILE_PY_THREAD_HPP

#include <chrono>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <condition_variable>

namespace NK {
class JobProcessor {
public:
  using Job = std::function<void()>;

public:
  JobProcessor();
  ~JobProcessor();

public:
  bool push(Job fnJob);
  bool pop();
  bool removeOne(Job fnJob);
  bool removeAll(Job fnJob);
  void clear();

public:
  void operator()();

  void start();
  void restart();
  void pause();
  void stopAndWait();

private:
  bool mbIsRunning; // Is this Job Processor currently running?
  std::thread mThread; // Thread spun up to process jobs
  std::queue<Job> mqueJobs; // Job Queue to process
  std::mutex mmtxJobQueue; // Job Queue locking mutex
  std::condition_variable mcvJobQueue; // Job Queue CV
  std::chrono::high_resolution_clock mClock; // Clock for timing operations
};
}

#endif  // CONANFILE_PY_THREAD_HPP
