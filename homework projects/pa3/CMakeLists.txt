cmake_minimum_required(VERSION 3.19)
project(pa3)

set(CMAKE_CXX_STANDARD 17)

add_executable(pa3 main.cc blocked_bloom_filter_base.h blocked_bloom_filter_single_thread.h blocked_bloom_filter_shared_mutex.h blocked_bloom_filter_atomic.h xxHash/xxhash.h fast_io.h bloom_filter.h bloom_filter_atomic.h)