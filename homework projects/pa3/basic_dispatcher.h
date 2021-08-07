//
// Created by yww on 2021/8/7.
//

#ifndef PA3__BASIC_DISPATCHER_H_
#define PA3__BASIC_DISPATCHER_H_

#include <vector>

template <typename T>
void Dispatch(int &n, int num_threads, std::vector<T> **data) {
  io::get(n);
  for(int i = 0; i < n; i++) {
    T datum{};
    char op[10];
    io::getstr(op);
    if (op[0] == 'i') {
      datum.type = 1;
    } else {
      datum.type = 2;
    }
    io::getstr(datum.key);
    io::get(datum.time);
    datum.ans = -1;
    XXH64_hash_t hash = XXH3_64bits_withSeed(datum.key, kStrLen, /* Seed */ 95728357235ll);
    data[hash % num_threads]->push_back(datum);
  }
}

#endif //PA3__BASIC_DISPATCHER_H_
