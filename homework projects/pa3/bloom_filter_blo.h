//
// Created by yww on 2021/8/6.
//

#ifndef PA3__BLOOM_FILTER_BLO_H_
#define PA3__BLOOM_FILTER_BLO_H_


#include <bitset>
#include "bloom_filter_base.h"


static const int kTableLen = kNumSlots / 8;
uint8_t table[kTableLen] __attribute__((aligned(64)));

void Init() {}

void Insert(const char *str) {
  XXH64_hash_t hash = GetHash(str);
  int block_id = hash % kNumBlocks;
  XXH64_hash_t hash1 = (hash / kNumBlocks) % kBlockSize;
  XXH64_hash_t hash2 = (hash / kNumBlocks / kBlockSize) % kBlockSize;
  for (int i = 0; i < kNumHashFunctions; i++) {
    int pos = block_id * kBlockSize + (hash1 + i * hash2) % kBlockSize;
    table[pos / 8] |= 1 << (pos % 8);
  }
}
bool Query(const char *str) {
  XXH64_hash_t hash = GetHash(str);
  int block_id = hash % kNumBlocks;
  XXH64_hash_t hash1 = (hash / kNumBlocks) % kBlockSize;
  XXH64_hash_t hash2 = (hash / kNumBlocks / kBlockSize) % kBlockSize;
  for (int i = 0; i < kNumHashFunctions; i++) {
    int pos = block_id * kBlockSize + (hash1 + i * hash2) % kBlockSize;
    if (!((table[pos / 8] >> (pos % 8)) & 1)) {
      return false;
    }
  }
  return true;
}

#endif //PA3__BLOOM_FILTER_BLO_H_
