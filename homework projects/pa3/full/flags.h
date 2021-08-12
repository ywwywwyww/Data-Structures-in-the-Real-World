//
// Created by yww on 2021/8/6.
//

#ifndef PA3__FLAGS_H_
#define PA3__FLAGS_H_

/**
 * Optional flags (the notations are the same as in the paper):
 * BLOOM_FILTER_STD        : std (bloom filter)
 * BLOOM_FILTER_STD_ATOMIC : std with atomic operation
 * BLOOM_FILTER_BLO        : blo[1]
 * BLOOM_FILTER_BLO_ATOMIC : blo[1] with atomic operation
 * BLOOM_FILTER_PAT        : pat[1, 1]
 * BLOOM_FILTER_PAT_ATOMIC : pat[1, 1] with atomic operation
 * BLOOM_FILTER_PAT_MUTEX  : pat[1, 1] with mutex
 */
#define BLOOM_FILTER_BLO_ATOMIC

#endif //PA3__FLAGS_H_
