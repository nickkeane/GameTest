//
// Created by Nick on 3/30/2026.
//

#include "Random.h"

#include <cassert>
#include <chrono>
#include <iterator>

namespace NK {
  // Random& Random::getInstanceForThread() {
  //   thread_local Random rng;
  //   return rng;
  // }

  Random::Random() : mnSeed(std::random_device{}()), mRandomNumberEngine(mnSeed) {}
  Random::~Random() = default;
// TODO: Is there a way to use the singleton pattern with a custom derived class constructor that takes arguments?
  Random& Random::init() {
    uint64_t nSeed = 0;
    std::random_device rd{};
    while ((nSeed = rd() | static_cast<uint64_t>(rd()) << 32) == 0U) {}
    this->init(nSeed);
    return *this;
  }

  Random& Random::init(uint64_t const nSeed) {
    assert(nSeed > 0);
    this->mnSeed = nSeed;
    this->mRandomNumberEngine = std::mt19937_64(nSeed);

    this->mCoin = std::uniform_int_distribution<uint32_t>(1, 2);
    this->mD3 = std::uniform_int_distribution<uint32_t>(1, 3);
    this->mD4 = std::uniform_int_distribution<uint32_t>(1, 4);
    this->mD6 = std::uniform_int_distribution<uint32_t>(1, 6);
    this->mD8 = std::uniform_int_distribution<uint32_t>(1, 8);
    this->mD10 = std::uniform_int_distribution<uint32_t>(1, 10);
    this->mD12 = std::uniform_int_distribution<uint32_t>(1, 12);
    this->mD20 = std::uniform_int_distribution<uint32_t>(1, 20);
    this->mD100 = std::uniform_int_distribution<uint32_t>(1, 100);

    this->mNormal = std::uniform_real_distribution<float>(0.0f, 1.0f);
    this->mNormalDP = std::uniform_real_distribution<double>(0.0, 1.0);
    return *this;
  }

  uint32_t Random::coin() {
    return this->mCoin(this->mRandomNumberEngine);
  }

  uint32_t Random::d3() {
    return this->mD3(this->mRandomNumberEngine);
  }

  uint32_t Random::d4() {
    return this->mD4(this->mRandomNumberEngine);
  }

  uint32_t Random::d6() {
    return this->mD6(this->mRandomNumberEngine);
  }

  uint32_t Random::d8() {
    return this->mD8(this->mRandomNumberEngine);
  }

  uint32_t Random::d10() {
    return this->mD10(this->mRandomNumberEngine);
  }

  uint32_t Random::d12() {
    return this->mD12(this->mRandomNumberEngine);
  }

  uint32_t Random::d20() {
    return this->mD20(this->mRandomNumberEngine);
  }

  uint32_t Random::d100() {
    return this->mD10(this->mRandomNumberEngine);
  }

  float Random::normal() {
    return this->mNormal(this->mRandomNumberEngine);
  }

  double Random::normalDP() {
    return this->mNormalDP(this->mRandomNumberEngine);
  }

  template <std::random_access_iterator TIter>
  void Random::shuffle(TIter start, TIter end) {
    // Implement Fisher-Yates Shuffler
    using iter_diff_t = std::iterator_traits<TIter>::difference_type;
    iter_diff_t n = std::distance(start, end);
    if (n <= 1)
      return;

    for (iter_diff_t i = n - 1; i > 0; --i) {
      RandomSource<iter_diff_t> rs(this->mRandomNumberEngine, 0, i);
//      std::uniform_int_distribution<iter_diff_t> distrib(0, i);
//      iter_diff_t j = distrib(this->mRandomNumberEngine);
      iter_diff_t j = rs();
      std::iter_swap(start + i, start + j);
    }
  }

  uint64_t Random::getSeed() const {
    return this->mnSeed;
  }

  uint64_t Random::operator()() {
    return this->mRandomNumberEngine();
  }

  std::mt19937_64& Random::getEngine() {
    return this->mRandomNumberEngine;
  }

  template<typename T>
  RandomSource<T> Random::getSource(T min, T max) {
    return RandomSource<T>(min, max);
  }

  template<typename T>
  std::generator<T> Random::getGenerator(T min, T max) {
    return [this, min, max]() -> std::generator<T> {
      RandomSource<T> rndSrc = this->getSource(min, max);
      while (true) {
        co_yield rndSrc();
      }
    };
  }
}  // namespace NK