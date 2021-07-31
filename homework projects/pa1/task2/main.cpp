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

int main() {
//  printf("press any key to continue...");
//  getchar();

//  freopen("a1.in", "r", stdin);
//  freopen("a1.ans", "w", stdout);

  tlx::btree_set<int> set;

  int n;
  io::get(n);

  char op;
  int x, y;
  for (int i = 0; i < n; i++) {
    io::getop(op);
    io::get(x);
    io::get(y);
    if (op == 'i') {
      set.insert(x);
    } else {
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
