cmake_minimum_required(VERSION 3.19)
project(pa3)

set(CMAKE_CXX_STANDARD 17)

add_executable(pa3 main.cc bloom_filter_base.h bloom_filter_pat.h bloom_filter_pat_mutex.h bloom_filter_pat_atomic.h xxHash/xxhash.h fast_io.h bloom_filter_std.h bloom_filter_std_atomic.h flags.h bloom_filter.h bloom_filter_blo.h bloom_filter_blo_atomic.h)