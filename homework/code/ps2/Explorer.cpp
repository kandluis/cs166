#include "SuffixArray.h"
#include "DC3.h"
#include "SAIS.h"
#include "Search.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <functional>
#include <cassert>
#include <iomanip>
#include <cctype>
using namespace std;

namespace { // Private helper functions
  /* Type: SABuilder
   *
   * A type representing something that can build a suffix array.
   */
  using SABuilder = SuffixArray (*) (const vector<size_t>&);

  /* Prompts the user and reads a line of text. */
  string readLine(const string& prompt) {
    cout << prompt;
    string result;
    getline(cin, result);
    return result;
  }

  /* Given a string, replaces each non-alphanumeric, non-punctuation character with
   * a space, making it easier to see what's in the string and eliminating issues
   * from things like newlines and Unicode weirdness.
   */
  string denoise(string input) {
    for (char& ch: input) {
      if (!isalnum(ch) && !ispunct(ch)) ch = ' ';
    }
    return input;
  }

  /* Given a match of a pattern string in a text, returns a pretty text string
   * showing where that match is.
   */
  string textAround(const string& pattern, size_t index, const string& text) {
    assert(index < text.size());  // Could overflow, need two checks
    assert(index + pattern.size() <= text.size());
    assert(text.compare(index, pattern.size(), pattern) == 0);
    
    /* How much lookahead and lookbehind to have. */
    const size_t kMaxLookahead = 30, kMaxLookbehind = 30;
    size_t lookahead  = min(kMaxLookahead, text.size() - index);
    size_t lookbehind = min(kMaxLookbehind, index);
    
    /* If there's extra text leading off the front or back, put in an indicator to
     * that effect. Otherwise, do nothing.
     */
    string header = (index <= kMaxLookbehind? "     " : "[...]");
    string footer = (index + lookahead < text.size()? "[...]" : "     ");
    
    /* Some leading context. */
    string prefix = denoise(string(text.c_str() + index - lookbehind, lookbehind));
    
    /* The string itself. We use ANSI control codes to make it look pretty. */
    string match = "\033[44m\033[1m" + (pattern.length() >= lookahead? pattern.substr(0, lookahead) : pattern) +
                   "\033[0m";
                   
    /* Trailing context. */
    string suffix = denoise(string(text.c_str() + index + pattern.length(), pattern.length() >= lookahead? 0 : lookahead - pattern.length()));
    
    /* Assemble the result. */
    ostringstream result;
    result << header
           << right << setw(kMaxLookbehind) << prefix
           << match 
           << suffix
           << footer;
           
    return result.str();
  }

  /* Lets the user explore around the text using suffix array searches. */
  void searchTextFor(SABuilder builder, const string& text) {
    /* Cutoff size for where we stop printing suffix array.s */
    const size_t kCutoffSize = 30;
    
    /* Build the suffix array. This might take a while, so give the user a heads-up. */
    cout << "Building suffix array... " << flush;
    auto sa = builder(toRankedArray(text));
    cout << "done!" << endl;
    
    /* Print the suffix array if it isn't too large. */
    if (sa.size() < kCutoffSize) {
      cout << "Generated suffix array: " << endl;
      cout << "[";
      for (size_t i = 0; i < sa.size(); i++) {
        cout << setw(2) << sa[i];
        if (i != sa.size() - 1) cout << ", ";
      }
      cout << "]" << endl;
    } else {
      cout << "Text is too long to display suffix array." << endl;
    }
    
    while (true) {
      string pattern = readLine("Enter a term to search for (ENTER to exit): ");
      if (pattern.empty()) break;
      
      auto matches = searchFor(pattern, text, sa);
      if (matches.empty()) {
        cout << "  No matches found." << endl;
      } else {
        for (auto match: matches) {
          cout << textAround(pattern, match, text) << endl;
        }
        cout << matches.size() << " match" << (matches.size() == 1? "" : "es") << " total." << endl;
      }
    }                  
  }

  /* Given a stream, returns the contents of that stream. */
  string contentsOf(istream& input) {
    ostringstream converter;
    converter << input.rdbuf();
    return converter.str();
  }

  /* Function that prompts the user to choose which text they'd like to build a
   * suffix array for.
   */
  string userChooseText() {
    while (true) {
      string line = readLine("Enter name of data file, or a text string (ENTER to quit): ");
      if (line.empty()) exit(0);
      
      /* Try to open this as a file. */
      if (ifstream input{line}) {
        return contentsOf(input);
      } else {
        cout << "Didn't find a file named \"" << line << "\"; treating that as a text string." << endl;
        return line;
      }
    }
  }

  /* Prompts the user to choose which suffix array construction algorithm
   * they want to use.
   */
  SABuilder userChooseBuilder() {
    while (true) {
      string builder = readLine("Use your (S)A-IS implementation, or our (D)C3 algorithm? ");
      if (builder == "S" || builder == "s" || builder == "SA-IS" || builder == "SAIS" ||
          builder == "sa-is" || builder == "sais" || builder == "") return sais;
      if (builder == "D" || builder == "d" || builder == "DC3" || builder == "dc3") return dc3;
      
      cout << "Sorry, I didn't understand that." << endl;
    }
  }
}

int main() {
  while (true) {
    cout << "Suffix Array Explorer" << endl;
    string text  = userChooseText();
    auto builder = userChooseBuilder();
    searchTextFor(builder, text);
  }
}
