#ifndef SuffixArray_Included
#define SuffixArray_Included

#include <vector>
#include <string>
#include <cstddef> // For size_t

/* Type: SuffixArray
 *
 * A type representing a suffix array. It's just a list of size_t's.
 */
using SuffixArray = std::vector<std::size_t>;

/**
 * Given a string, converts that string into one that's suitable for use in
 * suffix array algorithms. Each character is mapped to its index, and a
 * null terminator is appended.
 *
 * @param str The input string.
 * @return A version of that string prepared for use in suffix array algorithms.
 */
std::vector<std::size_t> toRankedArray(const std::string& str);

#endif
