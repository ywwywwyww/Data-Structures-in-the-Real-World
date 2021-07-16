//
// Created by yww on 2021/7/14.
//

#ifndef BST__B_TREE_H_
#define BST__B_TREE_H_

#include "binary_search_tree.h"

#include <vector>
#include <cstring>

template<typename T, int M>
class BTree;

/**
 * A implementation of the nodes of B-tree.
 * @tparam T The datatype of the keys.
 * @tparam M The maximal number of children a node can have.
 */
template<typename T, int M>
class BTreeNodeWithPtr final {
 public:
  BTreeNodeWithPtr() : num_keys_{0}, is_leaf_{0} {
    memset(keys_, 0, sizeof(keys_));
    memset(children_, 0, sizeof(children_));
  }

  bool Insert(const T &key) {
    int cur_key_pos;
    for (cur_key_pos = 0; cur_key_pos < num_keys_ && *keys_[cur_key_pos] < key;) {
      cur_key_pos += 1;
    }
    if (cur_key_pos < num_keys_ && !(key < *keys_[cur_key_pos]) && !(*keys_[cur_key_pos] < key)) { // Equal
      return false;
    }
    if (is_leaf_) { // Insert here
      for (int i = num_keys_ - 1; i >= cur_key_pos; i--) {
        keys_[i + 1] = keys_[i];
      }
      keys_[cur_key_pos] = new T{key};
      num_keys_++;
      return true;
    }

    // 在每一层处理下一层溢出的情况，在 BTree class 里处理根节点溢出的情况
    bool flag = children_[cur_key_pos]->Insert(key);
    if (children_[cur_key_pos]->num_keys_ < M) {
      return flag;
    }

    BTreeNodeWithPtr<T, M> *right_child;
    T *mid_key;
    children_[cur_key_pos]->Split(right_child, mid_key);

    for (int i = num_keys_ - 1; i >= cur_key_pos; i--) {
      keys_[i + 1] = keys_[i];
      children_[i + 2] = children_[i + 1];
    }
    keys_[cur_key_pos] = mid_key;
    children_[cur_key_pos + 1] = right_child;
    num_keys_++;

    return flag;
  }

  bool Find(const T &key) {
    int cur_key_pos;
    for (cur_key_pos = 0; cur_key_pos < num_keys_ && *keys_[cur_key_pos] < key;) {
      cur_key_pos += 1;
    }
    if (cur_key_pos < num_keys_ && !(key < *keys_[cur_key_pos]) && !(*keys_[cur_key_pos] < key)) { // Equal
      return true;
    }

    if (this->is_leaf_) {
      return false;
    }
    return children_[cur_key_pos]->Find(key);
  }

  bool Erase(const T &key) {
    // 可以考虑用一个指针记下数组中第一个元素的位置？这样删除的时候就不用整体移动数组了，但是查找的时候需要不停的取模...
    // 可能没必要吧，可能会更慢？

    int cur_key_pos;
    for (cur_key_pos = 0; cur_key_pos < num_keys_ && *keys_[cur_key_pos] < key;) {
      cur_key_pos += 1;
    }

    bool flag = false; // 是否删除成功

    if (cur_key_pos < num_keys_ && !(key < *keys_[cur_key_pos]) && !(*keys_[cur_key_pos] < key)) { // Equal
      if (this->is_leaf_) {
        // 删除
        delete keys_[cur_key_pos];
        for (int i = cur_key_pos; i < num_keys_ - 1; i++) {
          keys_[i] = keys_[i + 1];
        }

        // Debug only
        keys_[num_keys_ - 1] = 0;
        // end

        num_keys_--;
        return true;
      } else {
        // TODO: 随机删前驱或后继
        BTreeNodeWithPtr<T, M> *cur;
        for (cur = this->children_[cur_key_pos + 1]; !cur->is_leaf_; cur = cur->children_[0]) { ;
        }
        std::swap(keys_[cur_key_pos], cur->keys_[0]);
        flag = this->children_[cur_key_pos + 1]->Erase(key);
        cur_key_pos++;
      }
    } else if (this->is_leaf_) {
      return false;
    } else {
      flag = children_[cur_key_pos]->Erase(key);
    }

    if (children_[cur_key_pos]->num_keys_ < kNumKeysMin) {
      // 尝试旋转修复
      int left = 0, right = 0;
      if (cur_key_pos > 0 && children_[cur_key_pos - 1]->num_keys_ > kNumKeysMin) {
          left = 1;
        }
      if (cur_key_pos < num_keys_ && children_[cur_key_pos + 1]->num_keys_ > kNumKeysMin) {
          right = 1;
        }
      if (Rand01()) {
        if (left) {
          RotateRight(cur_key_pos - 1);
          return flag;
        }
        if (right) {
          RotateLeft(cur_key_pos);
          return flag;
        }
      } else {
        if (right) {
          RotateLeft(cur_key_pos);
          return flag;
        }
        if (left) {
          RotateRight(cur_key_pos - 1);
          return flag;
        }
      }

      // 合并修复
      left = (cur_key_pos > 0);
      right = (cur_key_pos < num_keys_);

      BTreeNodeWithPtr<T, M> *merge_left, *merge_right;
      int mid_pos;

      if (left && (!right || Rand01())) {
        merge_left = children_[cur_key_pos - 1];
        merge_right = children_[cur_key_pos];
        mid_pos = cur_key_pos - 1;
      } else {
        merge_left = children_[cur_key_pos];
        merge_right = children_[cur_key_pos + 1];
        mid_pos = cur_key_pos;
      }

      merge_left->keys_[merge_left->num_keys_] = keys_[mid_pos];
      merge_left->num_keys_++;
      for (int i = 0; i < merge_right->num_keys_; i++) {
        merge_left->keys_[merge_left->num_keys_] = merge_right->keys_[i];
        merge_left->children_[merge_left->num_keys_] = merge_right->children_[i];
        merge_left->num_keys_++;
      }
      merge_left->children_[merge_left->num_keys_] = merge_right->children_[merge_right->num_keys_];
      delete merge_right;
      merge_right = nullptr;

      for (int i = mid_pos; i < num_keys_ - 1; i++) {
        keys_[i] = keys_[i + 1];
        children_[i + 1] = children_[i + 2];
      }
      // Debug begin
      keys_[num_keys_ - 1] = nullptr;
      children_[num_keys_] = nullptr;
      // end
      num_keys_--;
    }
    return flag;
  }

  static const int kNumKeysMin;

 private:

  /**
   * Split a node into two from the middle. The left part is stored in the current node.
   * @param right_node The right node is stored in right_node.
   * @param mid_key The key in the middle.
   */
  void Split(BTreeNodeWithPtr<T, M> *&right_node, T *&mid_key) {
    int mid_pos = M / 2; // TODO: 随机选择左边或右边的中位数
    mid_key = keys_[mid_pos];

    right_node = new BTreeNodeWithPtr;
    right_node->num_keys_ = num_keys_ - mid_pos - 1;
    right_node->is_leaf_ = is_leaf_;
    for (int i = 0; i < right_node->num_keys_; i++) {
      right_node->keys_[i] = keys_[mid_pos + i + 1];
      right_node->children_[i] = children_[mid_pos + i + 1];
    }
    right_node->children_[right_node->num_keys_] = children_[num_keys_];

    // For debug, not necessary
    for (int i = mid_pos; i < num_keys_; i++) {
      keys_[i] = nullptr;
      children_[i + 1] = nullptr;
    }
    // end
    num_keys_ = mid_pos;
  }

  /**
   * 向右旋转
   */
  void RotateRight(int cur_key_pos) {
    BTreeNodeWithPtr<T, M> *left_child = children_[cur_key_pos];
    BTreeNodeWithPtr<T, M> *right_child = children_[cur_key_pos + 1];

    T *key_temp_left = left_child->keys_[left_child->num_keys_ - 1];
    BTreeNodeWithPtr<T, M> *node_temp_left = left_child->children_[left_child->num_keys_];
    // Debug only
    left_child->keys_[left_child->num_keys_ - 1] = nullptr;
    left_child->children_[left_child->num_keys_] = nullptr;
    // end
    left_child->num_keys_--;

    T *key_temp_mid = keys_[cur_key_pos];
    keys_[cur_key_pos] = key_temp_left;

    for (int i = right_child->num_keys_ - 1; i >= 0; i--) {
      right_child->keys_[i + 1] = right_child->keys_[i];
      right_child->children_[i + 2] = right_child->children_[i + 1];
    }
    right_child->children_[1] = right_child->children_[0];
    right_child->keys_[0] = key_temp_mid;
    right_child->children_[0] = node_temp_left;
    right_child->num_keys_++;
  }

  /**
   * 向左旋转
   */
  void RotateLeft(int cur_key_pos) {
    BTreeNodeWithPtr<T, M> *left_child = children_[cur_key_pos];
    BTreeNodeWithPtr<T, M> *right_child = children_[cur_key_pos + 1];

    T *key_temp_right = right_child->keys_[0];
    BTreeNodeWithPtr<T, M> *node_temp_right = right_child->children_[0];
    right_child->children_[0] = right_child->children_[1];
    for (int i = 0; i < right_child->num_keys_ - 1; i++) {
      right_child->keys_[i] = right_child->keys_[i + 1];
      right_child->children_[i + 1] = right_child->children_[i + 2];
    }
    // Debug only
    right_child->keys_[right_child->num_keys_ - 1] = nullptr;
    right_child->children_[right_child->num_keys_] = nullptr;
    // end
    right_child->num_keys_--;

    T *key_temp_mid = keys_[cur_key_pos];
    keys_[cur_key_pos] = key_temp_right;

    left_child->keys_[left_child->num_keys_] = key_temp_mid;
    left_child->children_[left_child->num_keys_ + 1] = node_temp_right;
    left_child->num_keys_++;
  }

  /**
   * 一个应该能用的不太随机数生成器 （应该只要保证树不会向一边偏就行了，随不随机可能不重要？
   * @return 0/1 随机数
   */
  static unsigned int Rand01() {
    return (++seed_) & 1;
  }
  static unsigned int seed_;

  T *keys_[M + 1]; // 多一位用来处理溢出的情况
  BTreeNodeWithPtr<T, M> *children_[M + 2];
  int num_keys_;
  bool is_leaf_;

  friend class BTree<T, M>;
};
template<typename T, int M>
unsigned int BTreeNodeWithPtr<T, M>::seed_ = 0;
template<typename T, int M>
const int BTreeNodeWithPtr<T, M>::kNumKeysMin = (M + 1) / 2 - 1;

/**
 * An implementation of B-tree.
 * @tparam T The datatype of the keys.
 * @tparam M The maximal number of children a node can have. It is better that m is odd, otherwise it will 倒向一侧.
 */
template<typename T, int M> // TODO: specify the size
class BTree : BinarySearchTree<T> {
 public:
  BTree() {
    root_ = new BTreeNodeWithPtr<T, M>;
    root_->is_leaf_ = true;
  }

  virtual bool Insert(const T &key) {
    int flag = root_->Insert(key);

    if (root_->num_keys_ < M) {
      return flag;
    }

    BTreeNodeWithPtr<T, M> *right_child;
    T *mid_key;
    root_->Split(right_child, mid_key);

    auto *new_root = new BTreeNodeWithPtr<T, M>;
    new_root->keys_[0] = mid_key;
    new_root->children_[0] = root_;
    new_root->children_[1] = right_child;
    new_root->num_keys_ = 1;
    root_ = new_root;

    return flag;
  }

  virtual bool Find(const T &key) {
    return root_->Find(key);
  }

  virtual bool Erase(const T &key) {
    bool flag = root_->Erase(key);
    if (!root_->is_leaf_ && root_->num_keys_ == 0) {
      BTreeNodeWithPtr<T, M> *new_root = root_->children_[0];
      delete root_;
      root_ = new_root;
    }
    return flag;
  }

 private:
  BTreeNodeWithPtr<T, M> *root_;
};

#endif //BST__B_TREE_H_
