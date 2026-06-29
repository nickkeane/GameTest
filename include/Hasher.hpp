//
// Created by Nick on 4/19/2026.
//

#ifndef CONANFILE_PY_HASHER_HPP
#define CONANFILE_PY_HASHER_HPP

#include <cstdint>
#include <concepts>
#include <ranges>
#include <span>

namespace NK {
  /**
   * Ensure we have a contiguous and random access range of items
   * i.e. const char*, std::string, std::array<T>, std::vector<T>
   * and ensure each element is trivially copyable i.e. scalar or simple
   **/
  template<typename T>
  concept CONHashableRange = std::ranges::contiguous_range<T> // Continuouse Array Type (const char*, std::string, std::array<T>, std::vector<T>, etc.)
    && std::ranges::sized_range<T> // Ensure the range has a known .size()
    && std::is_trivially_copyable_v<std::ranges::range_value_t<T>>;

  class Hasher {
    public:
      /**
       * Compile-time hash function suitable for game hashing
       */
      template<CONHashableRange T>
      static constexpr uint64_t hashFNV1a(T range) {
        // See http://www.isthe.com/chongo/tech/comp/fnv/index.html#FNV-param
        static constexpr uint64_t offsetBasis = 0xcbf29ce484222325ULL; // 14695981039346656037
        static constexpr uint64_t prime = 0x100000001b3ULL; // 1099511628211

        // TODO: There might be a conversion issue since std::as_bytes produces unsigned char
        // and the hashing alg uses char? or was using char? does it matter?
        auto bytes = std::as_bytes(std::span(range));

        uint64_t hash = offsetBasis;
        for (std::byte const b : bytes) {
          hash ^= static_cast<uint64_t>(std::to_integer<uint8_t>(b));
          hash *= prime;
        }
        return hash;
      }

    /**
     * Compile-time hash function suitable for game hashing (32-bit)
     */
    template<CONHashableRange T>
    static constexpr uint32_t hashFNV1a32(T range) {
        // See http://www.isthe.com/chongo/tech/comp/fnv/index.html#FNV-param
        static constexpr uint32_t offsetBasis = 0x811c9dc5UL; // 2166136261
        static constexpr uint32_t prime = 0x1000193UL; // 16777619

        auto bytes = std::as_bytes(std::span(range));

        uint32_t hash = offsetBasis;
        for (char c : bytes) {
          hash ^= static_cast<uint32_t>(c);
          hash *= prime;
        }
        return hash;
      }
    };
}  // namespace NK
#endif //CONANFILE_PY_HASHER_HPP
