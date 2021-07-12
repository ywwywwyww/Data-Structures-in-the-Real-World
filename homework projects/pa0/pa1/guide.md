# PA 1

这次作业主要针对 `makefile`，现在有四套 `.cc` 和 `.h`，提供了不同函数供 `main.cc` 使用

`flag.mak` 提供了不同开关，控制哪些函数会被调用

## 要求

1. 根据要求编写 `makefile` 文件，生成 `main` 可执行文件
2. 实现 `make clean`，这是良好的习惯
3. 先编译为 `.o` 文件再链接，这样可以减少编译次数
4. 当 `flag.mak` 发生变化时，直接 `make` 生成新的 `main` 而不需要 `clean`

## 提交内容

你只需提交 `makefile`
