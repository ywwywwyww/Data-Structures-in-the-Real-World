/**
 * @file Vector.cc
 * @author Wuwei YUAN
 * @date 7/12/21
 */

#include "Vector.h"

Vector::Vector(int n) {
  this->capacity = n;
  this->len = 0;
  this->array = new Node[this->capacity];
}

Vector::Vector(const Vector &other) {
  this->capacity = other.len;
  this->len = other.len;
  this->array = new Node[this->capacity]; // TODO: Maybe there is something to be improve here
  for (int i = 0; i < this->len; i++) {
    this->array[i] = other.array[i];
  }
}

Vector::Vector(Vector &&other) {
  this->capacity = other.capacity;
  this->len = other.len;
  this->array = other.array;

  other.capacity = 0;
  other.len = 0;
  other.array = nullptr;
}

Vector &Vector::operator=(const Vector &other) {
  if (this->capacity >= other.len) {
   this->len = other.len;
   for (int i = 0; i < this->len; i++) {
     this->array[i] = other.array[i];
   }
  } else {
    delete[] this->array;
    this->capacity = other.len;
    this->len = other.len;
    this->array = new Node[this->capacity]; // TODO: Maybe there is something to be improve here
    for (int i = 0; i < this->len; i++) {
      this->array[i] = other.array[i];
    }
  }

  return *this;
}

Vector & Vector::operator=(Vector &&other) {
  delete[] this->array;

  this->capacity = other.capacity;
  this->len = other.len;
  this->array = other.array;

  other.capacity = 0;
  other.len = 0;
  other.array = nullptr;

  return *this;
}

Vector::~Vector() {
  delete[] this->array;
}

Node & Vector::operator[](int pos) {
  return this->array[pos];
}

void Vector::append(int value) {
  if (this->len == this->capacity) {
    this->dilatation();
  }
  this->array[this->len] = std::move(Node(value));
  this->len++;
}

void Vector::insert(int pos, int value) {
  if (this->len == this->capacity) {
    this->dilatation();
  }
  for (int i = this->len - 1; i >= pos; i--) {
    this->array[i + 1] = std::move(this->array[i]);
  }
  this->array[pos] = std::move(Node(value));
  this->len++;
}

void Vector::dilatation() {
  Node *new_array = new Node[2 * this->capacity];
  for (int i = 0; i < this->len; i++) {
    new_array[i] = std::move(this->array[i]);
  }
  delete[] this->array;
  this->array = new_array;
  this->capacity *= 2;
}

void Vector::swap(int pos1, int pos2) {
  Node temp = std::move(this->array[pos1]);
  this->array[pos1] = std::move(this->array[pos2]);
  this->array[pos2] = std::move(temp);
}

int Vector::getlen() {
  return this->len;
}
