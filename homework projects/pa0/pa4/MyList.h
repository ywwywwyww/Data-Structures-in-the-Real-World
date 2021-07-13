/**
 * @file my_list.h
 * @author Wuwei YUAN
 * @date 7/13/21
 */

#ifndef PA4__MYLIST_H_
#define PA4__MYLIST_H_

#include <iostream>
#include <list>
#include <memory>

/**
 * 考虑用 begin_ 和 end_ 存下当前的 list 对应的 begin 和 end
 */
template<typename T = int>
class MyList {
 public:
  MyList() {
    this->list_ptr_ = std::make_shared<std::list<T>>();
    this->begin_ = this->list_ptr_->begin();
    this->end_ = this->list_ptr_->end();
  }

  MyList(const MyList<T> &other) {
    this->list_ptr_ = other.list_ptr_;
    this->begin_ = other.begin_;
    this->end_ = other.end_;
  }

  T &operator[](const int &pos) {
    return *this->GetIterator(pos);
  }

  MyList<T> operator()(const int &l, const int &r) {
    MyList<T> new_list;
    new_list.list_ptr_ = this->list_ptr_;
    new_list.begin_ = this->GetIterator(l);
    new_list.end_ = this->GetIterator(r);
    return new_list;
  }

  friend std::ostream &operator<<(std::ostream &out, const MyList<T> &list) {
    out << '[';
    if (list.begin_ != list.end_) {
      int cnt = 0;
      for (auto it = list.begin_; it != list.end_; it++) {
        if (cnt) {
          out << ", ";
        }
        out << *it;
        cnt++;
      }
    }
    out << ']';
    return out;
  }

  void append(const T &val) {
    if (this->begin_ == this->end_) {
      this->begin_ = this->list_ptr_->insert(this->end_, val);
    } else {
      this->list_ptr_->insert(this->end_, val);
    }
  }

  typename std::list<T>::iterator GetIterator(const int &pos) {
    typename std::list<T>::iterator cur = this->begin_;
    for (int i = 0; i < pos; i++) {
      cur++;
    }
    return cur;
  }

 private:
  typename std::list<T>::iterator begin_, end_;
  std::shared_ptr<std::list<T>> list_ptr_; // Saves a copy of the whole list to avoid the smart pointer being destroyed.
};

#endif //PA4__MYLIST_H_
