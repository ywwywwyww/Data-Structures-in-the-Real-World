//
// Created by yww on 2021/8/3.
//

#ifndef PA3__BLOOM_FILTER_STD_H_
#define PA3__BLOOM_FILTER_STD_H_

#include <bitset>
#include "bloom_filter_base.h"

std::bitset<kNumSlots> table __attribute__((aligned(64)));


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
    table.set(pos);
  }
}
bool Query(const char *str) {
  XXH64_hash_t hash = GetHash(str);
  XXH64_hash_t hash1 = hash % kNumSlots;
  XXH64_hash_t hash2 = (hash / kNumSlots) % kNumSlots;
  for (int i = 0; i < kNumHashFunctions; i++) {
    int pos = (hash1 + i * hash2) % kNumSlots;
    if (!table[pos]) {
      return false;
    }
  }
  return true;
}

#endif //PA3__BLOOM_FILTER_STD_H_
