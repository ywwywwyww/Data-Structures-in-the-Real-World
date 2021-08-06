//
// Created by yww on 2021/8/6.
//

#ifndef PA3__BLOOM_FILTER_H_
#define PA3__BLOOM_FILTER_H_

#include "flags.h"
#include "bloom_filter_base.h"

#ifdef BLOOM_FILTER_STD
#include "bloom_filter_std.h"
#endif

#ifdef BLOOM_FILTER_STD_ATOMIC
#include "bloom_filter_std_atomic.h"
#endif

#ifdef BLOOM_FILTER_BLO
#include "bloom_filter_blo.h"
#endif

#ifdef BLOOM_FILTER_BLO_ATOMIC
#include "bloom_filter_blo_atomic.h"
#endif

#ifdef BLOOM_FILTER_PAT
#include "bloom_filter_pat.h"
#endif

#ifdef BLOOM_FILTER_PAT_ATOMIC
#include "bloom_filter_pat_atomic.h"
#endif

#ifdef BLOOM_FILTER_PAT_MUTEX
#include "bloom_filter_pat_mutex.h"
#endif

#endif //PA3__BLOOM_FILTER_H_
