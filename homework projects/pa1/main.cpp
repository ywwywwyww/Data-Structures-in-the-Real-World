#include <iostream>
#include "btree_map.h"

int main() {
  tlx::btree_map<int, int> map;

  int n, m, p, q;
  scanf("%d%d%d%d", &n, &m, &p, &q);

  int key, val;
  for (int i = 1; i <= n; i++) {
    scanf("%d%d", &key, &val);
    map[key] = val;
  }

  for (int i = 1; i <= m; i++) {
    scanf("%d", &key);
    auto res = map.find(key);
    if (res != map.end()) {
      printf("%d\n", res->second);
    } else {
      printf("NOT FOUND\n");
    }
  }

  for (int i = 1; i <= p; i++) {
    scanf("%d%d", &key, &val);
    map[key] = val;
  }

  int lvalue, rvalue;
  for (int i = 1; i <= q; i++) {
    scanf("%d%d", &lvalue, &rvalue);
    int ans = 0;
    for (auto &&it = map.lower_bound(lvalue); it != map.end() && it->first < rvalue; it++) {
      ans++;
    }
    printf("%d\n", ans);
  }

  return 0;
}
