#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
#include <atomic>

int kNumThreads = 1;
std::atomic_int ans = 0;

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

void check(int l, int r, int id) {
  printf("%d: %d %d\n", id, l, r);
  for (int i = l; i <= r; i++) {
    ans += is_prime(i);
  }
  printf("%d\n", id);
}

int main(int argc, char **argv) {
  if (argc > 1) {
    sscanf(argv[1], "%d", &kNumThreads);
  }
  auto start = std::chrono::high_resolution_clock::now();
  int n = 10000000;
  std::thread *threads[kNumThreads];
  for (int i = 0, l = 1, r; i < kNumThreads; i++, l = r + 1) {
    r = l + n / kNumThreads + 1;
    r = std::min(r, n);
    threads[i] = new std::thread(check, l, r, i);
  }
  for (int i = 0; i < kNumThreads; i++) {
    threads[i]->join();
    delete threads[i];
  }
  printf("%d\n", ans.load());
  auto stop = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
  printf("duration = %dus\n", duration.count());
  return 0;
}
