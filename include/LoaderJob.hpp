//
// Created by Nick on 5/29/2026.
//

#ifndef CONANFILE_PY_LOADERJOB_HPP
#define CONANFILE_PY_LOADERJOB_HPP

#include <future>
#include <string>

template <typename T>
class Loader {
  struct Job {
    std::string mszFile;
    T* mpResource;
  };

  std::promise<T*> asyncLoad(std::string const& szFile);

};

#endif  // CONANFILE_PY_LOADERJOB_HPP
