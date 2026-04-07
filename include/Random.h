//
// Created by Nick on 3/30/2026.
//

#ifndef MYGAMEPROJECT_RANDOM_H
#define MYGAMEPROJECT_RANDOM_H

#include "CRTPSingleton.h"

#include <concepts>
#include <cstdint>
#include <generator>
#include <iterator>
#include <memory>
#include <random>
#include <ranges>

/**
 * Must initialize seed before use.
 */
namespace NK {
  template<typename T> requires std::is_arithmetic_v<T> && (sizeof(T) >= 2)
  class RandomSource {
  public:
    // ISO C/C++ does not support single byte integrals (char) for use. Upcast to 32-bit integer.
    using ISOCompliantT = std::conditional_t<std::is_integral_v<T> && (sizeof(T) == 1), int32_t, T>;
    using DistribType = std::conditional_t<std::is_integral_v<T>,
    std::uniform_int_distribution<T>,
    std::uniform_real_distribution<T>>;

  public:
    RandomSource(std::mt19937_64& eng, ISOCompliantT min, ISOCompliantT max) : mRandomEngine(eng), mDistrib(min, max) {}
    ~RandomSource() = default;

    ISOCompliantT operator()() {
      return mDistrib(mRandomEngine);
    }

  private:
    std::mt19937_64& mRandomEngine;
    DistribType mDistrib;
  };

  class Random : public CRTPSingleton<Random> {
    friend class CRTPSingleton;
  // public:
  //   static Random& getInstanceForThread();
  private:
    Random();
    ~Random() override;

  public:
    Random& init();
    Random& init(uint64_t nSeed);

    uint32_t coin();
    uint32_t d3();
    uint32_t d4();
    uint32_t d6();
    uint32_t d8();
    uint32_t d10();
    uint32_t d12();
    uint32_t d20();
    uint32_t d100();

    float normal();
    double normalDP();

    template <std::random_access_iterator TIter>
    void shuffle(TIter start, TIter end);

    uint64_t operator()();

    template<typename T>
    RandomSource<T> getSource(T min, T max);

    template<typename T>
    std::generator<T> getGenerator(T min, T max);

    // template <std::integral T>
    // std::generator<T> getIntGenerator(T min, T max);
    // template <std::floating_point T>
    // std::generator<T> getRealGenerator(T min, T max);

    [[nodiscard]] uint64_t getSeed() const;
    std::mt19937_64& getEngine();

  private:
    uint64_t mnSeed;
    std::mt19937_64 mRandomNumberEngine;
    std::uniform_int_distribution<uint32_t> mCoin;
    std::uniform_int_distribution<uint32_t> mD3;
    std::uniform_int_distribution<uint32_t> mD4;
    std::uniform_int_distribution<uint32_t> mD6;
    std::uniform_int_distribution<uint32_t> mD8;
    std::uniform_int_distribution<uint32_t> mD10;
    std::uniform_int_distribution<uint32_t> mD12;
    std::uniform_int_distribution<uint32_t> mD20;
    std::uniform_int_distribution<uint32_t> mD100;
    std::uniform_real_distribution<float> mNormal;
    std::uniform_real_distribution<double> mNormalDP;
  };
}  // namespace NK

#endif //MYGAMEPROJECT_RANDOM_H
