//
// Created by yww on 2021/8/4.
//

#ifndef PA3__BLOOM_FILTER_ATOMIC_H_
#define PA3__BLOOM_FILTER_ATOMIC_H_

#include <atomic>
#include "blocked_bloom_filter_base.h"

class BloomFilterAtomic : public BlockedBloomFilterBase {
 public:
  static const int kTableLen = kNumSlots / 8;

  BloomFilterAtomic() {
    for (int i = 0; i < kTableLen; i++) {
      table_[i].store(0);
    }
  }
  void Insert(const char *str) override {
    XXH64_hash_t hash1 = GetHash1(str);
    XXH64_hash_t hash2 = GetHash2(str);
    for (int i = 0; i < kNumHashFunctions; i++) {
      XXH64_hash_t hash = hash1 + i * hash2;
      int pos = hash % kNumSlots;
      table_[pos / 8] |= 1 << (pos % 8);
    }
  }
  bool Query(const char *str) override {
    XXH64_hash_t hash1 = GetHash1(str);
    XXH64_hash_t hash2 = GetHash2(str);
    for (int i = 0; i < kNumHashFunctions; i++) {
      XXH64_hash_t hash = hash1 + i * hash2;
      int pos = hash % kNumSlots;
      if (!((table_[pos / 8] >> (pos % 8)) & 1)) {
        return false;
      }
    }
    return true;
  }
  XXH64_hash_t GetHash1(const char *str) {
    return XXH3_64bits_withSeed(str, kStrLen, /* Seed */ 238762743235634621ll);
  }
  XXH64_hash_t GetHash2(const char *str) {
    return XXH3_64bits_withSeed(str, kStrLen, /* Seed */ 294857290343498117ll);
  }
 private:
  std::atomic<uint8_t> table_[kTableLen];
} __attribute__((aligned(64)));

#endif //PA3__BLOOM_FILTER_ATOMIC_H_
