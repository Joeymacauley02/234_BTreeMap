//---------------------------------------------------------------------------
// NAME: Joey Macauley
// FILE: arrayseq.h
// DATE: Spring 2022
// DESC: Implementation of an ArraySeq and the corresponding sort functions (merge and quick)
//----------------------------------------------------------------------

#ifndef ARRAYLIST_H
#define ARRAYLIST_H

#include <stdexcept>
#include <ostream>
#include "sequence.h"

template <typename T>
class ArraySeq : public Sequence<T>
{
public:
  // Default constructor
  ArraySeq();

  // Copy constructor
  ArraySeq(const ArraySeq &rhs);

  // Move constructor
  ArraySeq(ArraySeq &&rhs);

  // Copy assignment operator
  ArraySeq &operator=(const ArraySeq &rhs);

  // Move assignment operator
  ArraySeq &operator=(ArraySeq &&rhs);

  // Destructor
  ~ArraySeq();

  // Returns the number of elements in the sequence
  int size() const;

  // Tests if the sequence is empty
  bool empty() const;

  // Removes all of the elements from the sequence
  void clear();

  // Returns a reference to the element at the index in the
  // sequence. Throws out_of_range if index is invalid (less than 0 or
  // greater than or equal to size()).
  T &operator[](int index);

  // Returns a constant address to the element at the index in the
  // sequence. Throws out_of_range if index is invalid (less than 0 or
  // greater than or equal to size()).
  const T &operator[](int index) const;

  // Extends the sequence by inserting the element at the given index.
  // Throws out_of_range if the index is invalid (less than 0 or
  // greater than size()).
  void insert(const T &elem, int index);

  // Shrinks the sequence by removing the element at the index in the
  // sequence. Throws out_of_range if index is invalid.
  void erase(int index);

  // Returns true if the element is in the sequence, and false
  // otherwise.
  bool contains(const T &elem) const;

  // Sorts the elements in the sequence in place using less than equal
  // (<=) operator. Uses quick sort with a randomly selected pivot
  // index.
  void sort();

  // Sorts the sequence in place using the merge sort algorithm.
  void merge_sort();

  // Sorts the sequence in place using the quick sort algorithm. Uses
  // first element for pivot values.
  void quick_sort();

  // Sorts the sequence in place using the quick sort algorithm. Uses
  // randomly selected indexes for pivot values.
  void quick_sort_random();

private:
  // resizable array
  T *array = nullptr;

  // size of list
  int count = 0;

  // max capacity of the array
  int capacity = 0;

  // helper to double the capacity of the array
  void resize();

  // sort function helpers
  void merge_sort(int start, int end);
  void quick_sort(int start, int end);
  void quick_sort_random(int start, int end);

  // random seed for quick sort
  int seed = 22;
};

template <typename T>
std::ostream &operator<<(std::ostream &stream, const ArraySeq<T> &seq)
{
  int n = seq.size();
  for (int i = 0; i < n - 1; ++i)
    stream << seq[i] << ", ";
  if (n > 0)
    stream << seq[n - 1];
  return stream;
}

template <typename T>
ArraySeq<T>::ArraySeq()
{
}

// (1) TODO: Add your arrayseq function implementations from HW-3 here.

template <typename T>
ArraySeq<T>::ArraySeq(const ArraySeq &rhs)
{
  *this = rhs;
}

template <typename T>
ArraySeq<T>::ArraySeq(ArraySeq &&rhs)
{
  *this = std::move(rhs);
}

template <typename T>
ArraySeq<T> &ArraySeq<T>::operator=(const ArraySeq &rhs)
{
  if (this != &rhs)
  {
    clear();
    array = new T[rhs.capacity];
    count = rhs.count;
    capacity = rhs.capacity;

    for (int i = 0; i < count; i++)
    {
      array[i] = rhs[i];
    }
  }
  return *this;
}

template <typename T>
ArraySeq<T> &ArraySeq<T>::operator=(ArraySeq &&rhs)
{
  if (this != &rhs)
  {
    clear();
    array = rhs.array;
    count = rhs.count;
    capacity = rhs.capacity;

    rhs.array = nullptr;
    rhs.count = 0;
    rhs.capacity = 0;
  }
  return *this;
}

template <typename T>
ArraySeq<T>::~ArraySeq()
{
  clear();
}

template <typename T>
int ArraySeq<T>::size() const
{
  return count;
}

template <typename T>
bool ArraySeq<T>::empty() const
{
  if (count == 0)
  {
    return true;
  }
  return false;
}

template <typename T>
void ArraySeq<T>::clear()
{
  count = 0;
  capacity = 0;
  delete[] array;
  array = nullptr;
}

template <typename T>
T &ArraySeq<T>::operator[](int index)
{
  if (index < 0 or index >= size())
  {
    throw std::out_of_range("Invalid Index");
  }
  else
  {
    return array[index];
  }
}

template <typename T>
const T &ArraySeq<T>::operator[](int index) const
{
  if (index < 0 or index >= size())
  {
    throw std::out_of_range("Invalid Index");
  }
  else
  {
    return array[index];
  }
}

template <typename T>
void ArraySeq<T>::insert(const T &elem, int index)
{
  if (index < 0 or index > size())
  {
    throw std::out_of_range("Invalid Index");
  }
  else
  {
    if (size() == capacity)
    {
      resize();
    }
    for (int i = size(); i > index; --i)
    {
      array[i] = array[i - 1];
    }
    array[index] = elem;
    count++;
  }
}

template <typename T>
void ArraySeq<T>::erase(int index)
{
  if (index < 0 or index >= size())
  {
    throw std::out_of_range("Invalid Index");
  }
  else
  {
    for (int i = index; i < count - 1; ++i)
    {
      array[i] = array[i + 1];
    }
    count--;
  }
}

template <typename T>
bool ArraySeq<T>::contains(const T &elem) const
{
  for (int i = 0; i < count; ++i)
  {
    if (array[i] == elem)
    {
      return true;
    }
  }
  return false;
}

template <typename T>
void ArraySeq<T>::resize()
{
  if (capacity == 0)
  {
    capacity = capacity + 1;
  }
  else
  {
    capacity = capacity * 2;
  }

  T *new_array = nullptr;
  new_array = new T[capacity];

  for (int i = 0; i < count; ++i)
  {
    new_array[i] = array[i];
  }
  delete[] array;
  array = new_array;
}

// (2) TODO: Implement the following sorting functions for your array
// sequence. Note that:
//
//   (a) You are NOT ALLOWED to add any additional helper functions
//       for sorting.
//   (b) You MUST follow the approaches outlined in the lecture notes.
//   (c) To obtain a random integer, use the C++ standard function
//       std::rand(). This function returns a random number (up to a
//       fixed max value). You will need to convert the value returned
//       to a valid index.

template <typename T>
void ArraySeq<T>::sort()
{
  quick_sort_random();
}

template <typename T>
void ArraySeq<T>::merge_sort()
{
  merge_sort(0, size() - 1);
}

template <typename T>
void ArraySeq<T>::quick_sort()
{
  quick_sort(0, size() - 1);
}

template <typename T>
void ArraySeq<T>::quick_sort_random()
{
  // seed the pseudo-random number generator
  std::srand(seed);
  quick_sort_random(0, size() - 1);
}

template <typename T>
void ArraySeq<T>::merge_sort(int start, int end)
{
  int mid = 0, first = 0, second = 0, i = 0;
  if (start < end)
  {
    mid = (start + end) / 2;
    merge_sort(start, mid);
    merge_sort(mid + 1, end);

    // Merge Function
    T *temp = new T[(end - start) + 1];
    first = start;
    second = mid + 1;
    i = 0;
    while (first <= mid and second <= end)
    {
      if (array[first] < array[second]) // '<=' ??
      {
        temp[i++] = array[first++];
      }
      else
      {
        temp[i++] = array[second++];
      }
    }
    while (first <= mid)
    {
      temp[i++] = array[first++];
    }
    while (second <= end)
    {
      temp[i++] = array[second++];
    }
    for (int j = 0; j <= (end - start); ++j)
    {
      array[start + j] = temp[j];
    }
    delete[] temp;
  }
}

template <typename T>
void ArraySeq<T>::quick_sort(int start, int end)
{
  int pivot_val = 0, end_p1 = 0;
  T temp;
  if (start < end)
  {
    pivot_val = array[start];
    end_p1 = start;
    for (int i = start + 1; i <= end; ++i)
    {
      if (array[i] < pivot_val)
      {
        end_p1 = end_p1 + 1;
        temp = array[i];
        array[i] = array[end_p1];
        array[end_p1] = temp;
      }
    }
    temp = array[start];
    array[start] = array[end_p1];
    array[end_p1] = temp;
    quick_sort(start, end_p1 - 1);
    quick_sort(end_p1 + 1, end);
  }
}

template <typename T>
void ArraySeq<T>::quick_sort_random(int start, int end)
{
  T pivot_val; 
  int end_p1 = 0, randIdx = 0;
  T temp;
  if (start < end)
  {
    randIdx = start + rand() % (end - start);
    temp = array[start];
    array[start] = array[randIdx];
    array[randIdx] = temp;
    pivot_val = array[start];
    end_p1 = start;
    for (int i = start + 1; i <= end; ++i)
    {
      if (array[i] < pivot_val)
      {
        end_p1 = end_p1 + 1;
        temp = array[i];
        array[i] = array[end_p1];
        array[end_p1] = temp;
      }
    }
    temp = array[start];
    array[start] = array[end_p1];
    array[end_p1] = temp;
    quick_sort_random(start, end_p1 - 1);
    quick_sort_random(end_p1 + 1, end);
  }
} 

#endif
