//
// Created by yww on 2021/8/2.
//

#ifndef PA3__BLOCKED_BLOOM_FILTER_ATOMIC_H_
#define PA3__BLOCKED_BLOOM_FILTER_ATOMIC_H_

#include <atomic>

#include "blocked_bloom_filter_base.h"

typedef unsigned long long ull;

class BlockedBloomFilterAtomic : public BlockedBloomFilterBase {
 public:
  static const int kBlockLen = kBlockSize / 64;
  BlockedBloomFilterAtomic() {
    for (int i = 0; i < kNumBlocks * kBlockLen; i++) {
      table_[i].store(0);
    }

    std::mt19937 gen(183761); // Use the same seed
    std::uniform_int_distribution<> next(0, kBlockSize - 1);
    for (int i = 0; i < kNumPatterns; i++) {
      for (int j = 0; j < kBlockLen; j++) {
        pattern_[i][j] = 0;
      }
      for (int j = 0; j < kNumHashFunctions; j++) {
        int pos;
        do {
          pos = next(gen);
        } while((pattern_[i][pos / 64] >> (pos % 64)) & 1);
        pattern_[i][pos / 64] |= 1ull << (pos % 64);
      }
    }
  }
  void Insert(const char *str) override {
    int block_id = GetBlockId(str);
    int pattern_id = GetPatternId(str);
    for (int i = 0; i < kBlockLen; i++) {
      table_[block_id * kBlockLen + i] |= pattern_[pattern_id][i];
    }
  }
  bool Query(const char *str) override {
    int block_id = GetBlockId(str);
    int pattern_id = GetPatternId(str);
    bool flag = true;
    for (int i = 0; flag && i < kBlockLen; i++) {
      flag &= !(pattern_[pattern_id][i] & (~table_[block_id * kBlockLen + i].load()));
    }
    return flag;
  }

 private:
  std::atomic<ull> table_[kNumBlocks * kBlockLen];
  ull pattern_[kNumPatterns][kBlockLen];

} __attribute__((aligned(64)));

#endif //PA3__BLOCKED_BLOOM_FILTER_ATOMIC_H_
