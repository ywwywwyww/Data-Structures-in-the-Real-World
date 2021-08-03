//
// Created by yww on 2021/8/2.
//

#ifndef PA3__BLOCKED_BLOOM_FILTER_SINGLE_THREAD_H_
#define PA3__BLOCKED_BLOOM_FILTER_SINGLE_THREAD_H_

#include <immintrin.h>

#include "blocked_bloom_filter_base.h"

#include <set>
#include <random>


/**
 * The single thread version, use AVX2 instructions
 */
class BlockedBloomFilterSingleThread : public BlockedBloomFilterBase {
 public:
  static const int kBlockLen = kBlockSize / 256; // Length of each block

  BlockedBloomFilterSingleThread() {
    for (int i = 0; i < kNumBlocks * kBlockLen; i++) {
      table_[i] = _mm256_setzero_si256();
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
        } while((temp[pos / 32] >> (pos % 32)) & 1);
        temp[pos / 32] |= 1u << (pos % 32);
      }

      for (int j = 0; j < kBlockLen; j++) {
        pattern_[i][j] = _mm256_loadu_si256(((const __m256i*)temp) + j);
      }
    }
  }

  void Insert(const char *str) override {
    int block_id = GetBlockId(str);
    int pattern_id = GetPatternId(str);
    for (int i = 0; i < kBlockLen; i++) {
      table_[block_id * kBlockLen + i] =
          _mm256_or_si256(table_[block_id * kBlockLen + i], pattern_[pattern_id][i]);
    }
  }

  bool Query(const char *str) override {
    int block_id = GetBlockId(str);
    int pattern_id = GetPatternId(str);
    for (int i = 0; i < kBlockLen; i++) {
      __m256i temp = _mm256_or_si256(table_[block_id * kBlockLen + i], pattern_[pattern_id][i]);
      __m256i mask = _mm256_cmpeq_epi32(temp, table_[block_id * kBlockLen + i]);
      int res = _mm256_movemask_epi8(mask);
      if (res != 0xff) {
        return false;
      }
    }
    return true;
  }

  __m256i table_[kNumBlocks * kBlockLen];
  __m256i pattern_[kNumPatterns][kBlockLen];
} __attribute__((aligned(64)));

#endif //PA3__BLOCKED_BLOOM_FILTER_SINGLE_THREAD_H_
