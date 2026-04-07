//
// Created by Nick on 3/31/2026.
//

#ifndef MYGAMEPROJECT_CRTPSINGLETON_H
#define MYGAMEPROJECT_CRTPSINGLETON_H


namespace NK {
  template <typename T>
  class CRTPSingleton {
  public:
    static T& getInstanceForThread() {
      thread_local T instance;
      return instance;
    }

    static T& getInstanceGlobal() {
      static T instance;
      return instance;
    }

  public:
    // Delete copy/move to enforce Singleton pattern
    CRTPSingleton(const CRTPSingleton&) = delete;
    CRTPSingleton& operator=(const CRTPSingleton&) = delete;

  protected:
    CRTPSingleton() = default;
    virtual ~CRTPSingleton() = default;
  };
}


#endif //MYGAMEPROJECT_CRTPSINGLETON_H
