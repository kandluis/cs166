#include "SortedArray.h"
#include <algorithm>
#include <numeric>
using namespace std;

SortedArray::SortedArray(size_t nElems) : elems(nElems) {
  /* Fill the weights array with the values 0, 1, 2, ..., etc. */
  iota(elems.begin(), elems.end(), 0u);
}

bool SortedArray::contains(size_t key) const {
  return binary_search(elems.begin(), elems.end(), key);
}
