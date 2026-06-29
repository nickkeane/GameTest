//
// Created by Nick on 5/30/2026.
//

#ifndef CONANFILE_PY_JOBPROCESSOR_HPP
#define CONANFILE_PY_JOBPROCESSOR_HPP

#include <deque>
#include <mutex>

#include "IJob.hpp"

namespace NK {
  class JobProcessor {
  public:
    void add(IJob* pJob);
    void addPriority(IJob* pJob);
    void remove(IJob* pJob);
    void removeAll(IJob* pJob);
    void clear();

  public:
    void operator()();

  private:
    std::deque<IJob*> mqueueCurrentJobs;
    std::mutex mmtxJobQueueRW;
  };
}

#endif  // CONANFILE_PY_JOBPROCESSOR_HPP
