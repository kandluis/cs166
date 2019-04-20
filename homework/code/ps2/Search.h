#ifndef Search_Included
#define Search_Included

#include "SuffixArray.h"

/**
 * Finds all occurrences of the given pattern string inside the text, with
 * a little help from the suffix array.
 *
 * @param pattern The pattern string to search for.
 * @param text The text string to search in.
 * @param sa A suffix array of text.
 * @return A list of all places in which the given pattern was found.
 */
std::vector<std::size_t> searchFor(const std::string& pattern,
                                   const std::string& text,
                                   const SuffixArray& sa);

#endif
