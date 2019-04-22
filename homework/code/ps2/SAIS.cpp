#include "SAIS.h"

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
// Enum defining the possible tags for a given suffix.
enum class SuffixTag {
  // An S-Type suffix is one whose starting character is smaller
  // than the starting character of the suffix following it.
  // If equal, then the suffix following it must also by S-Type.
  // The sentinal suffix is always an S-Type suffix.
  kSType = 0,
  // An S-Type suffix is one whose starting character is larger
  // than the starting character of the suffix following it. If
  // equal, then the suffix following it must also be L-Type.
  kLType = 1,
};

// A suffix consists of a 'type' and whether or not it's the left-most s-type.
struct Suffix {
  SuffixTag type;
  bool isLMS;
};


// Does a backwards pass over a non-empty text, tagging each
// index as an SType or LType value as well as computing the
// indexes of all left most s-type characters and tracking this infromation.
std::vector<Suffix> TagText(const std::vector<std::size_t>& text) {
  std::vector<Suffix> suffixes(text.size());
  // The sentinal is always an S-Type suffix.
  suffixes[text.size() - 1] = {SuffixTag::kSType, /*isLMS=*/false};
  for (std::size_t i = 1; i < text.size(); i++) {
    const std::size_t suffixIdx = text.size() - i - 1;
    if (text[suffixIdx] < text[suffixIdx + 1]) {
      suffixes[suffixIdx] = {SuffixTag::kSType, /*isLMS=*/false};
    }
    else if (text[suffixIdx] > text[suffixIdx + 1]) {
      suffixes[suffixIdx] = {SuffixTag::kLType, /*isLMS=*/false};
    }
    else {
      // The suffix is the same as the next one.
      suffixes[suffixIdx] = suffixes[suffixIdx + 1];
    }
  }

  // We can now find the left-most S-Type suffixes in order of appearance.
  std::vector<std::size_t> leftMostSTypeIndexes;
  for (std::size_t suffixIdx = 1; suffixIdx < text.size(); suffixIdx++) {
    if (suffixes[suffixIdx].type == SuffixTag::kSType &&
        suffixes[suffixIdx - 1].type == SuffixTag::kLType) {
      suffixes[suffixIdx].isLMS = true;
    }
  }
  if (text.size() == 1) {
    // The sentinal is always an LMS even if it's not
    // preceeded by a character.
    suffixes[text.size() - 1].isLMS = true;
  }

  return suffixes;
}

// Represents the bucket [start, end) half-open interval. Empty intervals are
// valid.
struct Interval {
  std::size_t start;
  std::size_t end;
};
// Constructs a mapping where result[char] is the interval specifying
// the bucket to which the suffix starting at char belongs.
std::vector<Interval> CreateSuffixBuckets(const std::vector<std::size_t> text) {
  // Since text is rank-order, there are text.size() (at most) characters.
  std::vector<std::size_t> charCount(text.size());
  // This maps from character to the first occurrence in the string.
  std::vector<optional<std::size_t>> firstOccurrence(text.size());
  for (std::size_t suffixIdx = 0; suffixIdx < text.size(); suffixIdx++) {
    const std::size_t suffixStartChar = text[suffixIdx];
    charCount[suffixStartChar]++;
    if (!firstOccurrence[suffixStartChar]) {
      firstOccurrence[suffixStartChar] = suffixIdx;
    }
  }
  // This is a mapping from chars (sorted, since by index) to their bucket
  // intervals (smaller character have smaller lower indexed buckets)
  std::vector<Interval> suffixBuckets(text.size());
  std::size_t startIdx = 0;
  for (std::size_t character = 0; character < text.size(); character++) {
    suffixBuckets[character] = {
      /*start=*/startIdx,
      /*end=*/startIdx + charCount[character]
    };
    startIdx = startIdx + charCount[character];
  }
  return suffixBuckets;
}
// Performs induced sorting on the input vector and returns the suffixArray
// corresponding to the induced sorting.
// 'suffixes' should contain information about each suffix of string
//    (L-Type vs S-Type) as well as whether the suffix is an LMS suffix.
// 'lmsIndexes' is the list of lms indexes, in the order in which we see them
// in the string (or we wish to).
SuffixArray InducedSorting(const std::vector<std::size_t>& input,
                           const std::vector<Suffix>& suffixes,
                           const std::vector<std::size_t> lmsIndexes) {
  // suffixBuckets[char] gives the interval defining the bucket to
  // which the all suffixes starting with char map to.
  const std::vector<Interval> suffixBuckets = CreateSuffixBuckets(input);
  // Our partially sorted suffix array.
  SuffixArray suffixArray(input.size());
  // This copy gets updated as we fill the suffixArray.
  std::vector<Interval> emptyRanges = suffixBuckets;
  // Reverse pass over the LSM suffixes to put them into their buckets.
  for (std::size_t i = 0; i < lmsIndexes.size(); i++) {
    const std::size_t suffixIdx = lmsIndexes[lmsIndexes.size() - i - 1];
    // Place at the end of it's bucket and shrink the range. Interval is
    // half-open, so subtract an additional 1.
    suffixArray[emptyRanges[input[suffixIdx]].end-- - 1] = suffixIdx;
  }

  // Forward pass to put L-Type suffixes into their buckets (first free slot).
  for (const std::size_t suffixIdx : suffixArray) {
    if (suffixIdx > 0 && suffixes[suffixIdx - 1].type == SuffixTag::kLType) {
      suffixArray[emptyRanges[input[suffixIdx - 1]].start++] = suffixIdx - 1;
    }
  }

  // We reset the buckets since we overwrote the endpoints.
  emptyRanges = suffixBuckets;
  // Backward pass to put the S-Type suffixes into their buckets (last slot).
  for (std::size_t i = 0; i < suffixArray.size(); i++) {
    const std::size_t suffixIdx = suffixArray[suffixArray.size() - i - 1];
    if (suffixIdx > 0 && suffixes[suffixIdx - 1].type == SuffixTag::kSType) {
      // Half-open interval, so subtract an additional 1.
      suffixArray[emptyRanges[input[suffixIdx - 1]].end-- - 1] = suffixIdx - 1;
    }
  }
  return suffixArray;
}

std::vector<std::size_t> ExtractLMSIndexes(const std::vector<Suffix>& input) {
  std::vector<std::size_t> output;
  for (std::size_t i = 0; i < input.size(); i++) {
    if (input[i].isLMS) {
      output.push_back(i);
    }
  }
  return output;
}

// 'pSuffixArray' has all the LMS-blocks sorted, but possibly
// out of order. This function numbers each of these blocks based on their
// sorted index, and breaks the 'text' into blocks matching the suffixes, and
// returns the reduced string corresponding to the input.
// 'suffixes' just contains information about each suffix of 'text'.
std::vector<std::size_t> formReducedString(
  const std::vector<std::size_t>& text, const std::vector<Suffix>& suffixes,
  const SuffixArray& pSuffixArray) {
  // The index into 'text' corresponding to the current block.
  std::size_t currBlockTextIdx = pSuffixArray[0];
  // blockIndexes[i] is none null if text[i:] is an LMS suffix. The value of
  // blockIndexes[i] in that case is the index of the corresponding LMS block.
  std::vector<optional<std::size_t>> blockIndexes(text.size());
  blockIndexes[currBlockTextIdx] = 0;
  for (std::size_t pSuffixIdx = 1; pSuffixIdx < pSuffixArray.size(); pSuffixIdx++) {
    const std::size_t suffixIdx = pSuffixArray[pSuffixIdx];
    if (!suffixes[suffixIdx].isLMS) continue;
    if (text[currBlockTextIdx] != text[suffixIdx]) {
      // Blocks don't match, assign new index and continue.
      blockIndexes[suffixIdx] = blockIndexes[currBlockTextIdx].value() + 1;
      currBlockTextIdx = suffixIdx;
      continue;
    }
    // Continue checking for possible block match.
    std::size_t textIdx1 = currBlockTextIdx + 1;
    std::size_t textIdx2 = suffixIdx + 1;
    while (!suffixes[textIdx1].isLMS && !suffixes[textIdx2].isLMS &&
           text[textIdx1] == text[textIdx2]) {
      textIdx1++;
      textIdx2++;
    }
    if (suffixes[textIdx1].isLMS != suffixes[textIdx2].isLMS ||
        text[textIdx1] != text[textIdx2]) {
      // We ended in a mismatch. Assign new index and continue.
      blockIndexes[suffixIdx] = blockIndexes[currBlockTextIdx].value() + 1;
      currBlockTextIdx = suffixIdx;
      continue;
    }
    // We had a matching block. Just assign the current index to the suffix.
    blockIndexes[suffixIdx] = blockIndexes[currBlockTextIdx].value();

  }

  // We can now use blockIndex to construct the reduced string.
  std::vector<std::size_t> reducedString;
  for (const optional<std::size_t> blockIndex : blockIndexes) {
    if (blockIndex) {
      reducedString.push_back(blockIndex.value());
    }
  }
  return reducedString;
}

// Returns a new array such that newValues[i] = values[index[i]].
std::vector<std::size_t> ReorderUsingIndeces(
  const std::vector<std::size_t>& values,
  const std::vector<std::size_t>& index) {
  std::vector<std::size_t> output(values.size());
  for (std::size_t i = 0; i < index.size(); i++) {
    output[i] = values[index[i]];
  }
  return output;
}

// When the 'text' contains only unique characters, we can construct the
// suffix array directly without any complicated methodoloy.
optional<SuffixArray> MaybeSuffixArray(
  const std::vector<std::size_t>& text) {
  std::vector<std::size_t> charCount(text.size());
  // A map from character to index in the text.
  std::vector<std::size_t> charIndexes(text.size());
  for (std::size_t charIdx = 0; charIdx < text.size(); charIdx++) {
    const std::size_t character = text[charIdx];
    if (charCount[character] == 0) {
      charCount[character]++;
      charIndexes[character] = charIdx;
    } else {
      // We ran into a character multiple times. We can no longer do simple way.
      return nullopt;
    }
  }
  std::vector<std::size_t> indeces;
  indeces.reserve(text.size());
  for (std::size_t i = 0; i < text.size(); i++) {
    indeces.push_back(i);
  }
  return ReorderUsingIndeces(indeces, charIndexes);
}


}  // namespace

// The SA-IS algorithm for constructing a SuffixArray, which is juat an array
// where all suffixes from text have been sorted.
SuffixArray sais(const std::vector<std::size_t>& text) {
  if (text.empty()) {
    return {};
  }
  // Base-case. When text is all unique chars, we can immediately return sorted
  // suffixes (as these are just the sorted chars using radix sort).
  if (const auto suffixArray = MaybeSuffixArray(text)) {
    return *suffixArray;
  }
  // In other cases, we gotta do it the hard way.

  // Step One: Annotate each character and find the LMS characters
  const std::vector<Suffix> suffixes = TagText(text);
  const std::vector<std::size_t> lmsIndexes = ExtractLMSIndexes(suffixes);
  // Step Two: First induced sorting to get LMS-Blocks in order.
  const SuffixArray pSuffixArray = InducedSorting(text, suffixes, lmsIndexes);
  // Step Three: Using the block-sorted suffix array, find the reduced string.
  const std::vector<std::size_t> reducedString = formReducedString(
        text, suffixes, pSuffixArray);
  // Step Four: Sort the LMS Substring by processing the reduced string.
  const SuffixArray reducedSuffixArray = sais(reducedString);
  const std::vector<std::size_t> sortedLMSIdx = ReorderUsingIndeces(
        lmsIndexes, reducedSuffixArray);
  // Step Five: Resort the blocks using the new sorted LMS Indexes.
  const SuffixArray suffixArray = InducedSorting(text, suffixes, sortedLMSIdx);
  return suffixArray;
}
