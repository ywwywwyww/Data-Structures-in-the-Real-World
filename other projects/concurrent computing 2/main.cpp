#include <cstdio>
#include <shared_mutex>
#include <chrono>
#include <thread>
#include <iostream>
#include <atomic>

const int kNumThreads = 32;
const int n = 100000000;
const int gap = 1024;
std::shared_mutex lock[kNumThreads * gap];
std::atomic<int> arr[kNumThreads * gap];
int counter[kNumThreads * gap];


void run(int thread_id) {
	for (int i = 1; i <= n; i++) {
		//lock[thread_id * gap].lock();
		//counter[thread_id * gap]++;
		//lock[thread_id * gap].unlock();
		arr[thread_id * gap]++;
	}
}
int main() {
	auto start = std::chrono::high_resolution_clock::now();

	std::thread *threads[kNumThreads];
	for (int i = 0; i < kNumThreads; i++) {
		threads[i] = new std::thread(&run, i);
	}
	for (int i = 0; i < kNumThreads; i++) {
		threads[i]->join();
	}
	for (int i = 0; i < kNumThreads; i++) {
		std::cout << counter[i] << ' ';
	}
	std::cout << std::endl;

	for (int i = 0; i < kNumThreads; i++) {
		std::cout << arr[i] << ' ';
	}
	std::cout << std::endl;

	auto stop = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
	std::cout << "duration = " << duration.count() << std::endl;
	return 0;
}
