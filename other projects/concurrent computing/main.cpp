#include <iostream>
#include <thread>
#include <mutex>

const int kNumThreads = 1;
int ans = 0;

int is_prime(int x) {
  if (x <= 1) {
    return 0;
  }
  for (int i = 2; i * i <= x; i++) {
    if (x % i == 0) {
      return 0;
    }
  }
  return 1;
}

std::mutex exclusive;

void check(int n, int id) {
  int sum = 0;
  for (int i = id; i <= n; i += kNumThreads) {
    sum += is_prime(i);
  }
  exclusive.lock();
  ans += sum;
  exclusive.unlock();
}

int main() {
  int start = clock();
  int n = 10000000;
  std::thread *threads[kNumThreads];
  for (int i = 0; i < kNumThreads; i++) {
    threads[i] = new std::thread(check, n, i);
  }
  for (int i = 0; i < kNumThreads; i++) {
    threads[i]->join();
    delete threads[i];
  }
  printf("%d\n", ans);
  int end = clock();
  printf("%.5f\n", double(end - start) / CLOCKS_PER_SEC);
  return 0;
}
