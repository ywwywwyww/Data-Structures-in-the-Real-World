//
// Created by yww on 2021/8/3.
//

#ifndef PA3__FAST_IO_H_
#define PA3__FAST_IO_H_

#include <cstdio>
#include <cctype>

namespace io {
const int SIZE = (1 << 20) + 1;
char ibuf[SIZE], *iS, *iT;
char obuf[SIZE], *oS = obuf, *oT = oS + SIZE - 1;
FILE *input_file = stdin;
int getc() {
  (iS == iT ? iS = ibuf, iT = ibuf + fread(ibuf, 1, SIZE, input_file) : 0);
  return iS == iT ? EOF : *(iS++);
}
int f;
char c;
void getop(char &ch) {
  for(ch = getc(); ch != 'i' && ch != 'r'; ch = getc());
}
void getstr(char *str) {
  char ch;
  for (ch = getc(); !isalpha(ch) && !isdigit(ch); ch = getc());
  *(str++) = ch;
  for (ch = getc(); isalpha(ch) || isdigit(ch); ch = getc()) {
    *(str++) = ch;
  }
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
void open_input_file(char *file_name) {
  input_file = fopen(file_name, "r");
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


#endif //PA3__FAST_IO_H_
