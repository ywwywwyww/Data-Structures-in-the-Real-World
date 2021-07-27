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

void Sort(int *a, int n) {
  static unsigned int buf0[1 << 8], buf1[1 << 8], buf2[1 << 8], buf3[1 << 8];
  unsigned *b = new unsigned[n];
  unsigned *c = new unsigned[n];

  for (int i = 0; i < n; i++) {
    c[i] = a[i] ^ (1 << 31);
    buf0[c[i] % (1 << 8)]++;
    buf1[(c[i] >> 8) % (1 << 8)]++;
    buf2[(c[i] >> 16) % (1 << 8)]++;
    buf3[c[i] >> 24]++;
  }

  for (int i = 1; i < 1 << 8; i++) {
    buf0[i] += buf0[i - 1];
    buf1[i] += buf1[i - 1];
    buf2[i] += buf2[i - 1];
    buf3[i] += buf3[i - 1];
  }

  for (int i = n - 1; i >= 0; i--) {
    b[--buf0[c[i] % (1 << 8)]] = c[i];
  }
  for (int i = n - 1; i >= 0; i--) {
    c[--buf1[(b[i] >> 8) % (1 << 8)]] = b[i];
  }
  for (int i = n - 1; i >= 0; i--) {
    b[--buf2[(c[i] >> 16) % (1 << 8)]] = c[i];
  }
  for (int i = n - 1; i >= 0; i--) {
    a[--buf3[b[i] >> 24]] = b[i] ^ (1 << 31);
  }
  delete[] b;
  delete[] c;
}

int main() {
//  freopen("a.in", "r", stdin);
//  freopen("a.ans", "w", stdout);

  int n, m;
  io::get(n);
  io::get(m);

  int *keys = new int[n];
  int value;
  for (int i = 0; i < n; i++) {
    io::get(keys[i]);
    io::get(value);
  }

  Sort(keys, n);

  n = std::unique(keys, keys + n) - keys;

  tlx::btree_set<int> set;
  set.bulk_load(keys, keys + n);

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
