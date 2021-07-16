#include <iostream>
#include <set>
#include "b_tree_with_ptr.h"

int main() {
  int seed = 0;
  for (;; seed++) {
//    if (seed % 1000 == 0) {
//      printf("%d\n", seed);
//    }
    srand(seed);
    printf("seed: %d\n", seed);
    BTree<int, 11> bt_set;
    std::set<int> rbt_set;
    int m = 1000000;
//    printf("%d\n", m);
    for (int i = 1; i <= m; i++) {
      int op = rand() % 3;
      int v = rand() % 10000 + 1;
//      printf("%d %d %d\n", i, op, v);
      int s1, s2;
      if (op == 0) {
        s1 = bt_set.Insert(v);
        s2 = rbt_set.insert(v).second;
      } else if (op == 1) {
        s1 = bt_set.Find(v);
        s2 = rbt_set.count(v);
      } else {// op == 2
        s1 = bt_set.Erase(v);
        s2 = rbt_set.erase(v);
      }
      if (s1 != s2) {
        printf("wrong: %d\n", i);
        exit(0);
      }
    }
  }
  return 0;
}
