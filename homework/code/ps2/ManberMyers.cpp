#include "ManberMyers.h"
#include <tuple>
using namespace std;

namespace {
  /* Type representing a string formed from a pair of strings, along with
   * an initial index.
   */
  struct ManberString {
    tuple<size_t, size_t> entry;
    size_t index;
  };

  /* Sorts the given pairs using counting sort, according to the appropriate entry. */
  template <size_t Index>
  void countingSort(vector<ManberString>& strings,
                    vector<vector<ManberString>>& buckets) {
    /* Distribute entries into buckets. */
    for (const auto& str: strings) {
      buckets[get<Index>(str.entry)].push_back(str);
    }
    
    /* Gather them all back in order. */
    size_t i = 0;
    for (auto& bucket: buckets) {
      for (const auto& entry: bucket) {
        strings[i] = entry;
        i++;
      }
      bucket.clear();
    }
  }

  /* Radix sorts the given list of pairs in-place. */
  void radixSort(vector<ManberString>& strings) {
    /* Buckets into which the elements will be dropped. We need no more buckets than
     * there are elements.
     */
    vector<vector<ManberString>> buckets(strings.size());
    
    countingSort<1>(strings, buckets);
    countingSort<0>(strings, buckets);
  }
}

/* Use Manber's algorithm to construct a suffix array. */
SuffixArray manberMyers(const vector<size_t>& text) {
  SuffixArray result = text;
  
  /* Keep building strings of longer and longer widths until the strings exceed
   * the length of the string.
   */
  for (size_t halfSize = 1; halfSize < result.size(); halfSize *= 2) {
    /* Form the pairs that will be used for the radix sorting. */
    vector<ManberString> strings;
    for (size_t i = 0; i < result.size(); i++) {
      strings.push_back({ make_tuple(result[i], (i + halfSize < result.size()? result[i + halfSize] : 0)), i });
    }
    
    /* Radix sort everything to get back the new ordering. */
    radixSort(strings);
    
    /* Map the sorted list back into the suffix array. Remember to collapse together
     * adjacent items that are identical to one another.
     */
    size_t index = 0;
    for (size_t i = 0; i < strings.size(); i++) {
      result[strings[i].index] = index;
      if (i != strings.size() - 1 && strings[i].entry != strings[i+1].entry) {
          index++;
      }
    }
  }
  
  /* We now have a table associating each element with its sorted position, but we
   * want the inverse of this (an array associating each sorted position with its
   * element).
   */
  SuffixArray sa(result.size());
  for (size_t i = 0; i < sa.size(); i++) {
    sa[result[i]] = i;
  }
  
  return sa;
}
