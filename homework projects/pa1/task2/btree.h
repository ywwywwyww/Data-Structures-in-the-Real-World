/*******************************************************************************
 * tlx/container/btree.hpp
 *
 * Part of tlx - http://panthema.net/tlx
 *
 * Copyright (C) 2008-2017 Timo Bingmann <tb@panthema.net>
 *
 * All rights reserved. Published under the Boost Software License, Version 1.0
 ******************************************************************************/

#ifndef TLX_CONTAINER_BTREE_HEADER
#define TLX_CONTAINER_BTREE_HEADER

#include "die.h"

// *** Required Headers from the STL

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <functional>
#include <istream>
#include <memory>
#include <ostream>
#include <utility>

namespace tlx {

//! \addtogroup tlx_container
//! \{
//! \defgroup tlx_container_btree B+ Trees
//! B+ tree variants
//! \{

// *** Debugging Macros

#ifdef TLX_BTREE_DEBUG

#include <iostream>

//! Print out debug information to std::cout if TLX_BTREE_DEBUG is defined.
#define TLX_BTREE_PRINT(x)                                                     \
  do {                                                                         \
    if (debug)                                                                 \
      (std::cout << x << std::endl);                                           \
  } while (0)

//! Assertion only if TLX_BTREE_DEBUG is defined. This is not used in verify().
#define TLX_BTREE_ASSERT(x)                                                    \
  do {                                                                         \
    assert(x);                                                                 \
  } while (0)

#else

//! Print out debug information to std::cout if TLX_BTREE_DEBUG is defined.
#define TLX_BTREE_PRINT(x)                                                     \
  do {                                                                         \
  } while (0)

//! Assertion only if TLX_BTREE_DEBUG is defined. This is not used in verify().
#define TLX_BTREE_ASSERT(x)                                                    \
  do {                                                                         \
  } while (0)

#endif

//! The maximum of a and b. Used in some compile-time formulas.
#define TLX_BTREE_MAX(a, b) ((a) < (b) ? (b) : (a))

#ifndef TLX_BTREE_FRIENDS
//! The macro TLX_BTREE_FRIENDS can be used by outside class to access the B+
//! tree internals. This was added for wxBTreeDemo to be able to draw the
//! tree.
#define TLX_BTREE_FRIENDS friend class btree_friend
#endif

/*!
 * Generates default traits for a B+ tree used as a set or map. It estimates
 * leaf and inner node sizes by assuming a cache line multiple of 256 bytes.
 */
template<typename Key, typename Value>
struct btree_default_traits {
  //! If true, the tree will self verify its invariants after each insert() or
  //! erase(). The header must have been compiled with TLX_BTREE_DEBUG
  //! defined.
  static const bool self_verify = false;

  //! If true, the tree will print out debug information and a tree dump
  //! during insert() or erase() operation. The header must have been
  //! compiled with TLX_BTREE_DEBUG defined and key_type must be std::ostream
  //! printable.
  static const bool debug = false;

  //! Number of slots in each leaf of the tree. Estimated so that each node
  //! has a size of about 256 bytes.
  static const int leaf_slots = TLX_BTREE_MAX(8, 256 / (sizeof(Value)));

  //! Number of slots in each inner node of the tree. Estimated so that each
  //! node has a size of about 256 bytes.
  static const int inner_slots =
      TLX_BTREE_MAX(8, 256 / (sizeof(Key) + sizeof(void *) + sizeof(unsigned int)));

  //! As of stx-btree-0.9, the code does linear search in find_lower() and
  //! find_upper() instead of binary_search, unless the node size is larger
  //! than this threshold. See notes at
  //! http://panthema.net/2013/0504-STX-B+Tree-Binary-vs-Linear-Search
  static const size_t binsearch_threshold = 256;
};

/*!
 * Basic class implementing a B+ tree data structure in memory.
 *
 * The base implementation of an in-memory B+ tree. It is based on the
 * implementation in Cormen's Introduction into Algorithms, Jan Jannink's paper
 * and other algorithm resources. Almost all STL-required function calls are
 * implemented. The asymptotic time requirements of the STL are not always
 * fulfilled in theory, however, in practice this B+ tree performs better than a
 * red-black tree and almost always uses less memory. The insertion function
 * splits the nodes on the recursion unroll. Erase is largely based on Jannink's
 * ideas.
 *
 * This class is specialized into btree_set, btree_multiset, btree_map and
 * btree_multimap using default template parameters and facade functions.
 */
template<typename Key, typename Value, typename KeyOfValue,
    typename Compare = std::less<Key>,
    typename Traits = btree_default_traits<Key, Value>,
    bool Duplicates = false, typename Allocator = std::allocator<Value>>
class BTree {
 public:
  //! \name Template Parameter Types
  //! \{

  //! First template parameter: The key type of the B+ tree. This is stored in
  //! inner nodes.
  typedef Key key_type;

  //! Second template parameter: Composition pair of key and data types, or
  //! just the key for set containers. This data type is stored in the leaves.
  typedef Value value_type;

  //! Third template: key extractor class to pull key_type from value_type.
  typedef KeyOfValue key_of_value;

  //! Fourth template parameter: key_type comparison function object
  typedef Compare key_compare;

  //! Fifth template parameter: Traits object used to define more parameters
  //! of the B+ tree
  typedef Traits traits;

  //! Sixth template parameter: Allow duplicate keys in the B+ tree. Used to
  //! implement multiset and multimap.
  static const bool allow_duplicates = Duplicates;

  //! Seventh template parameter: STL allocator for tree nodes
  typedef Allocator allocator_type;

  //! \}

  // The macro TLX_BTREE_FRIENDS can be used by outside class to access the B+
  // tree internals. This was added for wxBTreeDemo to be able to draw the
  // tree.
  TLX_BTREE_FRIENDS;

 public:
  //! \name Constructed Types
  //! \{

  //! Typedef of our own type
  typedef BTree<key_type, value_type, key_of_value, key_compare, traits,
                allow_duplicates, allocator_type>
      Self;

  //! Size type used to count keys
  typedef size_t size_type;

  //! \}

 public:
  //! \name Static Constant Options and Values of the B+ Tree
  //! \{

  //! Base B+ tree parameter: The number of key/data slots in each leaf
  static const unsigned short leaf_slotmax = traits::leaf_slots;

  //! Base B+ tree parameter: The number of key slots in each inner node,
  //! this can differ from slots in each leaf.
  static const unsigned short inner_slotmax = traits::inner_slots;

  //! Computed B+ tree parameter: The minimum number of key/data slots used
  //! in a leaf. If fewer slots are used, the leaf will be merged or slots
  //! shifted from it's siblings.
  static const unsigned short leaf_slotmin = (leaf_slotmax / 2);

  //! Computed B+ tree parameter: The minimum number of key slots used
  //! in an inner node. If fewer slots are used, the inner node will be
  //! merged or slots shifted from it's siblings.
  static const unsigned short inner_slotmin = (inner_slotmax / 2);

  //! Debug parameter: Enables expensive and thorough checking of the B+ tree
  //! invariants after each insert/erase operation.
  static const bool self_verify = traits::self_verify;

  //! Debug parameter: Prints out lots of debug information about how the
  //! algorithms change the tree. Requires the header file to be compiled
  //! with TLX_BTREE_DEBUG and the key type must be std::ostream printable.
  static const bool debug = traits::debug;

  //! \}

 private:
  //! \name Node Classes for In-Memory Nodes
  //! \{

  //! The header structure of each node in-memory. This structure is extended
  //! by InnerNode or LeafNode.
  struct node {
    //! Level in the b-tree, if level == 0 -> leaf node
    unsigned short level;

    //! Number of key slotuse use, so the number of valid children or data
    //! pointers
    unsigned short slotuse;

    //! Delayed initialisation of constructed node.
    void initialize(const unsigned short l) {
      level = l;
      slotuse = 0;
    }

    //! True if this is a leaf node.
    bool is_leafnode() const { return (level == 0); }
  };

  //! Extended structure of a inner node in-memory. Contains only keys and no
  //! data items.
  struct InnerNode : public node {
    //! Define an related allocator for the InnerNode structs.
    typedef typename Allocator::template rebind<InnerNode>::other alloc_type;

    //! Keys of children or data pointers
    key_type slotkey[inner_slotmax]; // NOLINT

    //! Pointers to children and the sizes
//    node *childid[inner_slotmax + 1]; // NOLINT

//    int child_size[inner_slotmax + 1];

    std::pair<node*, unsigned int> child_info[inner_slotmax + 1];

    //! Size of this subtree
    unsigned int size;

    //! Set variables to initial values.
    void initialize(const unsigned short l) {
      node::initialize(l);
      size = 0;
    }

    //! Return key in slot s
    const key_type &key(size_t s) const { return slotkey[s]; }

    //! True if the node's slots are full.
    bool is_full() const { return (node::slotuse == inner_slotmax); }

    //! True if few used entries, less than half full.
    bool is_few() const { return (node::slotuse <= inner_slotmin); }

    //! True if node has too few entries.
    bool is_underflow() const { return (node::slotuse < inner_slotmin); }
  };

  //! Extended structure of a leaf node in memory. Contains pairs of keys and
  //! data items. Key and data slots are kept together in value_type.
  struct LeafNode : public node {
    //! Define an related allocator for the LeafNode structs.
    typedef typename Allocator::template rebind<LeafNode>::other alloc_type;

    //! Array of (key, data) pairs
    value_type slotdata[leaf_slotmax]; // NOLINT

    //! Set variables to initial values
    void initialize() {
      node::initialize(0);
    }

    //! Return key in slot s.
    const key_type &key(size_t s) const {
      return key_of_value::get(slotdata[s]);
    }

    //! True if the node's slots are full.
    bool is_full() const { return (node::slotuse == leaf_slotmax); }

    //! True if few used entries, less than half full.
    bool is_few() const { return (node::slotuse <= leaf_slotmin); }

    //! True if node has too few entries.
    bool is_underflow() const { return (node::slotuse < leaf_slotmin); }

    //! Set the (key,data) pair in slot. Overloaded function used by
    //! bulk_load().
    void set_slot(unsigned short slot, const value_type &value) {
      TLX_BTREE_ASSERT(slot < node::slotuse);
      slotdata[slot] = value;
    }
  };

  //! \}


 public:
  //! \name Iterators and Reverse Iterators
  //! \{

  class iterator;
  class const_iterator;
  class reverse_iterator;
  class const_reverse_iterator;

  //! STL-like iterator object for B+ tree items. The iterator points to a
  //! specific slot number in a leaf.
  class iterator {
   public:
    // *** Types

    //! The key type of the btree. Returned by key().
    typedef typename BTree::key_type key_type;

    //! The value type of the btree. Returned by operator*().
    typedef typename BTree::value_type value_type;

    //! Reference to the value_type. STL required.
    typedef value_type &reference;

    //! Pointer to the value_type. STL required.
    typedef value_type *pointer;

    //! STL-magic iterator category
    typedef std::bidirectional_iterator_tag iterator_category;

    //! STL-magic
    typedef ptrdiff_t difference_type;

    //! Our own type
    typedef iterator self;

   private:
    // *** Members

    //! The currently referenced leaf node of the tree
    typename BTree::LeafNode *curr_leaf;

    //! Current key/data slot referenced
    unsigned short curr_slot;

    //! Friendly to the const_iterator, so it may access the two data items
    //! directly.
    friend class const_iterator;

    //! Also friendly to the reverse_iterator, so it may access the two
    //! data items directly.
    friend class reverse_iterator;

    //! Also friendly to the const_reverse_iterator, so it may access the
    //! two data items directly.
    friend class const_reverse_iterator;

    //! Also friendly to the base btree class, because erase_iter() needs
    //! to read the curr_leaf and curr_slot values directly.
    friend class BTree<key_type, value_type, key_of_value, key_compare, traits,
                       allow_duplicates, allocator_type>;

    // The macro TLX_BTREE_FRIENDS can be used by outside class to access
    // the B+ tree internals. This was added for wxBTreeDemo to be able to
    // draw the tree.
    TLX_BTREE_FRIENDS;

   public:
    // *** Methods

    //! Default-Constructor of a mutable iterator
    iterator() : curr_leaf(nullptr), curr_slot(0) {}

    //! Initializing-Constructor of a mutable iterator
    iterator(typename BTree::LeafNode *l, unsigned short s)
        : curr_leaf(l), curr_slot(s) {}

    //! Copy-constructor from a reverse iterator
    iterator(const reverse_iterator &it) // NOLINT
        : curr_leaf(it.curr_leaf), curr_slot(it.curr_slot) {}

    //! Dereference the iterator.
    reference operator*() const { return curr_leaf->slotdata[curr_slot]; }

    //! Dereference the iterator.
    pointer operator->() const { return &curr_leaf->slotdata[curr_slot]; }

    //! Key of the current slot.
    const key_type &key() const { return curr_leaf->key(curr_slot); }


    //! Equality of iterators.
    bool operator==(const iterator &x) const {
      return (x.curr_leaf == curr_leaf) && (x.curr_slot == curr_slot);
    }

    //! Inequality of iterators.
    bool operator!=(const iterator &x) const {
      return (x.curr_leaf != curr_leaf) || (x.curr_slot != curr_slot);
    }
  };

  //! STL-like read-only iterator object for B+ tree items. The iterator
  //! points to a specific slot number in a leaf.
  class const_iterator {
   public:
    // *** Types

    //! The key type of the btree. Returned by key().
    typedef typename BTree::key_type key_type;

    //! The value type of the btree. Returned by operator*().
    typedef typename BTree::value_type value_type;

    //! Reference to the value_type. STL required.
    typedef const value_type &reference;

    //! Pointer to the value_type. STL required.
    typedef const value_type *pointer;

    //! STL-magic iterator category
    typedef std::bidirectional_iterator_tag iterator_category;

    //! STL-magic
    typedef ptrdiff_t difference_type;

    //! Our own type
    typedef const_iterator self;

   private:
    // *** Members

    //! The currently referenced leaf node of the tree
    const typename BTree::LeafNode *curr_leaf;

    //! Current key/data slot referenced
    unsigned short curr_slot;

    //! Friendly to the reverse_const_iterator, so it may access the two
    //! data items directly
    friend class const_reverse_iterator;

    // The macro TLX_BTREE_FRIENDS can be used by outside class to access
    // the B+ tree internals. This was added for wxBTreeDemo to be able to
    // draw the tree.
    TLX_BTREE_FRIENDS;

   public:
    // *** Methods

    //! Default-Constructor of a const iterator
    const_iterator() : curr_leaf(nullptr), curr_slot(0) {}

    //! Initializing-Constructor of a const iterator
    const_iterator(const typename BTree::LeafNode *l, unsigned short s)
        : curr_leaf(l), curr_slot(s) {}

    //! Copy-constructor from a mutable iterator
    const_iterator(const iterator &it) // NOLINT
        : curr_leaf(it.curr_leaf), curr_slot(it.curr_slot) {}

    //! Copy-constructor from a mutable reverse iterator
    const_iterator(const reverse_iterator &it) // NOLINT
        : curr_leaf(it.curr_leaf), curr_slot(it.curr_slot) {}

    //! Copy-constructor from a const reverse iterator
    const_iterator(const const_reverse_iterator &it) // NOLINT
        : curr_leaf(it.curr_leaf), curr_slot(it.curr_slot) {}

    //! Dereference the iterator.
    reference operator*() const { return curr_leaf->slotdata[curr_slot]; }

    //! Dereference the iterator.
    pointer operator->() const { return &curr_leaf->slotdata[curr_slot]; }

    //! Key of the current slot.
    const key_type &key() const { return curr_leaf->key(curr_slot); }

    //! Equality of iterators.
    bool operator==(const const_iterator &x) const {
      return (x.curr_leaf == curr_leaf) && (x.curr_slot == curr_slot);
    }

    //! Inequality of iterators.
    bool operator!=(const const_iterator &x) const {
      return (x.curr_leaf != curr_leaf) || (x.curr_slot != curr_slot);
    }
  };

  //! STL-like mutable reverse iterator object for B+ tree items. The
  //! iterator points to a specific slot number in a leaf.
  class reverse_iterator {
   public:
    // *** Types

    //! The key type of the btree. Returned by key().
    typedef typename BTree::key_type key_type;

    //! The value type of the btree. Returned by operator*().
    typedef typename BTree::value_type value_type;

    //! Reference to the value_type. STL required.
    typedef value_type &reference;

    //! Pointer to the value_type. STL required.
    typedef value_type *pointer;

    //! STL-magic iterator category
    typedef std::bidirectional_iterator_tag iterator_category;

    //! STL-magic
    typedef ptrdiff_t difference_type;

    //! Our own type
    typedef reverse_iterator self;

   private:
    // *** Members

    //! The currently referenced leaf node of the tree
    typename BTree::LeafNode *curr_leaf;

    //! One slot past the current key/data slot referenced.
    unsigned short curr_slot;

    //! Friendly to the const_iterator, so it may access the two data items
    //! directly
    friend class iterator;

    //! Also friendly to the const_iterator, so it may access the two data
    //! items directly
    friend class const_iterator;

    //! Also friendly to the const_iterator, so it may access the two data
    //! items directly
    friend class const_reverse_iterator;

    // The macro TLX_BTREE_FRIENDS can be used by outside class to access
    // the B+ tree internals. This was added for wxBTreeDemo to be able to
    // draw the tree.
    TLX_BTREE_FRIENDS;

   public:
    // *** Methods

    //! Default-Constructor of a reverse iterator
    reverse_iterator() : curr_leaf(nullptr), curr_slot(0) {}

    //! Initializing-Constructor of a mutable reverse iterator
    reverse_iterator(typename BTree::LeafNode *l, unsigned short s)
        : curr_leaf(l), curr_slot(s) {}

    //! Copy-constructor from a mutable iterator
    reverse_iterator(const iterator &it) // NOLINT
        : curr_leaf(it.curr_leaf), curr_slot(it.curr_slot) {}

    //! Dereference the iterator.
    reference operator*() const {
      TLX_BTREE_ASSERT(curr_slot > 0);
      return curr_leaf->slotdata[curr_slot - 1];
    }

    //! Dereference the iterator.
    pointer operator->() const {
      TLX_BTREE_ASSERT(curr_slot > 0);
      return &curr_leaf->slotdata[curr_slot - 1];
    }

    //! Key of the current slot.
    const key_type &key() const {
      TLX_BTREE_ASSERT(curr_slot > 0);
      return curr_leaf->key(curr_slot - 1);
    }

    //! Equality of iterators.
    bool operator==(const reverse_iterator &x) const {
      return (x.curr_leaf == curr_leaf) && (x.curr_slot == curr_slot);
    }

    //! Inequality of iterators.
    bool operator!=(const reverse_iterator &x) const {
      return (x.curr_leaf != curr_leaf) || (x.curr_slot != curr_slot);
    }
  };

  //! STL-like read-only reverse iterator object for B+ tree items. The
  //! iterator points to a specific slot number in a leaf.
  class const_reverse_iterator {
   public:
    // *** Types

    //! The key type of the btree. Returned by key().
    typedef typename BTree::key_type key_type;

    //! The value type of the btree. Returned by operator*().
    typedef typename BTree::value_type value_type;

    //! Reference to the value_type. STL required.
    typedef const value_type &reference;

    //! Pointer to the value_type. STL required.
    typedef const value_type *pointer;

    //! STL-magic iterator category
    typedef std::bidirectional_iterator_tag iterator_category;

    //! STL-magic
    typedef ptrdiff_t difference_type;

    //! Our own type
    typedef const_reverse_iterator self;

   private:
    // *** Members

    //! The currently referenced leaf node of the tree
    const typename BTree::LeafNode *curr_leaf;

    //! One slot past the current key/data slot referenced.
    unsigned short curr_slot;

    //! Friendly to the const_iterator, so it may access the two data items
    //! directly.
    friend class reverse_iterator;

    // The macro TLX_BTREE_FRIENDS can be used by outside class to access
    // the B+ tree internals. This was added for wxBTreeDemo to be able to
    // draw the tree.
    TLX_BTREE_FRIENDS;

   public:
    // *** Methods

    //! Default-Constructor of a const reverse iterator.
    const_reverse_iterator() : curr_leaf(nullptr), curr_slot(0) {}

    //! Initializing-Constructor of a const reverse iterator.
    const_reverse_iterator(const typename BTree::LeafNode *l, unsigned short s)
        : curr_leaf(l), curr_slot(s) {}

    //! Copy-constructor from a mutable iterator.
    const_reverse_iterator(const iterator &it) // NOLINT
        : curr_leaf(it.curr_leaf), curr_slot(it.curr_slot) {}

    //! Copy-constructor from a const iterator.
    const_reverse_iterator(const const_iterator &it) // NOLINT
        : curr_leaf(it.curr_leaf), curr_slot(it.curr_slot) {}

    //! Copy-constructor from a mutable reverse iterator.
    const_reverse_iterator(const reverse_iterator &it) // NOLINT
        : curr_leaf(it.curr_leaf), curr_slot(it.curr_slot) {}

    //! Dereference the iterator.
    reference operator*() const {
      TLX_BTREE_ASSERT(curr_slot > 0);
      return curr_leaf->slotdata[curr_slot - 1];
    }

    //! Dereference the iterator.
    pointer operator->() const {
      TLX_BTREE_ASSERT(curr_slot > 0);
      return &curr_leaf->slotdata[curr_slot - 1];
    }

    //! Key of the current slot.
    const key_type &key() const {
      TLX_BTREE_ASSERT(curr_slot > 0);
      return curr_leaf->key(curr_slot - 1);
    }


    //! Equality of iterators.
    bool operator==(const const_reverse_iterator &x) const {
      return (x.curr_leaf == curr_leaf) && (x.curr_slot == curr_slot);
    }

    //! Inequality of iterators.
    bool operator!=(const const_reverse_iterator &x) const {
      return (x.curr_leaf != curr_leaf) || (x.curr_slot != curr_slot);
    }
  };

  //! \}



 public:
  //! \name Small Statistics Structure
  //! \{

  /*!
   * A small struct containing basic statistics about the B+ tree. It can be
   * fetched using get_stats().
   */
  struct tree_stats {
    //! Number of items in the B+ tree
    size_type size;

    //! Number of leaves in the B+ tree
    size_type leaves;

    //! Number of inner nodes in the B+ tree
    size_type inner_nodes;

    //! Base B+ tree parameter: The number of key/data slots in each leaf
    static const unsigned short leaf_slots = Self::leaf_slotmax;

    //! Base B+ tree parameter: The number of key slots in each inner node.
    static const unsigned short inner_slots = Self::inner_slotmax;

    //! Zero initialized
    tree_stats() : size(0), leaves(0), inner_nodes(0) {}

    //! Return the total number of nodes
    size_type nodes() const { return inner_nodes + leaves; }

    //! Return the average fill of leaves
    double avgfill_leaves() const {
      return static_cast<double>(size) / (leaves * leaf_slots);
    }
  };

  //! \}

 private:
  //! \name Tree Object Data Members
  //! \{

  //! Pointer to the B+ tree's root node, either leaf or inner node.
  node *root_;

  //! Pointer to first leaf in the double linked leaf chain.
  LeafNode *head_leaf_;

  //! Pointer to last leaf in the double linked leaf chain.
  LeafNode *tail_leaf_;

  //! Other small statistics about the B+ tree.
  tree_stats stats_;

  //! Key comparison object. More comparison functions are generated from
  //! this < relation.
  key_compare key_less_;

  //! Memory allocator.
  allocator_type allocator_;

  //! \}

 public:
  //! \name Constructors and Destructor
  //! \{

  //! Default constructor initializing an empty B+ tree with the standard key
  //! comparison function.
  explicit BTree(const allocator_type &alloc = allocator_type())
      : root_(nullptr), head_leaf_(nullptr), tail_leaf_(nullptr),
        allocator_(alloc) {}

  //! Constructor initializing an empty B+ tree with a special key
  //! comparison object.
  explicit BTree(const key_compare &kcf,
                 const allocator_type &alloc = allocator_type())
      : root_(nullptr), head_leaf_(nullptr), tail_leaf_(nullptr),
        key_less_(kcf), allocator_(alloc) {}

  //! Constructor initializing a B+ tree with the range [first,last). The
  //! range need not be sorted. To create a B+ tree from a sorted range, use
  //! bulk_load().
  template<class InputIterator>
  BTree(InputIterator first, InputIterator last,
        const allocator_type &alloc = allocator_type())
      : root_(nullptr), head_leaf_(nullptr), tail_leaf_(nullptr),
        allocator_(alloc) {
    insert(first, last);
  }

  //! Constructor initializing a B+ tree with the range [first,last) and a
  //! special key comparison object.  The range need not be sorted. To create
  //! a B+ tree from a sorted range, use bulk_load().
  template<class InputIterator>
  BTree(InputIterator first, InputIterator last, const key_compare &kcf,
        const allocator_type &alloc = allocator_type())
      : root_(nullptr), head_leaf_(nullptr), tail_leaf_(nullptr),
        key_less_(kcf), allocator_(alloc) {
    insert(first, last);
  }

  //! Frees up all used B+ tree memory pages
  ~BTree() { clear(); }

  //! Fast swapping of two identical B+ tree objects.
  void swap(BTree &from) {
    std::swap(root_, from.root_);
    std::swap(head_leaf_, from.head_leaf_);
    std::swap(tail_leaf_, from.tail_leaf_);
    std::swap(stats_, from.stats_);
    std::swap(key_less_, from.key_less_);
    std::swap(allocator_, from.allocator_);
  }

  //! \}

 public:
  //! \name Key and Value Comparison Function Objects
  //! \{

  //! Function class to compare value_type objects. Required by the STL
  class value_compare {
   protected:
    //! Key comparison function from the template parameter
    key_compare key_comp;

    //! Constructor called from BTree::value_comp()
    explicit value_compare(key_compare kc) : key_comp(kc) {}

    //! Friendly to the btree class so it may call the constructor
    friend class BTree<key_type, value_type, key_of_value, key_compare, traits,
                       allow_duplicates, allocator_type>;

   public:
    //! Function call "less"-operator resulting in true if x < y.
    bool operator()(const value_type &x, const value_type &y) const {
      return key_comp(x.first, y.first);
    }
  };

  //! Constant access to the key comparison object sorting the B+ tree.
  key_compare key_comp() const { return key_less_; }

  //! Constant access to a constructed value_type comparison object. Required
  //! by the STL.
  value_compare value_comp() const { return value_compare(key_less_); }

  //! \}

 private:
  //! \name Convenient Key Comparison Functions Generated From key_less
  //! \{

  //! True if a < b ? "constructed" from key_less_()
  bool key_less(const key_type &a, const key_type &b) const {
    return key_less_(a, b);
  }

  //! True if a <= b ? constructed from key_less()
  bool key_lessequal(const key_type &a, const key_type &b) const {
    return !key_less_(b, a);
  }

  //! True if a > b ? constructed from key_less()
  bool key_greater(const key_type &a, const key_type &b) const {
    return key_less_(b, a);
  }

  //! True if a >= b ? constructed from key_less()
  bool key_greaterequal(const key_type &a, const key_type &b) const {
    return !key_less_(a, b);
  }

  //! True if a == b ? constructed from key_less(). This requires the <
  //! relation to be a total order, otherwise the B+ tree cannot be sorted.
  bool key_equal(const key_type &a, const key_type &b) const {
    return !key_less_(a, b) && !key_less_(b, a);
  }

  //! \}

 public:
  //! \name Allocators
  //! \{

  //! Return the base node allocator provided during construction.
  allocator_type get_allocator() const { return allocator_; }

  //! \}

 private:
  //! \name Node Object Allocation and Deallocation Functions
  //! \{

  //! Return an allocator for LeafNode objects.
  typename LeafNode::alloc_type leaf_node_allocator() {
    return typename LeafNode::alloc_type(allocator_);
  }

  //! Return an allocator for InnerNode objects.
  typename InnerNode::alloc_type inner_node_allocator() {
    return typename InnerNode::alloc_type(allocator_);
  }

  //! Allocate and initialize a leaf node
  LeafNode *allocate_leaf() {
    LeafNode *n = new(leaf_node_allocator().allocate(1)) LeafNode();
    n->initialize();
    stats_.leaves++;
    return n;
  }

  //! Allocate and initialize an inner node
  InnerNode *allocate_inner(unsigned short level) {
    InnerNode *n = new(inner_node_allocator().allocate(1)) InnerNode();
    n->initialize(level);
    stats_.inner_nodes++;
    return n;
  }

  //! Correctly free either inner or leaf node, destructs all contained key
  //! and value objects.
  void free_node(node *n) {
    if (n->is_leafnode()) {
      LeafNode *ln = static_cast<LeafNode *>(n);
      typename LeafNode::alloc_type a(leaf_node_allocator());
      a.destroy(ln);
      a.deallocate(ln, 1);
      stats_.leaves--;
    } else {
      InnerNode *in = static_cast<InnerNode *>(n);
      typename InnerNode::alloc_type a(inner_node_allocator());
      a.destroy(in);
      a.deallocate(in, 1);
      stats_.inner_nodes--;
    }
  }

  //! \}

 public:
  //! \name Fast Destruction of the B+ Tree
  //! \{

  //! Frees all key/data pairs and all nodes of the tree.
  void clear() {
    if (root_) {
      clear_recursive(root_);
      free_node(root_);

      root_ = nullptr;
      head_leaf_ = tail_leaf_ = nullptr;

      stats_ = tree_stats();
    }

    TLX_BTREE_ASSERT(stats_.size == 0);
  }

 private:
  //! Recursively free up nodes.
  void clear_recursive(node *n) {
    if (n->is_leafnode()) {
      LeafNode *leafnode = static_cast<LeafNode *>(n);

      for (unsigned short slot = 0; slot < leafnode->slotuse; ++slot) {
        // data objects are deleted by LeafNode's destructor
      }
    } else {
      InnerNode *innernode = static_cast<InnerNode *>(n);

      for (unsigned short slot = 0; slot < innernode->slotuse + 1; ++slot) {
        clear_recursive(innernode->child_info[slot].first);
        free_node(innernode->child_info[slot].first);
      }
    }
  }

  //! \}

 public:
  //! \name STL Iterator Construction Functions
  //! \{

  //! Constructs a read/data-write iterator that points to the first slot in
  //! the first leaf of the B+ tree.
  iterator begin() { return iterator(head_leaf_, 0); }

  //! Constructs a read/data-write iterator that points to the first invalid
  //! slot in the last leaf of the B+ tree.
  iterator end() {
    return iterator(tail_leaf_, tail_leaf_ ? tail_leaf_->slotuse : 0);
  }

  //! Constructs a read-only constant iterator that points to the first slot
  //! in the first leaf of the B+ tree.
  const_iterator begin() const { return const_iterator(head_leaf_, 0); }

  //! Constructs a read-only constant iterator that points to the first
  //! invalid slot in the last leaf of the B+ tree.
  const_iterator end() const {
    return const_iterator(tail_leaf_, tail_leaf_ ? tail_leaf_->slotuse : 0);
  }

  //! Constructs a read/data-write reverse iterator that points to the first
  //! invalid slot in the last leaf of the B+ tree. Uses STL magic.
  reverse_iterator rbegin() { return reverse_iterator(end()); }

  //! Constructs a read/data-write reverse iterator that points to the first
  //! slot in the first leaf of the B+ tree. Uses STL magic.
  reverse_iterator rend() { return reverse_iterator(begin()); }

  //! Constructs a read-only reverse iterator that points to the first
  //! invalid slot in the last leaf of the B+ tree. Uses STL magic.
  const_reverse_iterator rbegin() const {
    return const_reverse_iterator(end());
  }

  //! Constructs a read-only reverse iterator that points to the first slot
  //! in the first leaf of the B+ tree. Uses STL magic.
  const_reverse_iterator rend() const {
    return const_reverse_iterator(begin());
  }

  //! \}

 private:
  //! \name B+ Tree Node Binary Search Functions
  //! \{

  //! Searches for the first key in the node n greater or equal to key. Uses
  //! binary search with an optional linear self-verification. This is a
  //! template function, because the slotkey array is located at different
  //! places in LeafNode and InnerNode.
  template<typename node_type>
  unsigned short find_lower(const node_type *n, const key_type &key) const {
    if (sizeof(*n) > traits::binsearch_threshold) {
      if (n->slotuse == 0)
        return 0;

      unsigned short lo = 0, hi = n->slotuse;

      while (lo < hi) {
        unsigned short mid = (lo + hi) >> 1;

        if (key_lessequal(key, n->key(mid))) {
          hi = mid; // key <= mid
        } else {
          lo = mid + 1; // key > mid
        }
      }

      TLX_BTREE_PRINT("BTree::find_lower: on " << n << " key " << key << " -> "
                                               << lo << " / " << hi);

      // verify result using simple linear search
      if (self_verify) {
        unsigned short i = 0;
        while (i < n->slotuse && key_less(n->key(i), key))
          ++i;

        TLX_BTREE_PRINT("BTree::find_lower: testfind: " << i);
        TLX_BTREE_ASSERT(i == lo);
      }

      return lo;
    } else // for nodes <= binsearch_threshold do linear search.
    {
      unsigned short lo = 0;
      while (lo < n->slotuse && key_less(n->key(lo), key))
        ++lo;
      return lo;
    }
  }

  //! Searches for the first key in the node n greater than key. Uses binary
  //! search with an optional linear self-verification. This is a template
  //! function, because the slotkey array is located at different places in
  //! LeafNode and InnerNode.
  template<typename node_type>
  unsigned short find_upper(const node_type *n, const key_type &key) const {
    if (sizeof(*n) > traits::binsearch_threshold) {
      if (n->slotuse == 0)
        return 0;

      unsigned short lo = 0, hi = n->slotuse;

      while (lo < hi) {
        unsigned short mid = (lo + hi) >> 1;

        if (key_less(key, n->key(mid))) {
          hi = mid; // key < mid
        } else {
          lo = mid + 1; // key >= mid
        }
      }

      TLX_BTREE_PRINT("BTree::find_upper: on " << n << " key " << key << " -> "
                                               << lo << " / " << hi);

      // verify result using simple linear search
      if (self_verify) {
        unsigned short i = 0;
        while (i < n->slotuse && key_lessequal(n->key(i), key))
          ++i;

        TLX_BTREE_PRINT("BTree::find_upper testfind: " << i);
        TLX_BTREE_ASSERT(i == hi);
      }

      return lo;
    } else // for nodes <= binsearch_threshold do linear search.
    {
      unsigned short lo = 0;
      while (lo < n->slotuse && key_lessequal(n->key(lo), key))
        ++lo;
      return lo;
    }
  }

  //! \}

 public:
  //! \name Access Functions to the Item Count
  //! \{

  //! Return the number of key/data pairs in the B+ tree
  size_type size() const { return stats_.size; }

  //! Returns true if there is at least one key/data pair in the B+ tree
  bool empty() const { return (size() == size_type(0)); }

  //! Returns the largest possible size of the B+ Tree. This is just a
  //! function required by the STL standard, the B+ Tree can hold more items.
  size_type max_size() const { return size_type(-1); }

  //! Return a const reference to the current statistics.
  const struct tree_stats &get_stats() const { return stats_; }

  //! \}

 public:
  //! \name STL Access Functions Querying the Tree by Descending to a Leaf
  //! \{

  //! Non-STL function checking whether a key is in the B+ tree. The same as
  //! (find(k) != end()) or (count() != 0).
  bool exists(const key_type &key) const {
    const node *n = root_;
    if (!n)
      return false;

    while (!n->is_leafnode()) {
      const InnerNode *inner = static_cast<const InnerNode *>(n);
      unsigned short slot = find_lower(inner, key);

      n = inner->child_info[slot].first;
    }

    const LeafNode *leaf = static_cast<const LeafNode *>(n);

    unsigned short slot = find_lower(leaf, key);
    return (slot < leaf->slotuse && key_equal(key, leaf->key(slot)));
  }

  //! Tries to locate a key in the B+ tree and returns an iterator to the
  //! key/data slot if found. If unsuccessful it returns end().
  iterator find(const key_type &key) {
    node *n = root_;
    if (!n)
      return end();

    while (!n->is_leafnode()) {
      const InnerNode *inner = static_cast<const InnerNode *>(n);
      unsigned short slot = find_lower(inner, key);

      n = inner->child_info[slot].first;
    }

    LeafNode *leaf = static_cast<LeafNode *>(n);

    unsigned short slot = find_lower(leaf, key);
    return (slot < leaf->slotuse && key_equal(key, leaf->key(slot)))
           ? iterator(leaf, slot)
           : end();
  }

  //! Tries to locate a key in the B+ tree and returns an constant iterator to
  //! the key/data slot if found. If unsuccessful it returns end().
  const_iterator find(const key_type &key) const {
    const node *n = root_;
    if (!n)
      return end();

    while (!n->is_leafnode()) {
      const InnerNode *inner = static_cast<const InnerNode *>(n);
      unsigned short slot = find_lower(inner, key);

      n = inner->child_info[slot].first;
    }

    const LeafNode *leaf = static_cast<const LeafNode *>(n);

    unsigned short slot = find_lower(leaf, key);
    return (slot < leaf->slotuse && key_equal(key, leaf->key(slot)))
           ? const_iterator(leaf, slot)
           : end();
  }

  unsigned int count_prefix(const key_type &key) const {
    const node *n = root_;
    if (!n)
      return 0;

    unsigned int ans = 0;

    while (!n->is_leafnode()) {
      const InnerNode *inner = static_cast<const InnerNode *>(n);
      unsigned short slot = find_lower(inner, key);

      for (int i = 0; i < slot; i++) {
        ans += inner->child_info[i].second;
      }

      n = inner->child_info[slot].first;
    }

    const LeafNode *leaf = static_cast<const LeafNode *>(n);

    unsigned short slot = find_lower(leaf, key);

    ans += slot;

    if (leaf && slot < leaf->slotuse && key_equal(key, leaf->key(slot))) {
      ans++;
    }

    return ans;
  }

  //! Searches the B+ tree and returns an iterator to the first pair equal to
  //! or greater than key, or end() if all keys are smaller.
  iterator lower_bound(const key_type &key) {
    node *n = root_;
    if (!n)
      return end();

    while (!n->is_leafnode()) {
      const InnerNode *inner = static_cast<const InnerNode *>(n);
      unsigned short slot = find_lower(inner, key);

      n = inner->child_info[slot].first;
    }

    LeafNode *leaf = static_cast<LeafNode *>(n);

    unsigned short slot = find_lower(leaf, key);
    return iterator(leaf, slot);
  }

  //! Searches the B+ tree and returns a constant iterator to the first pair
  //! equal to or greater than key, or end() if all keys are smaller.
  const_iterator lower_bound(const key_type &key) const {
    const node *n = root_;
    if (!n)
      return end();

    while (!n->is_leafnode()) {
      const InnerNode *inner = static_cast<const InnerNode *>(n);
      unsigned short slot = find_lower(inner, key);

      n = inner->child_info[slot].first;
    }

    const LeafNode *leaf = static_cast<const LeafNode *>(n);

    unsigned short slot = find_lower(leaf, key);
    return const_iterator(leaf, slot);
  }

  //! Searches the B+ tree and returns an iterator to the first pair greater
  //! than key, or end() if all keys are smaller or equal.
  iterator upper_bound(const key_type &key) {
    node *n = root_;
    if (!n)
      return end();

    while (!n->is_leafnode()) {
      const InnerNode *inner = static_cast<const InnerNode *>(n);
      unsigned short slot = find_upper(inner, key);

      n = inner->child_info[slot].first;
    }

    LeafNode *leaf = static_cast<LeafNode *>(n);

    unsigned short slot = find_upper(leaf, key);
    return iterator(leaf, slot);
  }

  //! Searches the B+ tree and returns a constant iterator to the first pair
  //! greater than key, or end() if all keys are smaller or equal.
  const_iterator upper_bound(const key_type &key) const {
    const node *n = root_;
    if (!n)
      return end();

    while (!n->is_leafnode()) {
      const InnerNode *inner = static_cast<const InnerNode *>(n);
      unsigned short slot = find_upper(inner, key);

      n = inner->child_info[slot].first;
    }

    const LeafNode *leaf = static_cast<const LeafNode *>(n);

    unsigned short slot = find_upper(leaf, key);
    return const_iterator(leaf, slot);
  }

  //! Searches the B+ tree and returns both lower_bound() and upper_bound().
  std::pair<iterator, iterator> equal_range(const key_type &key) {
    return std::pair<iterator, iterator>(lower_bound(key), upper_bound(key));
  }

  //! Searches the B+ tree and returns both lower_bound() and upper_bound().
  std::pair<const_iterator, const_iterator>
  equal_range(const key_type &key) const {
    return std::pair<const_iterator, const_iterator>(lower_bound(key),
                                                     upper_bound(key));
  }

  //! \}

 public:
  //! \name B+ Tree Object Comparison Functions
  //! \{

  //! Equality relation of B+ trees of the same type. B+ trees of the same
  //! size and equal elements (both key and data) are considered equal. Beware
  //! of the random ordering of duplicate keys.
  bool operator==(const BTree &other) const {
    return (size() == other.size()) &&
        std::equal(begin(), end(), other.begin());
  }

  //! Inequality relation. Based on operator==.
  bool operator!=(const BTree &other) const { return !(*this == other); }

  //! Total ordering relation of B+ trees of the same type. It uses
  //! std::lexicographical_compare() for the actual comparison of elements.
  bool operator<(const BTree &other) const {
    return std::lexicographical_compare(begin(), end(), other.begin(),
                                        other.end());
  }

  //! Greater relation. Based on operator<.
  bool operator>(const BTree &other) const { return other < *this; }

  //! Less-equal relation. Based on operator<.
  bool operator<=(const BTree &other) const { return !(other < *this); }

  //! Greater-equal relation. Based on operator<.
  bool operator>=(const BTree &other) const { return !(*this < other); }

  //! \}

 public:
  //! \name Fast Copy: Assign Operator and Copy Constructors
  //! \{

  //! Assignment operator. All the key/data pairs are copied.
  BTree &operator=(const BTree &other) {
    if (this != &other) {
      clear();

      key_less_ = other.key_comp();
      allocator_ = other.get_allocator();

      if (other.size() != 0) {
        stats_.leaves = stats_.inner_nodes = 0;
        if (other.root_) {
          root_ = copy_recursive(other.root_);
        }
        stats_ = other.stats_;
      }
    }
    return *this;
  }

  //! Copy constructor. The newly initialized B+ tree object will contain a
  //! copy of all key/data pairs.
  BTree(const BTree &other)
      : root_(nullptr), head_leaf_(nullptr), tail_leaf_(nullptr),
        stats_(other.stats_), key_less_(other.key_comp()),
        allocator_(other.get_allocator()) {
    if (size() > 0) {
      stats_.leaves = stats_.inner_nodes = 0;
      if (other.root_) {
        root_ = copy_recursive(other.root_);
      }
    }
  }

 public:
  //! \name Public Insertion Functions
  //! \{

  //! Attempt to insert a key/data pair into the B+ tree. If the tree does not
  //! allow duplicate keys, then the insert may fail if it is already present.
  bool insert(const value_type &x) {
    return insert_start(key_of_value::get(x), x);
  }


 private:
  //! Start the insertion descent at the current root and handle root splits.
  //! Returns true if the item was inserted
  bool insert_start(const key_type &key,
                                         const value_type &value) {

    node *newchild = nullptr;
    key_type newkey = key_type();
    unsigned new_size = 0;

    if (root_ == nullptr) {
      root_ = head_leaf_ = tail_leaf_ = allocate_leaf();
    }

    bool r =
        insert_descend(root_, key, value, &newkey, &newchild, new_size);

    // increment size if the item was inserted
    if (r)
      ++stats_.size;

    if (newchild) {
      // this only occurs if insert_descend() could not insert the key
      // into the root node, this mean the root is full and a new root
      // needs to be created.
      InnerNode *newroot = allocate_inner(root_->level + 1);
      newroot->slotkey[0] = newkey;

      newroot->child_info[0].first = root_;
      newroot->child_info[1].first = newchild;

      newroot->child_info[0].second = stats_.size - new_size;
      newroot->child_info[1].second = new_size;

      newroot->size = stats_.size;

      newroot->slotuse = 1;

      root_ = newroot;
    }

#ifdef TLX_BTREE_DEBUG
    if (debug)
      print(std::cout);
#endif
    return r;
  }

  /*!
   * Insert an item into the B+ tree.
   *
   * Descend down the nodes to a leaf, insert the key/data pair in a free
   * slot. If the node overflows, then it must be split and the new split node
   * inserted into the parent. Unroll / this splitting up to the root.
   */
  bool insert_descend(node *n, const key_type &key,
                                           const value_type &value,
                                           key_type *splitkey,
                                           node **splitnode, unsigned &split_size) {

    bool is_in_split_node = false;

    if (!n->is_leafnode()) {
      InnerNode *inner = static_cast<InnerNode *>(n);

      key_type newkey = key_type();
      node *newchild = nullptr;
      unsigned new_size = 0;

      unsigned short slot = find_lower(inner, key);

      TLX_BTREE_PRINT("BTree::insert_descend into " << inner->child_info[slot].first);

      bool r =
          insert_descend(inner->child_info[slot].first, key, value, &newkey, &newchild, new_size);
      if (r == true) {
        inner->child_info[slot].second++;
        inner->size++;
      }

      if (newchild) {
        TLX_BTREE_PRINT("BTree::insert_descend newchild"
                            << " with key " << newkey << " node " << newchild
                            << " at slot " << slot);
        inner->child_info[slot].second -= new_size;
        inner->size -= new_size;

        if (inner->is_full()) {
          split_inner_node(inner, splitkey, splitnode, split_size, slot);

          TLX_BTREE_PRINT("BTree::insert_descend done split_inner:"
                              << " putslot: " << slot << " putkey: " << newkey
                              << " upkey: " << *splitkey);

#ifdef TLX_BTREE_DEBUG
          if (debug) {
            print_node(std::cout, inner);
            print_node(std::cout, *splitnode);
          }
#endif

          // check if insert slot is in the split sibling node
          TLX_BTREE_PRINT("BTree::insert_descend switch: "
                              << slot << " > " << inner->slotuse + 1);

          if (slot == inner->slotuse + 1 &&
              inner->slotuse < (*splitnode)->slotuse) {
            // special case when the insert slot matches the split
            // place between the two nodes, then the insert key
            // becomes the split key.

            TLX_BTREE_ASSERT(inner->slotuse + 1 < inner_slotmax);

            InnerNode *split = static_cast<InnerNode *>(*splitnode);

            // move the split key and it's datum into the left node
            inner->slotkey[inner->slotuse] = *splitkey;
            inner->child_info[inner->slotuse + 1].first = split->child_info[0].first;
            inner->child_info[inner->slotuse + 1].second = split->child_info[0].second;
            split->size -= split->child_info[0].second;
            split_size -= split->child_info[0].second;
            inner->size += split->child_info[0].second;
            inner->slotuse++;

            // set new split key and move corresponding datum into
            // right node
            split->child_info[0].first = newchild;
            *splitkey = newkey;
            split->child_info[0].second = new_size;
            split->size += new_size;
            split_size += new_size;

            return r;
          } else if (slot >= inner->slotuse + 1) {
            // in case the insert slot is in the newly create split
            // node, we reuse the code below.

            slot -= inner->slotuse + 1;
            inner = static_cast<InnerNode *>(*splitnode);
            is_in_split_node = true;
            TLX_BTREE_PRINT("BTree::insert_descend switching to "
                            "splitted node "
                                << inner << " slot " << slot);
          }
        }

        // move items and put pointer to child node into correct slot
        TLX_BTREE_ASSERT(slot >= 0 && slot <= inner->slotuse);

        std::copy_backward(inner->slotkey + slot,
                           inner->slotkey + inner->slotuse,
                           inner->slotkey + inner->slotuse + 1);
        std::copy_backward(inner->child_info + slot,
                           inner->child_info + inner->slotuse + 1,
                           inner->child_info + inner->slotuse + 2);

        inner->slotkey[slot] = newkey;
        inner->child_info[slot + 1].first = newchild;
        inner->child_info[slot + 1].second = new_size;
        inner->size += new_size;
        inner->slotuse++;
        if (is_in_split_node) {
          split_size += new_size;
        }

      }

      return r;
    } else // n->is_leafnode() == true
    {
      LeafNode *leaf = static_cast<LeafNode *>(n);

      unsigned short slot = find_lower(leaf, key);

      if (!allow_duplicates && slot < leaf->slotuse &&
          key_equal(key, leaf->key(slot))) {
        return false;
      }

      if (leaf->is_full()) {
        split_leaf_node(leaf, splitkey, splitnode, split_size);

        // check if insert slot is in the split sibling node
        if (slot >= leaf->slotuse) {
          slot -= leaf->slotuse;
          leaf = static_cast<LeafNode *>(*splitnode);
          is_in_split_node = true;
        }
      }

      // move items and put data item into correct data slot
      TLX_BTREE_ASSERT(slot >= 0 && slot <= leaf->slotuse);

      std::copy_backward(leaf->slotdata + slot, leaf->slotdata + leaf->slotuse,
                         leaf->slotdata + leaf->slotuse + 1);

      leaf->slotdata[slot] = value;
      leaf->slotuse++;
      if (is_in_split_node) {
        split_size++;
      }

      if (splitnode && leaf != *splitnode && slot == leaf->slotuse - 1) {
        // special case: the node was split, and the insert is at the
        // last slot of the old node. then the splitkey must be updated.
        *splitkey = key;
      }

      return true;
    }
  }

  //! Split up a leaf node into two equally-filled sibling leaves. Returns the
  //! new nodes and it's insertion key in the two parameters.
  void split_leaf_node(LeafNode *leaf, key_type *out_newkey,
                       node **out_newleaf, unsigned &out_new_size) {
    TLX_BTREE_ASSERT(leaf->is_full());

    unsigned short mid = (leaf->slotuse >> 1);

    TLX_BTREE_PRINT("BTree::split_leaf_node on " << leaf);

    LeafNode *newleaf = allocate_leaf();

    newleaf->slotuse = leaf->slotuse - mid;

    std::copy(leaf->slotdata + mid, leaf->slotdata + leaf->slotuse,
              newleaf->slotdata);

    leaf->slotuse = mid;

    *out_newkey = leaf->key(leaf->slotuse - 1);
    *out_newleaf = newleaf;
    out_new_size = newleaf->slotuse;
  }

  //! Split up an inner node into two equally-filled sibling nodes. Returns
  //! the new nodes and it's insertion key in the two parameters. Requires the
  //! slot of the item will be inserted, so the nodes will be the same size
  //! after the insert.
  void split_inner_node(InnerNode *inner, key_type *out_newkey,
                        node **out_newinner, unsigned &out_new_size, unsigned int addslot) {
    TLX_BTREE_ASSERT(inner->is_full());

    unsigned short mid = (inner->slotuse >> 1);

    TLX_BTREE_PRINT("BTree::split_inner: mid " << mid << " addslot "
                                               << addslot);

    // if the split is uneven and the overflowing item will be put into the
    // larger node, then the smaller split node may underflow
    if (addslot <= mid && mid > inner->slotuse - (mid + 1))
      mid--;

    TLX_BTREE_PRINT("BTree::split_inner: mid " << mid << " addslot "
                                               << addslot);

    TLX_BTREE_PRINT("BTree::split_inner_node on "
                        << inner << " into two nodes " << mid << " and "
                        << inner->slotuse - (mid + 1) << " sized");

    InnerNode *newinner = allocate_inner(inner->level);

    newinner->slotuse = inner->slotuse - (mid + 1);

    std::copy(inner->slotkey + mid + 1, inner->slotkey + inner->slotuse,
              newinner->slotkey);
    std::copy(inner->child_info + mid + 1, inner->child_info + inner->slotuse + 1,
              newinner->child_info);

    for (int i = 0; i <= newinner->slotuse; i++) {
      newinner->size += newinner->child_info[i].second;
    }
    inner->size -= newinner->size;
    inner->slotuse = mid;

    *out_newkey = inner->key(mid);
    *out_newinner = newinner;
    out_new_size = newinner->size;
  }

  //! \}

 public:
  //! \name Bulk Loader - Construct Tree from Sorted Sequence
  //! \{

  //! \}

 private:
  //! \name Support Class Encapsulating Deletion Results
  //! \{

  //! Result flags of recursive deletion.
 public:
  //! \name Public Erase Functions
  //! \{

  //! Erases all the key/data pairs associated with the given key. This is
  //! implemented using erase_one().
  size_type erase(const key_type &key) {
    size_type c = 0;

    while (erase_one(key)) {
      ++c;
      if (!allow_duplicates)
        break;
    }

    return c;
  }


#ifdef BTREE_TODO
  //! Erase all key/data pairs in the range [first,last). This function is
  //! currently not implemented by the B+ Tree.
  void erase(iterator /* first */, iterator /* last */) { abort(); }
#endif

  //! \}


  //! \}
};

//! \}
//! \}

} // namespace tlx

#endif // !TLX_CONTAINER_BTREE_HEADER

/******************************************************************************/
