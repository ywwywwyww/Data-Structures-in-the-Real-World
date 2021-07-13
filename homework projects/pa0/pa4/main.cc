/**
 * @file main.cc
 * @author Wuwei YUAN
 * @date 7/13/21
 */

#include <iostream>
#include "MyList.h"

int main() {
  // subtask 1
  MyList x;
  std::cout << x << std::endl; // []
  for(int i = 0; i < 5; i++)
    x.append(i);
  std::cout << x[0] << std::endl; // 0
  std::cout << x << std::endl; // [0, 1, 2, 3, 4]

  // subtask 2
  MyList y = x;
  y[0] += 5;
  std::cout << x << std::endl; // [5, 1, 2, 3, 4]

  // subtask 3
  MyList z = x(1, 3); // 左闭右开选出位置在 [1, 3) 的元素，保证 slice 左端点不大于右端点
  std::cout << z << std::endl; // [1, 2]
  z[1] += 5;
  std::cout << z << std::endl; // [1, 7]
  std::cout << x << std::endl; // [5, 1, 7, 3, 4]

  // subtask 4
  MyList r = x(2, 4);
  std::cout << r << std::endl; // [7, 3]
  r.append(-1);
  std::cout << r << std::endl; // [7, 3, -1]
  std::cout << x << std::endl; // [5, 1, 7, 3, -1, 4]
}