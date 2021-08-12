#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <thread>
#include <vector>
#include <iostream>
#include <iomanip>
#include <fstream>
//#include <tbb.h>

#include "fast_io.h"
#include "bloom_filter.h"
//#include "basic_dispatcher.h"
#include "advanced_dispatcher.h"
//#include "more_advanced_dispatcher.h"

class Operation {
 public:
  int type; // -1: insert, 1: query, also stores the answer
  char key[16];
  int time;
};


void test(const int thread_id, const int &num_ops, Operation *ops) {
  auto start = std::chrono::high_resolution_clock::now();
  //for (int t = 10; t; t--)
  for (int i = 0; i < num_ops; i++) {
    Operation &op = ops[i];
    if (op.type == -1) {
      Insert(op.key);
    } else {
      op.type = Query(op.key);
    }
  }
  auto stop = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
  //std::cerr << thread_id << "(on CPU " << sched_getcpu() << "): duration = " << duration.count() << "us" << std::endl;
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

  int max_len = 0;
  //for (int i = 0; i < num_threads; i++) {
  //printf("%5d: %d\n", i, data[i]->size());
  //max_len = std::max(max_len, (int)data[i]->size());
  //
  //intf("max len: %d\n", max_len);

  int *ans = new int[n];

  int *ni = new int[num_threads];
  Operation **ops = new Operation*[num_threads];
  for (int i = 0; i < num_threads; i++) {
	  ni[i] = data[i]->size();
	  ops[i] = new Operation[ni[i]];
	  for (int j = 0; j < ni[i]; j++) {
		  ops[i][j] = (*data[i])[j];
	  }
  }

  Init();

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
