//---------------------------------------------------------------------------
// NAME: Joey Macauley
// FILE: btreemap.h
// DATE: Spring 2022
// DESC: Map implementation using a 2-3-4 B-Tree
// NOTE: ERASE DOES NOT WORK TO SCALE... unfortunately (there is a bug in case 3b)
//---------------------------------------------------------------------------

#ifndef BTreeMAP_H
#define BTreeMAP_H

#include "map.h"
#include "arrayseq.h"

template <typename K, typename V>
class BTreeMap : public Map<K, V>
{
public:
  // default constructor
  BTreeMap();

  // copy constructor
  BTreeMap(const BTreeMap &rhs);

  // move constructor
  BTreeMap(BTreeMap &&rhs);

  // copy assignment
  BTreeMap &operator=(const BTreeMap &rhs);

  // move assignment
  BTreeMap &operator=(BTreeMap &&rhs);

  // destructor
  ~BTreeMap();

  // Returns the number of key-value pairs in the map
  int size() const;

  // Tests if the map is empty
  bool empty() const;

  // Allows values associated with a key to be updated. Throws
  // out_of_range if the given key is not in the collection.
  V &operator[](const K &key);

  // Returns the value for a given key. Throws out_of_range if the
  // given key is not in the collection.
  const V &operator[](const K &key) const;

  // Extends the collection by adding the given key-value pair.
  // Expects key to not exist in map prior to insertion.
  void insert(const K &key, const V &value);

  // Shrinks the collection by removing the key-value pair with the
  // given key. Does not modify the collection if the collection does
  // not contain the key. Throws out_of_range if the given key is not
  // in the collection.
  void erase(const K &key);

  // Returns true if the key is in the collection, and false otherwise.
  bool contains(const K &key) const;

  // Returns the keys k in the collection such that k1 <= k <= k2
  ArraySeq<K> find_keys(const K &k1, const K &k2) const;

  // Returns the keys in the collection in ascending sorted order
  ArraySeq<K> sorted_keys() const;

  // Gives the key (as an ouptput parameter) immediately after the
  // given key according to ascending sort order. Returns true if a
  // successor key exists, and false otherwise.
  bool next_key(const K &key, K &next_key) const;

  // Gives the key (as an ouptput parameter) immediately before the
  // given key according to ascending sort order. Returns true if a
  // predecessor key exists, and false otherwise.
  bool prev_key(const K &key, K &next_key) const;

  // Removes all key-value pairs from the map.
  void clear();

  // Returns the height of the binary search tree
  int height() const;

  // for debugging the tree
  void print() const
  {
    print("  ", root, height());
  }

private:
  // node for 2-3-4 tree
  struct Node
  {
    ArraySeq<std::pair<K, V>> keyvals;
    ArraySeq<Node *> children;
    // helper functions
    bool full() const { return keyvals.size() == 3; }
    bool leaf() const { return children.empty(); }
    K key(int i) const { return keyvals[i].first; }
    V &val(int i) { return keyvals[i].second; }
    Node *child(int i) const { return children[i]; }
  };

  // number of key-value pairs in map
  int count = 0;

  // root node
  Node *root = nullptr;

  // print helper function
  void print(std::string indent, Node *st_root, int levels) const;

  // clean up the tree memory
  void clear(Node *st_root);

  // helper function for copy assignment
  Node *copy(const Node *rhs_st_root) const;

  // split the parent's i-th child
  void split(Node *parent, int i);

  // erase helpers
  void erase(Node *st_root, const K &key);
  void remove_internal(Node *st_root, int key_idx);
  void rebalance(Node *st_root, int key_idx, int &child_idx);

  // find_keys helper
  void find_keys(const K &k1, const K &k2, const Node *st_root,
                 ArraySeq<K> &keys) const;

  // sorted_keys helper
  void sorted_keys(const Node *st_root, ArraySeq<K> &keys) const;

  // height helper
  int height(const Node *st_root) const;
};

template <typename K, typename V>
void BTreeMap<K, V>::print(std::string indent, Node *st_root, int levels) const
{
  if (levels == 0)
    return;
  if (!st_root)
    return;
  std::cout << indent << "(";
  for (int i = 0; i < 3; ++i)
  {
    if (i != 0)
      std::cout << ",";
    if (st_root->keyvals.size() > i)
      std::cout << st_root->key(i);
    else
      std::cout << "-";
  }
  std::cout << ")" << std::endl;
  if (levels > 1)
  {
    for (int i = 0; i < st_root->children.size(); ++i)
      print(indent + " ", st_root->child(i), levels - 1);
  }
}

// default constructor
template <typename K, typename V>
BTreeMap<K, V>::BTreeMap()
{
}

// copy constructor
template <typename K, typename V>
BTreeMap<K, V>::BTreeMap(const BTreeMap &rhs)
{
  *this = rhs;
}

// move constructor
template <typename K, typename V>
BTreeMap<K, V>::BTreeMap(BTreeMap &&rhs)
{
  *this = std::move(rhs);
}

// copy assignment
template <typename K, typename V>
BTreeMap<K, V> &BTreeMap<K, V>::operator=(const BTreeMap &rhs)
{
  if (this != &rhs)
  {
    clear();
    root = copy(rhs.root);
    count = rhs.count;
  }
  return *this;
}

// move assignment
template <typename K, typename V>
BTreeMap<K, V> &BTreeMap<K, V>::operator=(BTreeMap &&rhs)
{
  if (this != &rhs)
  {
    clear();
    root = rhs.root;
    count = rhs.count;

    rhs.root = nullptr;
    rhs.count = 0;
  }
  return *this;
}

// destructor
template <typename K, typename V>
BTreeMap<K, V>::~BTreeMap()
{
  clear();
}

// Returns the number of key-value pairs in the map
template <typename K, typename V>
int BTreeMap<K, V>::size() const
{
  return count;
}

// Tests if the map is empty
template <typename K, typename V>
bool BTreeMap<K, V>::empty() const
{
  if (root == nullptr)
  {
    return true;
  }
  return false;
}

// Allows values associated with a key to be updated. Throws
// out_of_range if the given key is not in the collection.
template <typename K, typename V>
V &BTreeMap<K, V>::operator[](const K &key)
{
  Node *traverse = root;
  Node *searchNext = nullptr;
  int m = 0;
  while (traverse != nullptr)
  {
    // m == the # of keys in node
    m = traverse->keyvals.size();

    // search keys from 0 to m-1
    for (int i = 0; i < m; ++i)
    {
      if (key < traverse->key(i))
      {
        if (traverse->leaf())
        {
          searchNext = nullptr;
        }
        else
        {
          searchNext = traverse->child(i);
          break;
        }
      }
      else if (key == traverse->key(i))
      {
        return traverse->val(i);
      }
    }

    // search next child if necessary
    if (key > traverse->key(m - 1))
    {
      if (traverse->leaf())
      {
        searchNext = nullptr;
      }
      else
      {
        searchNext = traverse->child(m);
      }
    }
    traverse = searchNext;
  }

  throw std::out_of_range("Key is not in the collection");
}

// Returns the value for a given key. Throws out_of_range if the
// given key is not in the collection.
template <typename K, typename V>
const V &BTreeMap<K, V>::operator[](const K &key) const
{
  Node *traverse = root;
  Node *searchNext = nullptr;
  int m = 0;
  while (traverse != nullptr)
  {
    // m == the # of keys in node
    m = traverse->keyvals.size();

    // search keys from 0 to m-1
    for (int i = 0; i < m; ++i)
    {
      if (key < traverse->key(i))
      {
        if (traverse->leaf())
        {
          searchNext = nullptr;
        }
        else
        {
          searchNext = traverse->child(i);
          break;
        }
      }
      else if (key == traverse->key(i))
      {
        return traverse->val(i);
      }
    }

    // search next child if necessary
    if (key > traverse->key(m - 1))
    {
      if (traverse->leaf())
      {
        searchNext = nullptr;
      }
      else
      {
        searchNext = traverse->child(m);
      }
    }
    traverse = searchNext;
  }

  throw std::out_of_range("Key is not in the collection");
}

// Extends the collection by adding the given key-value pair.
// Expects key to not exist in map prior to insertion.
template <typename K, typename V>
void BTreeMap<K, V>::insert(const K &key, const V &value)
{
  // empty tree
  if (!root)
  {
    std::pair<K, V> p{key, value};
    root = new Node;
    root->keyvals.insert(p, 0);
    count++;
    return;
  }

  // root is full
  if (root->full())
  {
    Node *left = root;
    root = new Node;
    root->children.insert(left, 0);
    split(root, 0);
  }

  Node *prev = nullptr;
  Node *curr = root;
  int index = -1;

  while (curr != nullptr)
  {
    // split node
    if (curr->full())
    {
      curr = prev;
      split(prev, index);
    }

    prev = curr;
    for (int j = 0; j < prev->keyvals.size(); ++j)
    {
      // if key is less than current key in current node
      if (key < prev->key(j))
      {
        index = j;
        if (prev->leaf())
        {
          curr = nullptr;
        }
        else
        {
          curr = prev->child(j);
        }
        break;
      }
      // if key is greater than current key in current node
      else if (key > prev->key(j))
      {
        index = j + 1;
        if (prev->leaf())
        {
          curr = nullptr;
        }
        else
        {
          curr = prev->child(j + 1);
        }
      }
    }
  }
  std::pair<K, V> p{key, value};
  prev->keyvals.insert(p, index);
  count++;
  return;
}

// Shrinks the collection by removing the key-value pair with the
// given key. Does not modify the collection if the collection does
// not contain the key. Throws out_of_range if the given key is not
// in the collection.
template <typename K, typename V>
void BTreeMap<K, V>::erase(const K &key)
{
  if (empty())
  {
    throw std::out_of_range("Key is not in the collection");
  }
  else if (!contains(key))
  {
    throw std::out_of_range("Key is not in the collection");
  }
  erase(root, key);
  if (root->keyvals.empty())
  {
    Node *left_child = nullptr;
    // check if one child left
    if (root->children.size() > 0)
      left_child = root->child(0);
    delete root;
    root = left_child;
  }
  --count;
}

// Returns true if the key is in the collection, and false otherwise.
template <typename K, typename V>
bool BTreeMap<K, V>::contains(const K &key) const
{
  Node *traverse = root;
  Node *searchNext = nullptr;
  int m = 0;
  while (traverse != nullptr)
  {
    // m == the # of keys in node
    m = traverse->keyvals.size();
    searchNext = nullptr;
    // search keys from 0 to m-1
    for (int i = 0; i < m; ++i)
    {
      if (key < traverse->key(i))
      {
        if (traverse->leaf())
        {
          return false;
        }
        else
        {
          searchNext = traverse->child(i);
          break;
        }
      }
      else if (key == traverse->key(i))
      {
        return true;
      }
    }

    // search next child if necessary
    if (key > traverse->key(m - 1))
    {
      if (traverse->leaf())
      {
        return false;
      }
      else
      {
        searchNext = traverse->child(m);
      }
    }

    traverse = searchNext;
  }
  return false;
}

// Returns the keys k in the collection such that k1 <= k <= k2
template <typename K, typename V>
ArraySeq<K> BTreeMap<K, V>::find_keys(const K &k1, const K &k2) const
{
  ArraySeq<K> keys;
  if (!empty())
  {
    find_keys(k1, k2, root, keys);
  }
  // keys.sort();
  return keys;
}

// Returns the keys in the collection in ascending sorted order
template <typename K, typename V>
ArraySeq<K> BTreeMap<K, V>::sorted_keys() const
{
  ArraySeq<K> keys;
  if (!empty())
  {
    sorted_keys(root, keys);
  }
  // keys.sort();
  return keys;
}

// Gives the key (as an ouptput parameter) immediately after the
// given key according to ascending sort order. Returns true if a
// successor key exists, and false otherwise.
template <typename K, typename V>
bool BTreeMap<K, V>::next_key(const K &key, K &next_key) const
{
  Node *traverse = root;
  Node *searchNext = nullptr;
  K hold;
  int m = 0;
  int nextExists = 0;

  if (empty())
  {
    return false;
  }

  while (traverse != nullptr)
  {
    // m == the # of keys in node
    m = traverse->keyvals.size();
    searchNext = nullptr;
    // search keys from 0 to m-1
    for (int i = 0; i < m; ++i)
    {
      if (key < traverse->key(i))
      {
        nextExists = 1;
        hold = traverse->key(i);
        if (traverse->leaf())
        {
          searchNext = nullptr;
        }
        else
        {
          searchNext = traverse->child(i);
        }
        break;
      }
      else if (key == traverse->key(i))
      {
        if (traverse->leaf())
        {
          searchNext = nullptr;
        }
        else
        {
          searchNext = traverse->child(i + 1);
        }
      }
    }
    // search next child if necessary
    if (key > traverse->key(m - 1))
    {
      if (traverse->leaf())
      {
        searchNext == nullptr;
      }
      else if (traverse->children.size() == m + 1)
      {
        searchNext = traverse->child(m);
      }
    }
    traverse = searchNext;
  }

  if (nextExists == 1)
  {
    next_key = hold;
    return true;
  }
  return false;
}

// Gives the key (as an ouptput parameter) immediately before the
// given key according to ascending sort order. Returns true if a
// predecessor key exists, and false otherwise.
template <typename K, typename V>
bool BTreeMap<K, V>::prev_key(const K &key, K &next_key) const
{
  Node *traverse = root;
  Node *searchNext = nullptr;
  K hold;
  int m = 0;
  int prevExists = 0;

  if (empty())
  {
    return false;
  }

  while (traverse != nullptr)
  {
    // m == the # of keys in node
    m = traverse->keyvals.size();
    searchNext = nullptr;
    // search keys from 0 to m-1
    for (int i = 0; i < m; ++i)
    {
      if (key < traverse->key(i))
      {
        if (traverse->leaf())
        {
          searchNext = nullptr;
        }
        else
        {
          searchNext = traverse->child(i);
        }
        break;
      }
      else if (key == traverse->key(i))
      {
        if (traverse->leaf())
        {
          searchNext = nullptr;
        }
        else
        {
          searchNext = traverse->child(i);
        }
        break;
      }
      else if (key > traverse->key(i))
      {
        prevExists = 1;
        hold = traverse->key(i);
        if (i == m - 1 && !traverse->leaf())
        {
          searchNext = traverse->child(i + 1);
        }
      }
    }
    traverse = searchNext;
  }
  if (prevExists == 1)
  {
    next_key = hold;
    return true;
  }
  return false;
}

// Removes all key-value pairs from the map.
template <typename K, typename V>
void BTreeMap<K, V>::clear()
{
  clear(root);
}

// Returns the height of the binary search tree
template <typename K, typename V>
int BTreeMap<K, V>::height() const
{
  if (empty())
  {
    return 0;
  }
  else
  {
    return height(root);
  }
}

// clean up the tree memory
template <typename K, typename V>
void BTreeMap<K, V>::clear(Node *st_root)
{
  if (st_root == root)
  {
    count = 0;
  }
  if (st_root != nullptr)
  {
    st_root->keyvals.clear();
    for (int i = 0; i < st_root->children.size(); ++i)
    {
      clear(st_root->child(i));
    }
    st_root->children.clear();
  }
  delete st_root;
  return;
}

// helper function for copy assignment
template <typename K, typename V>
typename BTreeMap<K, V>::Node *BTreeMap<K, V>::copy(const Node *rhs_st_root) const
{
  Node *root = nullptr;
  if (rhs_st_root != nullptr)
  {
    root = new Node;
    // use array copy assignment
    root->keyvals = rhs_st_root->keyvals;

    // traverse each child node
    for (int i = 0; i < rhs_st_root->children.size(); ++i)
    {
      root->children.insert(copy(rhs_st_root->child(i)), i);
    }
  }
  return root;
}

// split the parent's i-th child
template <typename K, typename V>
void BTreeMap<K, V>::split(Node *parent, int i)
{
  // split node
  Node *split = parent->child(i);
  std::pair<K, V> second{split->key(1), split->val(1)};

  // build right "NEW" node (values and children)
  Node *right = new Node;
  std::pair<K, V> third{split->key(2), split->val(2)};
  right->keyvals.insert(third, 0);

  if (!split->leaf())
  {
    right->children.insert(split->child(2), 0);
    right->children.insert(split->child(3), 1);
    split->children.erase(3);
    split->children.erase(2);
  }

  // insert middle element into parent node / update children
  parent->children.insert(right, i + 1);
  parent->keyvals.insert(second, i);

  // clean up left "OLD/SPLIT" node
  split->keyvals.erase(2);
  split->keyvals.erase(1);
}

// erase helpers
template <typename K, typename V>
void BTreeMap<K, V>::erase(Node *st_root, const K &key)
{
  int child_idx = -1, m = 0;

  while (st_root)
  {
    // case 1: leaf case
    if (st_root->leaf())
    {
      for (int i = 0; i < st_root->keyvals.size(); ++i)
      {
        if (key == st_root->key(i))
        {
          st_root->keyvals.erase(i);
          return;
        }
      }
      throw std ::out_of_range(" BTreeMap <K,V >:: erase ( const K&)");
    }
    else
    {
      m = st_root->keyvals.size();

      for (int i = 0; i < st_root->keyvals.size(); i++)
      {

        if (key > st_root->key(i))
        {

          continue;
        }
        if (key == st_root->key(i))
        {
          // case 2
          if (!st_root->leaf())
          {

            remove_internal(st_root, i);
          }
          // case 1
          else
          {

            st_root->keyvals.erase(i);
          }
          return;
        }
        // case 3, key in left child
        else if (key < st_root->key(i))
        {

          if (st_root->child(i)->keyvals.size() == 1)
          {

            rebalance(st_root, i, i);
          }
          erase(st_root->child(i), key);
          return;
        }
      }
      // case 3, key in right child
      if (key > st_root->key(m - 1))
      {

        if (st_root->child(m)->keyvals.size() == 1)
        {
          rebalance(st_root, m - 1, m);
        }
        erase(st_root->child(m), key);
        return;
      }
    }
  }
  throw std ::out_of_range(" BTreeMap <K,V >:: erase ( const K&)");
}

template <typename K, typename V>
void BTreeMap<K, V>::remove_internal(Node *st_root, int key_idx)
{
  Node *traverse = nullptr;
  int nodeSize = 0;
  K new_key, this_key;
  V new_val;
  // case 2a: left has 2 keys
  if (st_root->child(key_idx)->keyvals.size() > 1)
  {
    traverse = st_root->child(key_idx);
    while (traverse)
    {
      // Find Predecessor
      nodeSize = traverse->keyvals.size();
      if (traverse->leaf())
      {
        new_key = traverse->key(nodeSize - 1);
        new_val = traverse->val(nodeSize - 1);

        // Replace key to erase with predecessor
        std::pair<K, V> p{new_key, new_val};
        st_root->keyvals.erase(key_idx);
        st_root->keyvals.insert(p, key_idx);

        // erase P starting from left child
        erase(st_root->child(key_idx), traverse->key(nodeSize - 1));

        return;
      }
      else
      {
        traverse = traverse->child(nodeSize);
      }
    }
  }
  // case 2b: right has 2 keys
  else if (st_root->child(key_idx + 1)->keyvals.size() > 1)
  {
    traverse = st_root->child(key_idx + 1);
    while (traverse)
    {
      // Find Successor
      if (traverse->leaf())
      {
        new_key = traverse->key(0);
        new_val = traverse->val(0);

        // Replace key to erase with Successor
        std::pair<K, V> p{new_key, new_val};
        st_root->keyvals.erase(key_idx);
        st_root->keyvals.insert(p, key_idx);

        // erase S starting from right child
        erase(st_root->child(key_idx + 1), traverse->key(0));

        return;
      }
      else
      {
        traverse = traverse->child(0);
      }
    }
  }
  // case 2c: left and right have 1 key... MERGE
  else
  {
    // Insert node to delete into left child
    this_key = st_root->key(key_idx);
    new_val = st_root->val(key_idx);
    std::pair<K, V> p{this_key, new_val};
    st_root->child(key_idx)->keyvals.insert(p, 1);

    // Erase node
    st_root->keyvals.erase(key_idx);

    // Insert right child key and children into left child
    new_key = st_root->child(key_idx + 1)->key(0);
    new_val = st_root->child(key_idx + 1)->val(0);
    std::pair<K, V> p1{new_key, new_val};
    st_root->child(key_idx)->keyvals.insert(p1, 2);

    // traverse each child node if necessary
    if (!st_root->child(key_idx)->leaf())
    {
      for (int i = 0; i < 2; ++i)
      {
        // copy right child's children into left child's children
        st_root->child(key_idx)->children.insert(copy(st_root->child(key_idx + 1)->child(i)), i + 2);
      }
    }

    // delete right child node
    clear(st_root->child(key_idx + 1));
    st_root->children.erase(key_idx + 1);

    // Erase "node to delete" in merged child
    erase(st_root->child(key_idx), this_key);
  }
}

template <typename K, typename V>
void BTreeMap<K, V>::rebalance(Node *st_root, int key_idx, int &child_idx)
{
  int m = st_root->keyvals.size(), n = 0;
  K n_key, p_key;
  V n_val, p_val;

  // Check Case 3a: Left or Right have 2 keys
  // OTHERWISE: Case 3b: Left and Right only have 1 key

  // Remove Key from Parent
  p_key = st_root->key(key_idx);
  p_val = st_root->key(key_idx);
  std::pair<K, V> p{p_key, p_val};
  st_root->keyvals.erase(key_idx);

  // c_i is far right child
  if (child_idx == m)
  {
    // check if "only neighbor" has 2 keys
    if (st_root->child(m - 1)->keyvals.size() > 1)
    {
      n = st_root->child(m - 1)->keyvals.size() - 1;

      n_key = st_root->child(m - 1)->key(n);
      n_val = st_root->child(m - 1)->val(n);
      std::pair<K, V> ne{n_key, n_val};
      st_root->child(m - 1)->keyvals.erase(n);

      st_root->keyvals.insert(ne, key_idx);
      st_root->child(m)->keyvals.insert(p, 1);

      if (!st_root->child(m - 1)->leaf())
      {
        st_root->child(m)->children.insert(st_root->child(m - 1)->child(2), 2);
        st_root->child(m - 1)->children.erase(2);
      }
      return;
    }
    // case 3b --------------------------------------------------------------------------
    else
    {
      // merge with left neighbor and key from parent
      st_root->child(m)->keyvals.insert(p, 0);

      n_key = st_root->child(m - 1)->key(0);
      n_val = st_root->child(m - 1)->val(0);
      std::pair<K, V> ne{n_key, n_val};
      st_root->child(m)->keyvals.insert(ne, 0);

      // copy children
      if (!st_root->child(m - 1)->leaf())
      {
        st_root->child(m)->children.insert(st_root->child(m - 1)->child(0), 0);
        st_root->child(m)->children.insert(st_root->child(m - 1)->child(1), 1);
      }

      // delete right node
      delete st_root->child(m - 1);
      st_root->children.erase(m - 1);
      child_idx = m - 1;
    }
  }

  // c_i is far left child
  else if (child_idx == 0)
  {
    // check if "only neighbor" has 2 keys
    if (st_root->child(1)->keyvals.size() > 1)
    {
      n_key = st_root->child(1)->key(0);
      n_val = st_root->child(1)->val(0);
      std::pair<K, V> n{n_key, n_val};
      st_root->child(1)->keyvals.erase(0);

      st_root->keyvals.insert(n, key_idx);
      st_root->child(0)->keyvals.insert(p, 1);

      // take care of children
      if (!st_root->child(1)->leaf())
      {
        st_root->child(0)->children.insert(st_root->child(1)->child(0), 2);
        st_root->child(1)->children.erase(0);
      }

      return;
    }
    // case 3b ------------------------------------------------------------------------
    else
    {
      // merge with right neighbor and key from parent
      st_root->child(0)->keyvals.insert(p, 1);

      n_key = st_root->child(1)->key(0);
      n_val = st_root->child(1)->val(0);
      std::pair<K, V> ne{n_key, n_val};
      st_root->child(0)->keyvals.insert(ne, 2);

      // copy children
      if (!st_root->child(1)->leaf())
      {
        st_root->child(0)->children.insert(st_root->child(1)->child(0), 2);
        st_root->child(0)->children.insert(st_root->child(1)->child(1), 3);
      }

      // delete right node
      delete st_root->child(1);
      st_root->children.erase(1);
      child_idx = 0;
    }
  }

  // c_i is a middle child (2 neighbors)
  else
  {
    // right neighbor has 2
    if (st_root->child(child_idx + 1)->keyvals.size() > 1)
    {
      n_key = st_root->child(child_idx + 1)->key(0);
      n_val = st_root->child(child_idx + 1)->val(0);
      std::pair<K, V> n{n_key, n_val};
      st_root->child(child_idx + 1)->keyvals.erase(0);

      st_root->keyvals.insert(n, key_idx);

      st_root->child(0)->keyvals.insert(p, 1);
      return;
    }
    // left neighbor has 2
    else if (st_root->child(child_idx - 1)->keyvals.size() > 1)
    {
      n = st_root->child(child_idx - 1)->keyvals.size() - 1;

      n_key = st_root->child(child_idx - 1)->key(n);
      n_val = st_root->child(child_idx - 1)->val(n);
      std::pair<K, V> ne{n_key, n_val};
      st_root->child(child_idx - 1)->keyvals.erase(n);

      st_root->keyvals.insert(ne, key_idx);

      st_root->child(m)->keyvals.insert(p, 1);

      return;
    }
    // case 3b --------------------------------------------------------------------
    else
    {
      // merge with any neighbor (chose right) and key from parent
      st_root->child(key_idx)->keyvals.insert(p, 1);
      n_key = st_root->child(key_idx + 1)->key(0);
      n_val = st_root->child(key_idx + 1)->val(0);
      std::pair<K, V> ne{n_key, n_val};
      st_root->child(key_idx + 1)->keyvals.erase(0);
      st_root->child(key_idx)->keyvals.insert(ne, 2);

      // copy children
      if (!st_root->child(key_idx + 1)->leaf())
      {
        st_root->child(key_idx)->children.insert(st_root->child(key_idx + 1)->child(0), 2);
        st_root->child(key_idx)->children.insert(st_root->child(key_idx + 1)->child(1), 3);
      }
      // delete right node
      delete st_root->child(key_idx + 1);
      // clear(st_root->child(key_idx + 1));
      st_root->children.erase(key_idx + 1);
    }
  }
  return;
}

// find_keys helper
template <typename K, typename V>
void BTreeMap<K, V>::find_keys(const K &k1, const K &k2, const Node *st_root, ArraySeq<K> &keys) const
{
  K key;
  Node *temp = nullptr;
  int m = st_root->keyvals.size();

  if (st_root == nullptr)
  {
    return;
  }

  if (!st_root->leaf())
  {
    for (int i = 0; i < m; ++i)
    {
      temp = st_root->child(i);

      find_keys(k1, k2, temp, keys);

      key = st_root->key(i);

      if (key >= k1 and key <= k2) // DOESN'T ENTER ?????
      {

        keys.insert(key, keys.size());
      }
    }

    temp = st_root->child(m);

    find_keys(k1, k2, temp, keys);
  }

  else
  {
    for (int j = 0; j < m; ++j)
    {
      key = st_root->key(j);
      if (key >= k1 and key <= k2)
      {
        keys.insert(key, keys.size());
      }
    }
  }
}

// sorted_keys helper
template <typename K, typename V>
void BTreeMap<K, V>::sorted_keys(const Node *st_root, ArraySeq<K> &keys) const
{
  K key;
  Node *temp = nullptr;

  if (st_root == nullptr)
  {
    return;
  }

  if (!st_root->leaf())
  {
    for (int i = 0; i < st_root->keyvals.size(); ++i)
    {
      temp = st_root->child(i);
      sorted_keys(temp, keys);

      key = st_root->key(i);
      keys.insert(key, keys.size());
    }
    temp = st_root->child(st_root->keyvals.size());
    sorted_keys(temp, keys);
  }

  else
  {
    for (int i = 0; i < st_root->keyvals.size(); ++i)
    {
      keys.insert(st_root->key(i), keys.size());
    }
  }

  return;
}

// height helper
template <typename K, typename V>
int BTreeMap<K, V>::height(const Node *st_root) const
{
  int m = 0, l_chld_ht = 0, r_chld_ht = 0, root_height = 0;

  if (st_root == nullptr)
  {
    return 0;
  }

  m = st_root->keyvals.size();
  for (int i = 0; i < m; ++i)
  {
    // Leaf -> Height = 1, no children.
    if (st_root->leaf())
    {
      return 1;
    }
    // First key, traverse both right and left child
    else if (i == 0)
    {
      l_chld_ht = height(st_root->child(i));
      r_chld_ht = height(st_root->child(i + 1));
      // Take the largest of the left and right child heights
      if (l_chld_ht > r_chld_ht)
      {
        root_height = l_chld_ht + 1;
      }
      else
      {
        root_height = r_chld_ht + 1;
      }
    }
    // Other keys, traverse just right child
    else
    {
      r_chld_ht = height(st_root->child(i + 1));
      // Since only right child traversed, add right child height
      if (r_chld_ht > root_height)
      {
        root_height = r_chld_ht + 1;
      }
    }
    return root_height;
  }
}

#endif
