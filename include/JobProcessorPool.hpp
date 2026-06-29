//
// Created by Nick on 5/29/2026.
//

#ifndef CONANFILE_PY_THREADPOOL_HPP
#define CONANFILE_PY_THREADPOOL_HPP

#include <vector>

#include "JobProcessor.hpp"

namespace NK {
class JobProcessorPool {
public:
  JobProcessorPool(int nThreads);
  ~JobProcessorPool();

public:
  JobProcessor& get(int idx);

private:
  std::vector<JobProcessor> mvecJobProcessors;
};
}

#endif  // CONANFILE_PY_THREADPOOL_HPP
