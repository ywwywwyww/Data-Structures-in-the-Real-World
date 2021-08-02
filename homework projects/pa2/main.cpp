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
void getop(char &ch) {
  for(ch = getc(); ch != 'i' && ch != 'r'; ch = getc());
}
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
//  printf("press any key to continue...");
//  getchar();

//  freopen("1.in", "r", stdin);
//  freopen("1.ans", "w", stdout);

  tlx::btree_set<int> set;

  int n;
  io::get(n);

  char op;
  int x, y;

  int *keys = new int[n];
  int cnt = 0;
  int flag = 1; // 之前是否没有询问过

  for (int i = 0; i < n; i++) {
    io::getop(op);
    io::get(x);
    io::get(y);
    if (op == 'i') {
      if (flag) {
        keys[cnt++] = x;
      } else {
        set.insert(x);
      }
    } else {
      if (flag) {
        flag = 0;
        Sort(keys, cnt);
        cnt = std::unique(keys, keys + cnt) - keys;
        set.bulk_load(keys, keys + cnt);
      }
      int ans = set.count_prefix(y - 1);
      if (x - 1 < x) {
        ans -= set.count_prefix(x - 1);
      }
      io::put(ans);
      io::enter();
    }
  }

  return 0;
}
