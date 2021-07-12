#pragma once
#include <iostream>
#include <list>

#include "CustomClass.h"

class Content {
  virtual void output(std::ostream &out) { // virtual function for output
    std::cout << "the operator is not supported for this type." << std::endl;
  }
};

class IntContent : public Content {
private:
  int x;

public:
  IntContent(int _x) : x(_x) {}
  void output(const std::ostream &out) { //???
    out << x;
  }
};
class StringContent : public Content {
private:
  std::string x;

public:
  StringContent(std::string _x) : x(_x) {}
  void output(const std::ostream &out) { //???
    out << x;
  }
};
// The following codes are not working
// class VectorContent: public Content
// {
// private:
// 	std::vector<Object> x; //Object is not defined???
// public:
// 	VectorContent(const std::vector<Object>& _x): x(_x) {}
// };
class CustomContent : public Content {
private:
  CustomClass x;

public:
  CustomContent(const CustomClass &_x) : x(_x) {}
};

class Object {
private:
  Content *pt;

public:
  Object() {}
  Object(int x) {
    pt = new int(x); // where to delete? maybe use std::shared_ptr?
  }
  Object(const std::string &x) { pt = new StringContent(x); }
  // The following codes are not working
  // Object(const std::vector<Object> &x){
  // 	pt = new VectorContent(x);
  // }
  Object(const CustomClass &x) { pt = new CustomContent(x); }

  friend std::ostream &operator<<(std::ostream &out, const Object &obj) {
    obj.pt->output(out);
    return out;
  }

  Object &operator+=(int y) {
    pt->operator+=(y);
    return *this;
  }
  Object &operator+=(const std::string &y) {
    pt->operator+=(y);
    return *this;
  }

  // need more operators......
};
