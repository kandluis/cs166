#include "FingerArray.h"
#include <algorithm> // For lower_bound, min
#include <numeric>   // For iota
using namespace std;

FingerArray::FingerArray(size_t nElems) : elems(nElems) {
  /* Fill the weights array with the values 0, 1, 2, ..., etc. */
  iota(elems.begin(), elems.end(), 0u);
  
  /* Put the finger in the middle of the array. */
  finger = nElems / 2;
}

bool FingerArray::contains(size_t key) const {
  /* If the finger happens to be right where the key is, we're done! */
  if (finger == key) return finger < elems.size();
  
  /* Search right. */
  else if (finger < key) {
    /* Exponential search outward. */
    size_t delta = 1;
    while (finger + delta < elems.size() && elems[finger + delta] < key) {
      delta *= 2;
    }
    
    /* Binary search over the appropriate range. */
    size_t end = min(elems.size(), finger + delta);
    finger = lower_bound(elems.begin() + finger, elems.begin() + end, key) - elems.begin();
    
    return finger < elems.size() && elems[finger] == key;
  }
  
  /* Search left. */
  else {
    /* Exponential search outward. */
    size_t delta = 1;
    while (delta < finger && elems[finger - delta] > key) {
      delta *= 2;
    }
    
    /* Binary search over the appropriate range. */
    size_t start = 0;
    if (delta < finger) start = finger - delta;
    
    finger = lower_bound(elems.begin() + start, elems.begin() + finger, key) - elems.begin();
    
    return elems[finger] == key;
  }
}
