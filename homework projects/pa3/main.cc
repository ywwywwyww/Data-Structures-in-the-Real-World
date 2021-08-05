#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <thread>
#include <vector>
#include <iostream>
#include <iomanip>
#include <fstream>

#include "blocked_bloom_filter_base.h"
#include "blocked_bloom_filter_single_thread.h"
#include "blocked_bloom_filter_atomic.h"
#include "blocked_bloom_filter_shared_mutex.h"
#include "bloom_filter.h"
#include "bloom_filter_atomic.h"
#include "fast_io.h"

class Operation {
 public:
  int type; // 1: insert, 2: query
  char key[16];
  int time;
  int ans;
};

std::vector<Operation> *data;
BlockedBloomFilterBase *bloom_filter;
int *ans;

void test(int thread_id) {
  //bloom_filter->Prefetch();
  for (auto &op: data[thread_id]) {
    if (op.type == 1) {
      bloom_filter->Insert(op.key);
      //op.ans = bloom_filter->Query(op.key);
    } else {
      op.ans = bloom_filter->Query(op.key);
    }
  }
}

int main(int argc, char **argv) {
  //      -1: single thread standard bloom filter
  //       0: single thread bit pattern bloom filter using SIMD
  //   1~100: multi-thread bit pattern bloom filter using atomic operation
  // 101~200: multi-thread bit pattern bloom filter using shared mutex and SIMD
  // 201~300: multi-thread standard bloom filter
  int num_threads;

  if (argc == 2) {
    num_threads = atoi(argv[1]);
  } else {
    fprintf(stderr, "Please input the number of threads\n");
    return 0;
  }
  fprintf(stderr, "num_threads: %d\n", num_threads);

  if (num_threads == -1) {
    bloom_filter = new BloomFilter();
    num_threads = 1;
  } else if (num_threads == 0) {
    bloom_filter = new BlockedBloomFilterSingleThread();
    num_threads = 1;
  } else if (num_threads <= 100) {
    bloom_filter = new BlockedBloomFilterAtomic();
  } else if (num_threads <= 200) {
    bloom_filter = new BlockedBloomFilterSharedMutex();
    num_threads -= 100;
  } else {
    bloom_filter = new BloomFilterAtomic();
    num_threads -= 200;
  }

  int n = 0;
  data = new std::vector<Operation>[num_threads];
  for (int i = 0; i < num_threads; i++) {
    int ni;
    char op[10];
    char filename[20];
    sprintf(filename, "data%d.in", i + 1);
    io::open_input_file(filename);
    io::get(ni);
    n += ni;
    for(int j = 0; j < ni; j++) {
      data[i].emplace_back();
      io::getstr(op);
      if (op[0] == 'i') {
        data[i].back().type = 1;
      } else {
        data[i].back().type = 2;
      }
      io::getstr(data[i].back().key);
      io::get(data[i].back().time);
      data[i].back().ans = -1;
    }
  }
  ans = new int[n];

//  printf("...\n");
//  getchar();
  auto start = std::chrono::high_resolution_clock::now();

//  auto **threads = new std::thread*[num_threads];
//  for (int i = 0; i < num_threads; i++) {
//    threads[i] = new std::thread(test, i);
//  }
//  for (int i = 0; i < num_threads; i++) {
//    threads[i]->join();
//  }
  test(0);

  auto stop = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
  std::cerr << "duration = " << duration.count() << "us" << std::endl;

  double time_used = duration.count() / 1e3;
  std::ofstream f_time("time.out");
  f_time << std::fixed << std::setprecision(3) << time_used << std::endl;
  f_time.close();

  for (int i = 0; i < num_threads; i++) {
    for (auto &&op: data[i]) {
      ans[op.time - 1] = op.ans;
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
