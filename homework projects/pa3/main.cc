#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <thread>
#include <vector>
#include <iostream>
#include <iomanip>
#include <fstream>

#include "fast_io.h"
#include "bloom_filter.h"
//#include "basic_dispatcher.h"
#include "advanced_dispatcher.h"

class Operation {
 public:
  int type; // 1: insert, 2: query
  char key[16];
  int time;
  int ans;
};

std::vector<Operation> **data;
int *ans;

void test(int thread_id) {
  Entry entry_copy = entry;
  for (auto &op: *data[thread_id]) {
    if (op.type == 1) {
      Insert(entry_copy, op.key);
      //op.ans = bloom_filter->Query(op.key);
    } else {
      op.ans = Query(entry_copy, op.key);
    }
  }
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
  data = new std::vector<Operation>*[num_threads];
  for (int i = 0; i < num_threads; i++) {
    data[i] = new std::vector<Operation>{};
  }

  int n;
  Dispatch(n, num_threads, data);

  for (int i = 0; i < num_threads; i++) {
    printf("%5d: %d\n", i, data[i]->size());
  }

  ans = new int[n];
  Init();

//  printf("...\n");
//  getchar();
  auto start = std::chrono::high_resolution_clock::now();

  auto **threads = new std::thread*[num_threads];
  for (int i = 0; i < num_threads; i++) {
    threads[i] = new std::thread(test, i);
  }
  for (int i = 0; i < num_threads; i++) {
    threads[i]->join();
  }
//  test(0);

  auto stop = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
  std::cerr << "duration = " << duration.count() << "us" << std::endl;

  double time_used = duration.count() / 1e3;
  std::ofstream f_time("time.out");
  f_time << std::fixed << std::setprecision(3) << time_used << std::endl;
  f_time.close();

  for (int i = 0; i < num_threads; i++) {
    for (auto &&op: *data[i]) {
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
