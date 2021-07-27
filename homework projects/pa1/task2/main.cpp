#include <iostream>
#include <algorithm>
#include "btree_set.h"

namespace io {
const int SIZE = (1 << 20) + 1;
char ibuf[SIZE], *iS, *iT;
char obuf[SIZE], *oS = obuf, *oT = oS + SIZE - 1;
int getc() {
  (iS == iT ? iS = ibuf, iT = ibuf + fread(ibuf, 1, SIZE, stdin) : 0);
  return iS == iT ? EOF : *(iS++);
}
int f;
char c;
template<class T>
void get(T &x) {
  f = 1;
  for (c = getc(); (c < '0' || c > '9') && c != '-'; c = getc());
  (c == '-' ? f = -1, c = getc() : 0);
  x = 0;
  for (; c >= '0' && c <= '9'; c = getc())
    x = x * 10 + c - '0';
  x *= f;
}
void flush() {
  fwrite(obuf, 1, oS - obuf, stdout);
  oS = obuf;
}
void putc(char x) {
  *(oS++) = x;
  if (oS == oT)
    flush();
}
int a[55], t;
template<class T>
void put(T x) {
  if (!x)
    putc('0');
  x < 0 ? putc('-'), x = -x : 0;
  while (x) {
    a[++t] = x % 10;
    x /= 10;
  }
  while (t)
    putc(a[t--] + '0');
}
void space() {
  putc(' ');
}
void enter() {
  putc('\n');
}
struct flusher {
  ~flusher() {
    flush();
  }
}
    io_flusher;
}

int main() {
//  freopen("a.in", "r", stdin);
//  freopen("a.ans", "w", stdout);

  tlx::btree_set<int> set;

  int n, m;
  io::get(n);
  io::get(m);

  int key, value;
  for (int i = 0; i < n; i++) {
    io::get(key);
    io::get(value);
    set.insert(key);
  }

  int lvalue, rvalue;
  for (int i = 0; i < m; i++) {
    io::get(lvalue);
    io::get(rvalue);
    int ans = set.count_prefix(rvalue - 1);
    if (lvalue - 1 < lvalue) {
      ans -= set.count_prefix(lvalue - 1);
    }
    printf("%d\n", ans);
  }
  return 0;
}
