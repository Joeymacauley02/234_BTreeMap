//---------------------------------------------------------------------------
// NAME: S. Bowers
// FILE: map.h
// DATE: Spring 2022
// DESC: An abstract class for representing generic key-value pair
//       collections (often refered to as maps, dictionaries, or
//       associative arrays). Maps consist of a set of unique keys,
//       where each key has an associated value. A key together with
//       its value is called a "key-value pair". A key should only be
//       added to a collection if the key isn't already in the
//       collection. If duplicates keys are added, the resulting
//       behavior of the underlying implementation may not return
//       expected results.
//---------------------------------------------------------------------------

#ifndef MAP_H
#define MAP_H

#include "arrayseq.h"


template<typename K, typename V>
class Map
{
public:

  // For concrete subclasses
  virtual ~Map() {}

  // Returns the number of key-value pairs in the map
  virtual int size() const = 0;

  // Tests if the map is empty
  virtual bool empty() const = 0;

  // Allows values associated with a key to be updated. Throws
  // out_of_range if the given key is not in the collection.
  virtual V& operator[](const K& key) = 0;

  // Returns the value for a given key. Throws out_of_range if the
  // given key is not in the collection. 
  virtual const V& operator[](const K& key) const = 0;

  // Extends the collection by adding the given key-value pair.
  // Expects key to not exist in map prior to insertion.
  virtual void insert(const K& key, const V& value) = 0;

  // Shrinks the collection by removing the key-value pair with the
  // given key. Does not modify the collection if the collection does
  // not contain the key. Throws out_of_range if the given key is not
  // in the collection.
  virtual void erase(const K& key) = 0;

  // Returns true if the key is in the collection, and false otherwise.
  virtual bool contains(const K& key) const = 0;

  // Returns the keys k in the collection such that k1 <= k <= k2
  virtual ArraySeq<K> find_keys(const K& k1, const K& k2) const = 0;

  // Returns the keys in the collection in ascending sorted order
  virtual ArraySeq<K> sorted_keys() const = 0;  

  // Gives the key (as an ouptput parameter) immediately after the
  // given key according to ascending sort order. Returns true if a
  // successor key exists, and false otherwise.
  virtual bool next_key(const K& key, K& next_key) const = 0; 

  // Gives the key (as an ouptput parameter) immediately before the
  // given key according to ascending sort order. Returns true if a
  // predecessor key exists, and false otherwise.
  virtual bool prev_key(const K& key, K& next_key) const = 0; 

  // Removes all key-value pairs from the map.
  virtual void clear() = 0;
  
};


#endif
