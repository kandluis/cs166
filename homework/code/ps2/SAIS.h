#ifndef SAIS_Included
#define SAIS_Included

#include "SuffixArray.h"

/**
 * Computes a suffix array for the given string using the SA-IS algorithm.
 * <p>
 * SA-IS is both one of the theoretically and practically fastest suffix array
 * construction algorithms. It runs in worst-case linear time and has excellent
 * practical performance, both due to a good compression ratio with each
 * recursive call and excellent locality of reference.
 *
 * @param text The text for which to build a suffix array. It's assumed that
 *        the characters are in the range [0, n), where n is the length of the
 *        string, and that the string is suffixed with a sentinel that
 *        lexicographically precedes all other characters.
 * @return A suffix array for the input string.
 */
SuffixArray sais(const std::vector<std::size_t>& text);

#endif
