//
// Created by yww on 2021/8/7.
//

#ifndef PA3__ADVANCED_DISPATCHER_H_
#define PA3__ADVANCED_DISPATCHER_H_

#include <vector>
#include <queue>
#include <algorithm>


template <typename T>
void Dispatch(int &n, int num_threads, std::vector<T> **data) {

  const int kNumBuckets = 1 << 20;
  int map[kNumBuckets];
  memset(map, -1, sizeof(map));

  //int *kinds = new int[num_threads];

  //typedef std::pair<double, int> pdi;
  //std::priority_queue<pdi, std::vector<pdi>, std::greater<>> q;
  //for (int i = 0; i < num_threads; i++) {
  //  q.push(std::make_pair(0, i));
  //}

  io::get(n);
  for(int i = 0; i < n; i++) {
    T datum{};
    char op[10];
    io::getstr(op);
    if (op[0] == 'i') {
      datum.type = -1;
    } else {
      datum.type = 1;
    }
    io::getstr(datum.key);
    io::get(datum.time);
    XXH64_hash_t hash = XXH3_64bits_withSeed(datum.key, kStrLen, /* Seed */ 95728357235ll);
    int bucket = hash % kNumBuckets;
    if (map[bucket] == -1) {
//      double freq = sqrt((double)n / (i + 1));
//      pdi top = q.top();
//      q.pop();
//      map[bucket] = top.second;
//      top.first += freq;
//      q.push(top);
      typedef std::pair<int, int> pii;
      pii s(0x3fffffff, 0);
      for (int j = 0; j < num_threads; j++) {
        s = min(s, pii(data[j]->size(), j));
      }
      map[bucket] = s.second;
      //kinds[s.second]++;
    }
    data[map[bucket]]->push_back(datum);
  }
}

#endif //PA3__ADVANCED_DISPATCHER_H_
