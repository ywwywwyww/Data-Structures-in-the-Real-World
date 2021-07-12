#include <iostream>

#ifdef __F1__
#include "f1.h"
#endif

#ifdef __F2__
#include "f2.h"
#endif

#ifdef __F3__
#include "f3.h"
#endif

#ifdef __F4__
#include "f4.h"
#endif

int main() {
  std::cout << "main" << std::endl;

#ifdef __F1__
  f1();
#endif

#ifdef __F2__
  f2();
#endif

#ifdef __F3__
  f3();
#endif

#ifdef __F4__
  f4();
#endif

  return 0;
}
