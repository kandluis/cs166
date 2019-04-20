#ifndef DC3_Included
#define DC3_Included

#include "SuffixArray.h"

/**
 * Computes a suffix array using the DC3 algorithm.
 * <p>
 * DC3 is a linear-time suffix array construction algorithm based on a
 * fundamentally different observation than SA-IS. It works by using a clever
 * divide-and-conquer technique (blocking the characters into groups of size 3,
 * radix-sorting those blocks, renaming them based on their index, and forming
 * a new string from those renamed blocks) to get the sorted order of the
 * suffixes at positions that aren't multiples of three, using that to induce
 * the order of the suffixes at positions that <i>are</i> at positions that are
 * multiples of three, and using a modified merge sort to combine them.
 * <p>
 * This algorithm is not as fast in practice as SA-IS; most reports put it at
 * somewhere between 3-4x slower and at 3-4x memory usage. This results from
 * both an extra amount of processing needed to perform multiple radix sorts
 * and from the fact that the reduction in problem size from each stage of the
 * algorithm to the next (2/3) is not nearly as good as SA-IS.
 *
 * @param text The text for which to build a suffix array. It's assumed that
 *        the characters are in the range [0, n), where n is the length of the
 *        string, and that the string is suffixed with a sentinel that
 *        lexicographically precedes all other characters.
 * @return A suffix array for that text.
 */
SuffixArray dc3(const std::vector<std::size_t>& text);

#endif
