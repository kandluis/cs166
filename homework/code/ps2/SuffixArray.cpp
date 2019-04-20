#include "SuffixArray.h"
#include <set>
#include <map>
using namespace std;

/**
 * Given a string, converts that string into one that's suitable for use in
 * suffix array algorithms. Each character is mapped to its index, and a
 * null terminator is appended.
 *
 * @param str The input string.
 * @return A version of that string prepared for use in suffix array algorithms.
 */
vector<size_t> toRankedArray(const string& str) {
  /* Sort all of the characters in order. */
  set<char> charsUsed(str.begin(), str.end());
  
  /* Map each character to its index. */
  map<char, size_t> indices;
  for (char ch: charsUsed) {
    size_t index = indices.size() + 1; // +1 because of the end-of-string marker
    indices[ch] = index;
  }
  
  /* Convert the original string to a list of numbers. */
  vector<size_t> result;
  for (char ch: str) {
    result.push_back(indices[ch]);  
  }
  
  /* Append the explicit terminator. */
  result.push_back(0);
  
  return result;
}
