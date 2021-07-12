# PA 4

小明的同学觉得小明卷歪了，跟他说 Python 中的数组也非常好用，小明表示不服，于是也想在C++里实现一个类似功能的类。

小明将这个类命名为 `MyList`，具体来说，需要满足以下的功能：

- 通过 `append` 可以向 `MyList` 的最后添加元素
- `x[i]` 可以访问 `MyList` 的第 `i` 个元素
- 可以用 `cout` 输出 `MyList` 中的所有元素

```c++
// subtask 1
MyList x;
std::cout << x << std::endl; // []
for(int i = 0; i < 5; i++)
    x.append(i);
std::cout << x[0] << std::endl; // 0
std::cout << x << std::endl; // [0, 1, 2, 3, 4]
```

Python 中的对象有一个特性，即所有的对象均为引用。下列代码中，对于 y的修改，也会导致x的修改。

```c++
// subtask 2
MyList y = x;
y[0] += 5;
std::cout << x << std::endl; // [5, 1, 2, 3, 4]
```

接下来，小明还想实现 `slice` 功能。拿出 `MyList` 的任意一段，也可以当做一个新的 `MyList`，较小的那一段被称为原 `MyList` 的一个 `slice`。并且对 `slice` 的修改也会影响原始的 `MyList`。

```c++
// subtask 3
MyList z = x(1, 3); // 左闭右开选出位置在 [1, 3) 的元素，保证 slice 左端点不大于右端点
std::cout << z << std::endl; // [1, 2]
z[1] += 5;
std::cout << z << std::endl; // [1, 7]
std::cout << x << std::endl; // [5, 1, 7, 3, 4]
```

`Slice` 还有一个神奇的特点。如果向 `slice` 中 `append`，不仅 `slice` 的最后会多出一个新增的元素，原始的 `List` 也会在相应位置加入该元素。

```c++
// subtask 4
MyList r = x(2, 4);
std::cout << r << std::endl; // [7, 3]
r.append(-1);
std::cout << r << std::endl; // [7, 3, -1]
std::cout << x << std::endl; // [5, 1, 7, 3, -1, 4]
```

为了简便，该题的 `list` 只存储 `int`，感兴趣的同学可以用模板实现更多数据类型的存储，评测时不会涉及，并且可以默认在使用中不出现非法的访问操作。

很可惜的是，小明忙于和同学线下 battle，并没有现成的代码给你用，你需要自己实现全部内容。

## 样例输入与输出

请考虑到尽量多的 corner cases，本题不提供样例输入与输出。

## 提交内容

你需要提交 `MyList.h` 和 `MyList.cc`，用不到的文件可以置为空

评测时将和 `main.cc` 一起编译