# PA 3

小明和同学最近在朋友圈看到了 Python 入门课程，他们发现 Python 中变量的类型是动态的，因此可以为同一个变量赋予不同类型的值。小明的同学报名了 Python 课程，但小明不一样，他想在 C++ 中也做到类似的事情，于是它设计了一个类 `Object`，任意的变量都可以储存进该类中。具体来说：

```c++
// subtask 1
Object x;
x = 1;
std::cout << x << std::endl; //1
x = std::string("text");
std::cout << x << std::endl; // text
x = CustomClass("obj1"); // a user-defined class
```

`Object` 一般都是以引用的形式储存数据，这种方式可以避免多余的复制操作。这也使得有时候对某一 `Object` 的修改导致另一 `Object` 的变化，举例来说：

```c++
// subtask 2
Object x, y, z;
x = 1;
y = x; // y point to the same value of x
z = CustomClass("obj2");
std::cout << x << " " << y << std::endl; // 1 1

x += 1;
std::cout << x << " " << y << std::endl; // 2 2

x = std::string("text"); // x points to "text", but y still points to 2
std::cout << x << " " << y << std::endl; // text 2

y = x;
x += std::string("1");
std::cout << x << " " << y << std::endl; // text1 text1
x = z;
```

注意这里 `=` 和 `+=` 的不同，`=` 是让 `Object` 改为储存另一个数据，`+=` 是修改储存的数据。

接下来，小明考虑让 `std::vector` 能够装进不同类型的对象。

```c++
// subtask 3
std::vector<Object> arr;
arr.push_back(Object(1));
arr.push_back(Object("text"));
arr.push_back(Object(CustomClass("obj3")));
std::cout << arr[0] << " " << arr[1] << std::endl; // 1 text
arr[0] = arr[1];
std::cout << arr[0] << " " << arr[1] << std::endl; // text text
arr[1] += std::string("1");
std::cout << arr[0] << " " << arr[1] << std::endl; // text1 text1
arr[0] = arr[2];
std::cout << arr[1] << std::endl; // text1
```

如果将 `std::vector` 赋给 `Object`，甚至能够做到在 `std::vector` 中包含 `std::vector`。对于以下代码，如果存在理解困难，下载文件中提供了一个 `instruction.pdf` 解释了操作的过程。

```c++
// subtask 4
std::vector<Object> inner_arr1 = {Object(1), Object("text1"), CustomClass("obj4")};
std::vector<Object> inner_arr2 = {Object(2), Object("text2")};
std::vector<Object> arr = {Object(inner_arr1), Object(inner_arr2), CustomClass("obj5")};

std::cout << arr[0][0] << " " << arr[0][1] << std::endl; // 1 text1
std::cout << arr[1][0] << " " << arr[1][1] << std::endl; // 2 text2

arr[1] = arr[0];
std::cout << arr[1][0] << " " << arr[1][1] << std::endl; // 1 text1

arr[0][0] += 2;
std::cout << arr[0][0] << " " << arr[1][0] << std::endl; // 3 3

arr[0][1] = arr[1][0]; // arr[1][1] and arr[0][1] is the same Object
std::cout << arr[0][1] << " " << arr[1][1] << std::endl; // 3 3

arr[0][0] = arr[2];
```

- 为了简单考虑，我们对题目做出以下限制：
  - 只要求 `Object` 能够储存以下类型 `int`，`std::string`，`std::vector<Object>` 和 `CustomClass`。其中 `CustomClass` 是一个用户自定义类，该类会统计其对象构造与析构的次数，并测试你的代码是否存在内存泄露。
  - `+=` 运算右侧只会出现 `int` 和 `std::string`，并且保证类型与 `Object` 中存储的类型一致。
  - 中括号运算 `[]` 只会对储存了 `std::vector<Object>` 的 `Object` 使用，并且保证不会越界访问。

你可以从头实现。如果觉得太困难，也可以参考小明实现的代码（`Object.h`），但有一些空缺和一些 bug。

## 样例输入与输出

输出

```
1
text
1 1
2 2
text 2
text1 text1
1 text
text text
text1 text1
text1
1 text1
2 text2
1 text1
3 3
3 3
```

跑通 `main.cpp` 即可获得全部分数

## 提交内容

你只需提交 `Object.h`