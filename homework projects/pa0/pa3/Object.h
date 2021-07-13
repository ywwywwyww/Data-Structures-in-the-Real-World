#pragma once
#include <iostream>
#include <list>
#include <memory>

#include "CustomClass.h"

class Object;

class Content {
 public:
  virtual void output(std::ostream &out) { // virtual function for output
    std::cout << "the operator is not supported for this type." << std::endl;
  }
};

class IntContent : public Content {
 private:
  int x;

 public:
  IntContent(int _x) : x(_x) {}
  void output(std::ostream &out) {
    out << x;
  }

  IntContent &operator+=(int y) {
    this->x += y;
    return *this;
  }
};
class StringContent : public Content {
 private:
  std::string x;

 public:
  StringContent(std::string _x) : x(_x) {}
  void output(std::ostream &out) {
    out << x;
  }

  StringContent &operator+=(std::string y) {
    this->x += y;
    return *this;
  }
};

class VectorContent : public Content {
 private:
  std::vector<Object> x;
 public:
  VectorContent(const std::vector<Object> &_x) : x(_x) {}
  Object &operator[](int pos) {
    return x[pos];
  }
};

class CustomContent : public Content {
 private:
  CustomClass x;

 public:
  CustomContent(const CustomClass &_x) : x(_x) {}
};

class Object {
 private:
  std::shared_ptr<Content> pt;

 public:
  Object() {}
  Object(int x) {
    pt = std::make_shared<IntContent>(x);
  }
  Object(const std::string &x) {
    pt = std::make_shared<StringContent>(x);
  }
  Object(const std::vector<Object> &x) {
    pt = std::make_shared<VectorContent>(x);
  }
  Object(const CustomClass &x) {
    pt = std::make_shared<CustomContent>(x);
  }

  friend std::ostream &operator<<(std::ostream &out, const Object &obj) {
    obj.pt->output(out);
    return out;
  }

  Object &operator+=(int y) {
    (dynamic_cast<IntContent *>(pt.get()))->operator+=(y);
    return *this;
  }
  Object &operator+=(const std::string &y) {
    (dynamic_cast<StringContent *>(pt.get()))->operator+=(y);
    return *this;
  }

  Object &operator[](const int &pos) {
    return (*(dynamic_cast<VectorContent *>(pt.get())))[pos];
  }
  // need more operators......
};
