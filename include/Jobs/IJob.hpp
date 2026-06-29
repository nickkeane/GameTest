//
// Created by Nick on 5/30/2026.
//

#ifndef CONANFILE_PY_JOBS_HPP
#define CONANFILE_PY_JOBS_HPP

struct IJob {
  virtual void operator()() = 0;
};

template <typename R>
struct JobR : public IJob {
  R& getReturn();

  void operator()();
};

template <typename R>
class JobR : public IJob {
public:
  R& getReturn();

  void operator()();
private:
  R mReturn;
};

template <typename R, typename A1>
class JobRA1 : public IJob {
  public:
  R& getReturn();
  A1& getArg1();

  void operator()();
  private:
  R mReturn;
  A1 mArg1;
};

#endif  // CONANFILE_PY_IACTION_HPP
