#include "Search.h"

#if __has_include(<optional>)
#   include <optional>
    using std::optional;
    using std::nullopt;
#else
#   include <experimental/optional>
    using std::experimental::optional;
    using std::experimental::nullopt;
#endif

namespace {

// Returns true if 'prefix' is fully contained as a prefix of the suffix of
// 'text' starting at 'suffixIdx'.
bool IsPrefix(const std::string& text, const std::size_t suffixIdx,
              const std::string& prefix) {
  const std::size_t suffixLen = text.size() - suffixIdx;
  if (suffixLen < prefix.size()) {
    return false;
  }
  for (std::size_t i = 0; i < prefix.size(); i++) {
    if (prefix[i] != text[suffixIdx + i]) return false;
  }
  return true;
}

// Same semantics as std::string::compare but no copy of text is made.
int compare(const std::string& text, const std::size_t suffixIdx,
            const std::string& pattern) {
  const std::size_t suffixLen = text.length() - suffixIdx;
  const std::size_t minLen = std::min(suffixLen, pattern.length());
  for (std::size_t i = 0; i < minLen; i++) {
    if (text[suffixIdx + i] != pattern[i]) {
      return text[suffixIdx + i] - pattern[i];
    }
  }
  return suffixLen - pattern.length();
}

// Finds the index of the smallest suffix matching pattern.
// [startIndex, endIndex) is a half-open interval. Returns nullopt
// if no suffixes match pattern
optional<std::size_t> binarySearchSmallest(
  const std::string& pattern,
  const std::string& text,
  const SuffixArray& suffixArr,
  const std::size_t startIndex, const std::size_t endIndex) {
  if (startIndex >= endIndex) {
    return nullopt;
  }
  const auto recurse = [&](const std::size_t start, const std::size_t end) {
    return binarySearchSmallest(pattern, text, suffixArr, start, end);
  };
  const std::size_t midpoint = (startIndex + endIndex) / 2;
  const int result = compare(text, suffixArr[midpoint], pattern);
  // Pattern matches suffix exactly. This is the smallest such match.
  if (result == 0) {
    return midpoint;
  }
  // Either no match and 'suffix' is smaller, or match but 'suffix'
  // is shorter than 'pattern'. Either way, not a prefix match.
  // We search the right side of the array.
  if (result < 0) {
    return recurse(midpoint + 1, endIndex);
  }
  // Match, where 'pattern' is a prefix of 'suffix'.
  if (IsPrefix(text, suffixArr[midpoint], pattern)) {
    // Try to find a 'smaller' match on the left hand side.
    if (const auto maybeSmaller = recurse(startIndex, midpoint)) {
      return *maybeSmaller;
    }
    return midpoint;
  }
  // Not a match. 'suffix' is larger. Check the LHS.
  return recurse(startIndex, midpoint);
}

// Same as above but finds the largest matching suffix.
optional<std::size_t> binarySearchLargest(
  const std::string& pattern,
  const std::string& text,
  const SuffixArray& suffixArr,
  const std::size_t startIndex, const std::size_t endIndex) {
  if (startIndex >= endIndex) {
    return nullopt;
  }
  const auto recurse = [&](const std::size_t start, const std::size_t end) {
    return binarySearchLargest(pattern, text, suffixArr, start, end);
  };
  const std::size_t midpoint = (startIndex + endIndex) / 2;
  const int result = compare(text, suffixArr[midpoint], pattern);
  // Pattern matches suffix exactly.
  if (result == 0) {
    // Try to find a larger suffix which also matches.
    if (const auto maybeLarger = recurse(midpoint + 1, endIndex)) {
      return *maybeLarger;
    }
    return midpoint;
  }
  // Either no match and 'suffix' is smaller, or match but 'suffix'
  // is shorter than 'pattern'. Either way, not a prefix match.
  // We search the right side of the array.
  if (result < 0) {
    return recurse(midpoint + 1, endIndex);
  }
  // Match, where 'pattern' is a prefix of 'suffix'.
  if (IsPrefix(text, suffixArr[midpoint], pattern)) {
    // Try to find a 'larger' match on the right hand side.
    if (const auto maybeLarger = recurse(midpoint + 1, endIndex)) {
      return *maybeLarger;
    }
    return midpoint;
  }
  // Not a match. 'suffix' is larger. Check the LHS.
  return recurse(startIndex, midpoint);
}


}  // namespace


std::vector<std::size_t> searchFor(const std::string& pattern,
                                   const std::string& text,
                                   const SuffixArray& suffixArr) {
  const auto maybeSmallestSuffix = binarySearchSmallest(
                                     pattern, text, suffixArr, 0,
                                     suffixArr.size());
  if (!maybeSmallestSuffix) {
    // Nothing matches the pattern.
    return {};
  }
  const std::size_t smallestSuffix = *maybeSmallestSuffix;
  // If a smallest exist, a largest must too (worst case, they are the same value).
  const std::size_t largestSuffix = binarySearchLargest(
                                      pattern, text, suffixArr, 0,
                                      suffixArr.size()).value();
  std::vector<std::size_t> indeces;
  indeces.reserve(largestSuffix - smallestSuffix + 1);
  for (std::size_t i = smallestSuffix; i <= largestSuffix; i++) {
    indeces.push_back(suffixArr[i]);
  }
  return indeces;
}
