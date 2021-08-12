//
// Created by yww on 2021/8/4.
//

#ifndef PA3__BLOOM_FILTER_STD_ATOMIC_H_
#define PA3__BLOOM_FILTER_STD_ATOMIC_H_

#include <atomic>
#include "bloom_filter_base.h"

static const int kTableLen = kNumSlots / 8;
std::atomic<uint8_t> table[kTableLen] __attribute__((aligned(64)));

//XXH64_hash_t GetHash1(const char *str) {
//  return XXH3_64bits_withSeed(str, kStrLen, /* Seed */ 238762743235634621ll);
//}
//XXH64_hash_t GetHash2(const char *str) {
//  return XXH3_64bits_withSeed(str, kStrLen, /* Seed */ 294857290343498117ll);
//}

void Init() {}

void Insert(const char *str) {
  XXH64_hash_t hash = GetHash(str);
  XXH64_hash_t hash1 = hash % kNumSlots;
  XXH64_hash_t hash2 = (hash / kNumSlots) % kNumSlots;
  for (int i = 0; i < kNumHashFunctions; i++) {
    int pos = (hash1 + i * hash2) % kNumSlots;
    table[pos / 8] |= 1 << (pos % 8);
  }
}
bool Query(const char *str) {
  XXH64_hash_t hash = GetHash(str);
  XXH64_hash_t hash1 = hash % kNumSlots;
  XXH64_hash_t hash2 = (hash / kNumSlots) % kNumSlots;
  for (int i = 0; i < kNumHashFunctions; i++) {
    int pos = (hash1 + i * hash2) % kNumSlots;
    if (!((table[pos / 8] >> (pos % 8)) & 1)) {
      return false;
    }
  }
  return true;
}

#endif //PA3__BLOOM_FILTER_STD_ATOMIC_H_
