//
// Created by yww on 2021/8/2.
//

#ifndef PA3__BLOOM_FILTER_PAT_MUTEX_H_
#define PA3__BLOOM_FILTER_PAT_MUTEX_H_

#include <immintrin.h>

#include <mutex>
#include <random>
#include "bloom_filter_base.h"


static const int kBlockLen = kBlockSize / 256; // Length of each block

__m256i table[kNumBlocks * kBlockLen]  __attribute__((aligned(64)));
__m256i pattern[kNumPatterns][kBlockLen]  __attribute__((aligned(64)));
std::mutex lock[kNumBlocks];

void Init() {
  for (int i = 0; i < kNumBlocks * kBlockLen; i++) {
    table[i] = _mm256_setzero_si256();
  }

  unsigned int temp[kBlockSize / 32];
  std::mt19937 gen(183761); // Use the same seed
  std::uniform_int_distribution<> next(0, kBlockSize - 1);
  for (int i = 0; i < kNumPatterns; i++) {
    memset(temp, 0, sizeof temp);
    for (int j = 0; j < kNumHashFunctions; j++) {
      int pos;
      do {
        pos = next(gen);
      } while ((temp[pos / 32] >> (pos % 32)) & 1);
      temp[pos / 32] |= 1u << (pos % 32);
    }

    for (int j = 0; j < kBlockLen; j++) {
      pattern[i][j] = _mm256_loadu_si256(((const __m256i *) temp) + j);
    }
  }
}

void Insert(const char *str) {
  XXH64_hash_t hash = GetHash(str);
  int block_id = hash % kNumBlocks;
  int pattern_id = (hash / kNumBlocks) % kNumPatterns;
  lock[block_id].lock();
  for (int i = 0; i < kBlockLen; i++) {
    table[block_id * kBlockLen + i] = _mm256_or_si256(table[block_id * kBlockLen + i], pattern[pattern_id][i]);
  }
  lock[block_id].unlock();
}

bool Query(const char *str) {
  XXH64_hash_t hash = GetHash(str);
  int block_id = hash % kNumBlocks;
  int pattern_id = (hash / kNumBlocks) % kNumPatterns;
  for (int i = 0; i < kBlockLen; i++) {
    __m256i temp = _mm256_or_si256(table[block_id * kBlockLen + i], pattern[pattern_id][i]);
    __m256i mask = _mm256_cmpeq_epi32(temp, table[block_id * kBlockLen + i]);
    int res = _mm256_movemask_epi8(mask);
    if (res != -1) {
      return false;
    }
  }
  return true;
}

#endif //PA3__BLOOM_FILTER_PAT_MUTEX_H_
