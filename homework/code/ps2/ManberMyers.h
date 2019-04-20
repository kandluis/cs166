#ifndef ManberMyers_Included
#define ManberMyers_Included

#include "SuffixArray.h"

/**
 * Constructs the suffix array of the given string in time O(m log m) using
 * the Manber-Myers algorithm from their original paper.
 * <p>
 * This algorithm, which we didn't cover in lecture, uses dynamic programming
 * and repeated doubling, in conjunction with radix sort, to construct the
 * suffix array. It first sorts all substrings of length one, then all
 * substrings of length two, then length four, then length-eight, etc. Because
 * each substring of length 2<sup>k+1</sup> can be nicely split into two
 * smaller substrings of length 2<sup>k</sup>, given information about the
 * ranked ordering of the substrings of length 2<sup>k</sup>, it's possible
 * to sort all substrings of length 2<sup>k+1</sup> in time O(m). Since there
 * are O(log m) rounds before this is processing maximum-length strings, the
 * overall runtime is O(m log m).
 * <p>
 * We've included this algorithm because it's used by DC3 as its stopper
 * algorithm, but also so that you can see how this approach works. If you
 * needed to code something up in a pinch, this is a pretty good choice!
 *
 * @param text The text for which to build a suffix array. It's assumed that
 *        the characters are in the range [0, n), where n is the length of the
 *        string, and that the string is suffixed with a sentinel that
 *        lexicographically precedes all other characters.
 * @return A suffix array for that text.
 */
SuffixArray manberMyers(const std::vector<std::size_t>& text);

#endif
