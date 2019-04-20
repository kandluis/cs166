#include "DC3.h"
#include "ManberMyers.h"
#include <tuple>
#include <algorithm>
using namespace std;

namespace {
  /* Threshold controlling where the DC3 base case is triggered. Larger
   * values help up to a point.
   */
  const size_t kBaseCaseSize = 50;

  /* Given a string, forms the intermediate DC3 doubled array.
   * This string consists of T[1:] padded with sentinels followed
   * by T[2:] padded with sentinels.
   */
  vector<size_t> doubledVersionOf(const vector<size_t>& str) {
    vector<size_t> result;
    result.reserve(str.size() * 2 + 3);

    for (size_t i = 1; i < str.size(); i++) {
      result.push_back(str[i]);
    }
    while (result.size() % 3 != 0) {
      result.push_back(0);
    }
    for (size_t i = 2; i < str.size(); i++) {
      result.push_back(str[i]);
    }
    
    while (result.size() % 3 != 0) {
      result.push_back(0);
    }
    
    return result;
  }

  /* Type: DC3Block
   *
   * A type representing a triple of character from one of the input strings.
   */
  struct DC3Block {
    tuple<size_t, size_t, size_t> entry;
    size_t index;
  };

  /* Uses counting sort to sort a collection of triples of characters. */
  template <size_t index> void countingSort(vector<DC3Block>& blocks,
                                            vector<vector<DC3Block>>& buckets) {
    /* Distribute everything. */
    for (const auto& block: blocks) {
      buckets[get<index>(block.entry)].push_back(block);
    }
    
    /* Gather it back up. */
    size_t i = 0;
    for (auto& bucket: buckets) {
      for (auto& entry: bucket) {
        blocks[i] = entry;
        i++;
      }
      bucket.clear();
    }
  }

  /* Radix sorts a list of DC3 blocks. */
  void radixSort(vector<DC3Block>& blocks, size_t numBuckets) {
    vector<vector<DC3Block>> buckets(numBuckets);
    
    countingSort<2>(blocks, buckets);
    countingSort<1>(blocks, buckets);
    countingSort<0>(blocks, buckets);
  }

  /* Builds the partial suffix array for the positions in text that aren't congruent
   * to 0 mod 3.
   */
  SuffixArray solveNonCongruentPositionsIn(const vector<size_t>& str) {
    /* Form the first prefix string and pad up to a length that's a multiple of three. */
    vector<size_t> bigString = doubledVersionOf(str);
    
    /* Form blocks of three characters each, each tagged with their index, to prep for
     * the radix sort.
     */
    size_t maxValue = *max_element(bigString.begin(), bigString.end());
    vector<DC3Block> blocks;
    for (size_t i = 0; i < bigString.size(); i += 3) {
      blocks.push_back({ make_tuple(bigString[i], bigString[i+1], bigString[i+2]), i / 3 });
    }
    
    /* Radix sort these items to get them in order. */
    radixSort(blocks, maxValue + 1);
    
    /* Form a new string by relabeling each character with its index. */
    vector<size_t> newString(blocks.size() + 1); // Implicity 0-terminated
    size_t index = 1; // Because the sentinel is 0.
    for (size_t i = 0; i < blocks.size(); i++) {
      newString[blocks[i].index] = index;
      
      if (i != blocks.size() - 1 && blocks[i].entry != blocks[i+1].entry) index++;
    }
    
    /* Run DC3, recursively, to get the suffix array. */
    auto suffixArr = dc3(newString);
    
    /* Compute the inverse suffix array, which is actually what we want
     * because we want to know the rank of each suffix.
     */
    SuffixArray inverse(suffixArr.size());
    for (size_t i = 0; i < suffixArr.size(); i++) {
      inverse[suffixArr[i]] = i;
    }
    
    /* Map everything back to its original positions. */
    /* Remember that all elements here are shifted up by one because
     * the sentinel at the end of the list always comes first. So we'll
     * be subtracting one from everything here.
     */
    vector<size_t> result(str.size());
    for (size_t i = 0; i < inverse.size() - 1; i++) {
      if (i < inverse.size() / 2) {
        result[3*i + 1] = inverse[i] - 1;
      } else {
        result[3*(i - inverse.size() / 2) + 2] = inverse[i] - 1;
      }
    }
    
    return result;
  }

  /* Given a string and the relative orderings of the noncongruent suffixes,
   * determine the relative ordering of the congruent suffixes.
   */
  vector<size_t> sortCongruentPositions(const vector<size_t>& str,
                                        const SuffixArray& partial) {

    /* Form all of the pairs to sort. We'll store them as tuples whose
     * last element is ignored.
     */
    vector<DC3Block> blocks;
    for (size_t i = 0; i < str.size(); i += 3) {
      blocks.push_back({ make_tuple(str[i], (i == str.size() - 1? 0 : partial[i+1]), 0), i });
    }
    
    /* Radix sort everything. */
    radixSort(blocks, str.size() + 1);
    
    vector<size_t> result;
    for (const auto& entry: blocks) {
      result.push_back(entry.index);
    }
    
    return result;
  }

  /* Given a partial suffix array, returns a list of all the noncongruent suffixes
   * in sorted order.
   */
  vector<size_t> sortNonCongruentPositions(const SuffixArray& partial) {
    vector<size_t> result(*max_element(partial.begin(), partial.end()) + 1);
    
    for (size_t i = 0; i < partial.size(); i++) {
      if (i % 3 != 0) {
        result[partial[i]] = i;
      }
    }
    
    return result;
  }

  /* Performs the merging step necessary to get all the suffixes into sorted
   * order. Returns a list of the suffixes in sorted order, which happens
   * to be the suffix array we want!
   */
  SuffixArray merge(const vector<size_t>& sorted0,
                    const vector<size_t>& sorted12,
                    const vector<size_t>& str,
                    const SuffixArray& partial) {
    SuffixArray result;
    
    size_t i0 = 0, i12 = 0;
    while (i0 < sorted0.size() && i12 < sorted12.size()) {
      bool i0wins = false;
    
      /* See if the characters differ. Use that as a primary tiebreaker. */
      if (str[sorted0[i0]] != str[sorted12[i12]]) {
        i0wins = (str[sorted0[i0]] < str[sorted12[i12]]);
      }
      /* So the characters match. If this is a 1-suffix, we just look at
       * the next entries in the partial array as our tiebreaker.
       */
      else if (sorted12[i12] % 3 == 1) {
        /* Note: this case can't happen if we're looking at the very last element
         * of the array, since that sentinel should tiebreak us above.
         */
        i0wins = (partial[sorted0[i0] + 1] < partial[sorted12[i12] + 1]);
      }
      /* The characters match and this is a 2-suffix. So look at the next characters
       * as a tiebreaker, and if that fails look at the suffix information.
       */
      else {
        if (str[sorted0[i0] + 1] != str[sorted12[i12] + 1]) {
          i0wins = (str[sorted0[i0] + 1] < str[sorted12[i12] + 1]);
        } else {
          i0wins = (partial[sorted0[i0] + 2] < partial[sorted12[i12] + 2]);
        }
      }
      
      if (i0wins) {
        result.push_back(sorted0[i0]);
        i0++;
      } else {
        result.push_back(sorted12[i12]);
        i12++;
      }
    }
    
    /* Transfer all remaining elements. */
    for (; i0 < sorted0.size(); i0++) {
      result.push_back(sorted0[i0]);
    }
    for (; i12 < sorted12.size(); i12++) {
      result.push_back(sorted12[i12]);
    }
    
    return result;
  }
}

SuffixArray dc3(const vector<size_t>& text) {
  /* Base case: Any sufficiently small string we just solve naively. */
  if (text.size() < kBaseCaseSize) return manberMyers(text);
  
  /* Get a partial result for strings at positions that are congruent to 1 and 2 mod 3. */
  auto partial12 = solveNonCongruentPositionsIn(text);
 
  /* Get a list of the congruent suffixes in sorted order. */
  auto sorted0 = sortCongruentPositions(text, partial12);
  
  /* Get a list of the noncongruent suffixes in sorted order. */
  auto sorted12 = sortNonCongruentPositions(partial12);
  
  /* Merge them into an overall list. */
  return merge(sorted0, sorted12, text, partial12);
}
