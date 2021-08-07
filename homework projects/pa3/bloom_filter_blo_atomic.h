//
// Created by yww on 2021/8/6.
//

#ifndef PA3__BLOOM_FILTER_BLO_ATOMIC_H_
#define PA3__BLOOM_FILTER_BLO_ATOMIC_H_

#include <atomic>
#include "bloom_filter_base.h"

static const int kTableLen = kNumSlots / 8;
std::atomic<uint8_t> table[kTableLen] __attribute__((aligned(64)));

class Entry {
 public:
  std::atomic<uint8_t> *table_;
};
Entry entry;

void Init() {
  entry.table_ = table;
}

void Insert(Entry &entry, const char *str) {
  XXH64_hash_t hash = GetHash(str);
  int block_id = hash % kNumBlocks;
  XXH64_hash_t hash1 = (hash / kNumBlocks) % kBlockSize;
  XXH64_hash_t hash2 = (hash / kNumBlocks / kBlockSize) % kBlockSize;
  for (int i = 0; i < kNumHashFunctions; i++) {
    int pos = block_id * kBlockSize + (hash1 + i * hash2) % kBlockSize;
    entry.table_[pos / 8] |= 1 << (pos % 8);
  }
}
bool Query(Entry &entry, const char *str) {
  XXH64_hash_t hash = GetHash(str);
  int block_id = hash % kNumBlocks;
  XXH64_hash_t hash1 = (hash / kNumBlocks) % kBlockSize;
  XXH64_hash_t hash2 = (hash / kNumBlocks / kBlockSize) % kBlockSize;
  for (int i = 0; i < kNumHashFunctions; i++) {
    int pos = block_id * kBlockSize + (hash1 + i * hash2) % kBlockSize;
    if (!((entry.table_[pos / 8] >> (pos % 8)) & 1)) {
      return false;
    }
  }
  return true;
}

#endif //PA3__BLOOM_FILTER_BLO_ATOMIC_H_
