//
// Created by yww on 2021/7/14.
//

#ifndef BST__BINARY_SEARCH_TREE_H_
#define BST__BINARY_SEARCH_TREE_H_

/**
 * Abstract data type: Binary search tree with unique keys and without values.
 * @tparam T The datatype of the keys.
 */
template<typename T> // TODO 加上自定义比较方式
class BinarySearchTree {
 public:
  /**
   * Default constructor
   */
  BinarySearchTree() = default;

  /**
   * Virtual destructor
   */
  virtual ~BinarySearchTree() = default;

  /**
   * Insert a key to the tree
   * @param key The key to be inserted.
   * @return Whether this insertion is successful (false means the key is already in the tree).
   */
  virtual bool Insert(const T &key) = 0;

  /**
   * Find a key in the tree.
   * @param key The key to be found.
   * @return Whether the key is in the tree.
   */
  virtual bool Find(const T &key) = 0;

  /**
   * Erase a key in the tree.
   * @param key The key to be erased.
   * @return Whether this erasure is successful.
   */
  virtual bool Erase(const T &key) = 0;
};

#endif //BST__BINARY_SEARCH_TREE_H_
