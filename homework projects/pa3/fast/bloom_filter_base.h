//
// Created by yww on 2021/8/2.
//

#ifndef PA3__BLOOM_FILTER_BASE_H_
#define PA3__BLOOM_FILTER_BASE_H_


#include <cmath>

#define XXH_INLINE_ALL
#include "xxHash/xxhash.c"


constexpr int kStrLen = 16; // Length of each string

constexpr int kCacheLineSize = 64; // Bytes
//constexpr int kL2CacheSize = 512 * 1024; // Bytes
constexpr int kL3CacheSize = 8 * 1024 * 1024; // Bytes

constexpr int kBlockSize = kCacheLineSize * 8; // The size of each block in bits
constexpr int kNumKeys = 10000000; // Number of different keys
constexpr int kNumSlots = 1 << 27; // Number of slots in the bloom filter
constexpr int kNumHashFunctions = int((double)kNumSlots / kNumKeys * M_LN2); // Number of hash functions
constexpr int kNumBlocks = (kNumSlots + kBlockSize - 1) / kBlockSize; // Number of blocks
constexpr int kNumPatterns = /* int((kL3CacheSize * 0.5) / (kBlockSize / 8)) */ 65536; // Number of patterns, use 50% of the L3 cache

/**
 * Use this hash function to generate different hash values
 * @param str
 * @return Hash value
 */
XXH64_hash_t GetHash(const char *str) {
  return XXH3_64bits_withSeed(str, kStrLen, /* Seed */ 293542968347621986ll);
}
//XXH64_hash_t GetBlockId(const char *str) {
//  XXH64_hash_t hash = XXH3_64bits_withSeed(str, kStrLen, /* Seed */ 321672423451412311ll);
//  return hash % kNumBlocks;
//}
//XXH64_hash_t GetPatternId(const char *str) {
//  XXH64_hash_t hash = XXH3_64bits_withSeed(str, kStrLen, /* Seed */ 123236235612231983ll);
//  return hash % kNumPatterns;
//}

#endif //PA3__BLOOM_FILTER_BASE_H_
