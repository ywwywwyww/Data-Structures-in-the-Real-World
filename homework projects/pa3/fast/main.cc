#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <thread>
#include <vector>
#include <iostream>
#include <iomanip>
#include <fstream>
//#include <tbb.h>


#include <atomic>
#include <cmath>

#define XXH_INLINE_ALL
#include "xxHash/xxhash.c"

#include "fast_io.h"


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

static const int kTableLen = kNumSlots / 8;
std::atomic<uint8_t> table[kTableLen] __attribute__((aligned(64)));

#include "advanced_dispatcher.h"

class Operation {
 public:
  int type; // -1: insert, 1: query, also stores the answer
  char key[16];
  int time;
};


void test(const int thread_id, const int &num_ops, Operation *ops) {
  auto start = std::chrono::high_resolution_clock::now();
  XXH64_hash_t next_hash = XXH3_64bits_withSeed(ops[0].key, kStrLen, /* Seed */ 28375293592153ll);
  //for (int t = 10; t; t--)
  for (int i = 0; i < num_ops; i++) {
    //__builtin_prefetch(&ops[i+1]);
    XXH64_hash_t hash = next_hash;
    int block_id = hash % kNumBlocks;
    XXH64_hash_t hash1 = (hash / kNumBlocks) % kBlockSize;
    XXH64_hash_t hash2 = (hash / kNumBlocks / kBlockSize) % kBlockSize;
    __builtin_prefetch(&table[block_id * kBlockSize / 8]);
    next_hash = XXH3_64bits_withSeed(ops[i + 1].key, kStrLen, /* Seed */ 28375293592153ll);

    if (ops[i].type == -1) {
      for (int j = 0; j < kNumHashFunctions; j++) {
        int pos = block_id * kBlockSize + (hash1 + j * hash2) % kBlockSize;
        table[pos / 8] |= 1 << (pos % 8);
	//table[thread_id * 100] |= 1 << (pos % 8);
      }
    } else {
      ops[i].type = 1;
      for (int j = 0; j < kNumHashFunctions; j++) {
        int pos = block_id * kBlockSize + (hash1 + j * hash2) % kBlockSize;
        if (!(table[pos / 8] & (1 << (pos % 8)))) {
        //if (!((table[thread_id * 100] >> (pos % 8)) & 1)) {
          ops[i].type = 0;
          break;
        }
      }
    }
  }
  auto stop = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
  std::cerr << thread_id << "(on CPU " << sched_getcpu() << "): duration = " << duration.count() << "us" << std::endl;
}

int main(int argc, char **argv) {
  int num_threads;

  if (argc == 2) {
    num_threads = atoi(argv[1]);
  } else {
    fprintf(stderr, "Please input the number of threads\n");
    return 0;
  }
  fprintf(stderr, "num_threads: %d\n", num_threads);


  io::open_input_file("data.in");
  std::vector<Operation> **data = new std::vector<Operation>*[num_threads];
  for (int i = 0; i < num_threads; i++) {
    data[i] = new std::vector<Operation>{};
  }

  int n;
  Dispatch(n, num_threads, data);

  for (int i = 0; i < num_threads; i++) {
    printf("%5d: %d\n", i, data[i]->size());
  }

  int *ans = new int[n];

  int *ni = new int[num_threads];
  Operation **ops = new Operation*[num_threads];
  for (int i = 0; i < num_threads; i++) {
	  ni[i] = data[i]->size();
	  ops[i] = new Operation[ni[i] + 1];
	  for (int j = 0; j < ni[i]; j++) {
		  ops[i][j] = (*data[i])[j];
	  }
  }


//  printf("...\n");
//  getchar();
  auto start = std::chrono::high_resolution_clock::now();

  auto **threads = new std::thread*[num_threads];
  for (int i = 0; i < num_threads; i++) {
    threads[i] = new std::thread(test, i, ni[i], ops[i]);


    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    // 32 threads
    //CPU_SET(i, &cpuset);
    //CPU_SET(i / 2 + (i % 2) * 32, &cpuset);
    //CPU_SET(64 / num_threads * i, &cpuset); // fastest
    //CPU_SET(i / 2, &cpuset);
    //
    // 2 threads
    CPU_SET(i * 2, &cpuset); // same CCX
    //CPU_SET(i * 4, &cpuset); // same CCD
    //CPU_SET(64 / num_threads * i, &cpuset); // different CCD
    int rc = pthread_setaffinity_np(threads[i]->native_handle(),
		    sizeof(cpu_set_t), &cpuset);
  }
  for (int i = 0; i < num_threads; i++) {
    threads[i]->join();
  }
  
  //tbb::parallel_for(0, num_threads, 1,
		  //[&](int thread_id) {
		  //test(thread_id, ni[thread_id], ops[thread_id]);
				  //});


//  test(0);

  auto stop = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
  std::cerr << "duration = " << duration.count() << "us" << std::endl;

  double time_used = duration.count() / 1e3;
  std::ofstream f_time("time.out");
  f_time << std::fixed << std::setprecision(3) << time_used << std::endl;
  f_time.close();

  for (int i = 0; i < num_threads; i++) {
    for (int j = 0; j < ni[i]; j++) {
      ans[ops[i][j].time - 1] = ops[i][j].type;
    }
  }
  FILE *f_out = fopen("result.out", "w");
  for (int i = 0; i < n; i++) {
    if (ans[i] >= 0) {
      fprintf(f_out, "%d\n", ans[i]);
    }
  }

  return 0;
}
