//
// Created by yww on 2021/8/2.
//

#ifndef PA3__BLOOM_FILTER_PAT_ATOMIC_H_
#define PA3__BLOOM_FILTER_PAT_ATOMIC_H_

#include <atomic>
#include <random>

#include "bloom_filter_base.h"

typedef unsigned long long ull;

static const int kBlockLen = kBlockSize / 64;

std::atomic<ull> table[kNumBlocks * kBlockLen] __attribute__((aligned(64)));
ull pattern[kNumPatterns][kBlockLen] __attribute__((aligned(64)));

void Init() {
  for (int i = 0; i < kNumBlocks * kBlockLen; i++) {
    table[i].store(0);
  }

  std::mt19937 gen(183761); // Use the same seed
  std::uniform_int_distribution<> next(0, kBlockSize - 1);
  for (int i = 0; i < kNumPatterns; i++) {
    for (int j = 0; j < kNumHashFunctions; j++) {
      int pos;
      do {
        pos = next(gen);
      } while ((pattern[i][pos / 64] >> (pos % 64)) & 1);
      pattern[i][pos / 64] |= 1ull << (pos % 64);
    }
  }
}
void Insert(const char *str) {
  XXH64_hash_t hash = GetHash(str);
  int block_id = hash % kNumBlocks;
  int pattern_id = (hash / kNumBlocks) % kNumPatterns;
  for (int i = 0; i < kBlockLen; i++) {
    table[block_id * kBlockLen + i] |= pattern[pattern_id][i];
  }
}
bool Query(const char *str) {
  XXH64_hash_t hash = GetHash(str);
  int block_id = hash % kNumBlocks;
  int pattern_id = (hash / kNumBlocks) % kNumPatterns;
  for (int i = 0; i < kBlockLen; i++) {
    if (pattern[pattern_id][i] & (~table[block_id * kBlockLen + i].load())) {
      return false;
    }
  }
  return true;
}

#endif //PA3__BLOOM_FILTER_PAT_ATOMIC_H_
