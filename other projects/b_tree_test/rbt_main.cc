//
// Created by yww on 2021/7/16.
//

#include <cstdio>
#include <set>
#include <cstdlib>
#include <iostream>

int a[10000010];

int main() {
  srand(0);
  int n = 10000000;
  for (int i = 1; i <= n; i++) {
    a[i] = rand();
  }
  std::set<int> rbt_set;
  for (int i = 1; i <= n; i++) {
    rbt_set.insert(a[i]);
  }

  std::cout << "press enter key to continue...";
  std::cin.get();

  int sum = 0;
  for (int i = 1; i <= n; i++) {
    if (rbt_set.count(a[i])) {
      sum ^= a[i];
    }
  }
  std::cout << "sum = " << sum << std::endl;
  return 0;
}
