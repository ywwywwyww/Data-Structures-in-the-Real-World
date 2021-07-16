//
// Created by yww on 2021/7/16.
//

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include "../binary search trees/b_tree.h"

int a[10000010];

int main() {
  const int m = 7;
  std::cout << sizeof(BTreeNode<int, m>) << std::endl;
  srand(0);
  int n = 10000000;
  for (int i = 1; i <= n; i++) {
    a[i] = rand();
  }
  BTree<int, m> bt_set;
  for (int i = 1; i <= n; i++) {
    bt_set.Insert(a[i]);
  }

  std::cout << "press enter key to continue..";
  std::cin.get();

  int sum = 0;
  for (int i = 1; i <= n; i++) {
    if (bt_set.Find(a[i])) {
      sum ^= a[i];
    }
  }
  std::cout << "sum = " << sum << std::endl;
  return 0;
}
