#include "SuffixArray.h"
#include "ManberMyers.h"
#include "DC3.h"
#include "Search.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <cassert>
#include <iomanip>
#include <cctype>
#include <algorithm>
#include <iterator>
#include <sys/types.h>
#include <dirent.h>
using namespace std;

namespace { // Private Helper Functions
  void abortProgram() {
    cout << "Run this program under gdb and backtrace for more information." << endl;
    abort();
  }

  /* Prints out a list of matches in a human-readable format. */
  void printMatches(const string& header, const vector<size_t>& matches) {
    cout << header << " [";
    for (size_t i = 0; i < matches.size(); i++) {
      cout << matches[i] << (i == matches.size() - 1? "" : ", ");
    }
    cout << "]" << endl;
  }

  /* Confirms that searchFor behaves properly on the specified strings. */
  void assertSearchCorrect(const string& pattern, const string& text,
                           const SuffixArray& sa, bool verbose) {
    if (verbose) cout << "  Looking for \"" << pattern << "\"... " << flush;

    /* Perform a student query. */
    auto student = searchFor(pattern, text, sa);
    
    if (verbose) cout << "(" << student.size() << " match" << (student.size() == 1? "" : "es") << ".)" << endl;
    
    /* Confirm that all entries are unique. */
    sort(student.begin(), student.end());
    for (size_t i = 0; i + 1 < student.size(); i++) {
      if (student[i] == student[i + 1]) {
        cout << "You reported the same match multiple times." << endl;
        printMatches("Matches reported: ", student);
        abortProgram();
      }
    }
    
    /* Manually search for all instances of this string. */
    vector<size_t> ours;
    for (size_t i = 0; i = text.find(pattern, i), i != string::npos; i++) {
      ours.push_back(i);
    }
    
    if (student != ours) {
      cout << "Your matches differ from ours." << endl;
      printMatches("Matches reported: ", student);
      printMatches("Actual matches:   ", ours);
      abortProgram();
    }
  }

  /* Reports that a test was passed. Unified to make the reporting cleaner. */
  void passTest() {
    cout << "Pass!" << endl << endl;
  }

  /* Macro: error(msg)
   * ---------------------------------------------------------------------------
   * Reports an error and terminates the program.
   */
  #define error(msg) do_error(msg, __FILE__, __LINE__)
  void do_error(const string& msg, const string& file, unsigned line) {
    cerr << "ERROR:  " << msg << endl;
    cerr << "  File: " << file << endl;
    cerr << "  Line: " << line << endl;
    abortProgram();
  }

  /* Returns the contents of the specified file. */
  string contentsOf(const string& filename) {
    ifstream input(filename);
    if (!input) error("Could not open file " + filename + " for reading.");
    ostringstream converter;
    converter << input.rdbuf();
    return converter.str();
  }

  /* Constant: TEST_DIRECTORY
   * ---------------------------------------------------------------------------
   * Name of the directory containing test cases to run. This must end with a
   * slash.
   */
  #define TEST_DIRECTORY "Tests/"

  /* Constant: kTestFileSuffix
   * ---------------------------------------------------------------------------
   * The expected suffix for a test file. Only files with this suffix will get
   * tested.
   */
  const string kTestFileSuffix = ".searches";

  /* Constant: kDNAChars
   * ---------------------------------------------------------------------------
   * String containing all possible letters that can appear in a DNA strand.
   */

  const string kDNAChars = "ACGT";

  /* Constant: kFixedTestSize
   * ---------------------------------------------------------------------------
   * Size to use when brute-forcing all possible strings of a given length.
   */
  const size_t kFixedTestSize = 7;

  /* Recursive helper function to exhaustively enumerate sequences. */
  void recGenerateSequencesUpTo(size_t n, vector<string>& result, string curr) {
    result.push_back(curr);
    
    /* Base case: If we've hit the maximum size, we're done. */
    if (curr.size() != n) {
      for (char ch: kDNAChars) {
        recGenerateSequencesUpTo(n, result, curr + ch);
      }
    }
  }

  /* Produces a list of all sequences of length up to some fixed size,
   * in reverse-sorted order.
   */
  vector<string> allSequencesOfLengthUpTo(const size_t n) {
    vector<string> result;
    recGenerateSequencesUpTo(n, result, "");
    sort(result.begin(), result.end(),
         [](const auto& lhs, const auto& rhs) { return lhs.length() > rhs.length(); });
    return result;
  }

  /* Brute-force tests search by testing all possible searches on all possible
   * DNA strands of a certain length.
   */
  void bruteForceTests() {
    cout << "Exhaustively testing your search implementation on all DNA strands of length " 
         << kFixedTestSize << "." << endl;
    cout << "We'll be searching for every DNA sequence of length up to " << kFixedTestSize
         << ", inclusive, in each DNA sequence of length exactly " << kFixedTestSize << "." << endl;
    cout << "(This might take a few minutes.)" << endl;
    
    vector<string> allSequences = allSequencesOfLengthUpTo(kFixedTestSize);
    
    /* Scan over all sequences of the maximum length and try searching for every
     * possible substring.
     */
    for (const string& text: allSequences) {
      if (text.length() != kFixedTestSize) break;
      auto sa = dc3(toRankedArray(text));
      
      /* Do all possible queries. */
      for (const string& pattern: allSequences) {
        assertSearchCorrect(pattern, text, sa, false);
      }
    }
    
    passTest();
  }

  /* Tests searchFor against all the reference files. */
  void sampleFileTests() {
    cout << "We'll now test for a bunch of specific sequences inside of our prepared test files." << endl;

    DIR* tests = opendir(TEST_DIRECTORY);
    if (tests == NULL) error("Could not open " TEST_DIRECTORY " for reading.");
    
    for (struct dirent* entry; (entry = readdir(tests)) != NULL; ) {
      /* If this file doesn't have an affiliated test file, skip it. */
      string testFile = TEST_DIRECTORY + string(entry->d_name);
      ifstream tests(testFile + kTestFileSuffix);
      if (!tests) {
        /* Clear the global error flag, which would be set here.
         * This error-reporting system is a very well-designed
         * system and I can't see any problems with it.
         */
        errno = 0;
        continue;
      }
      
      string fileContents = contentsOf(testFile);
      
      /* Use DC3 to build a suffix array; we can't assume the student solution
       * works properly at this point.
       */
      cout << "Using our reference DC3 algorithm to build a suffix array for " << testFile << "... " << flush;
      auto sa = dc3(toRankedArray(fileContents));
      cout << "done!" << endl;
      for (string pattern; getline(tests, pattern); ) {
        assertSearchCorrect(pattern, fileContents, sa, true);
      }
      
      cout << endl;
    }
    if (errno != 0) error("Error traversing the " TEST_DIRECTORY " directory.");
    closedir(tests);
    
    passTest();
  }
}

int main() {
  bruteForceTests();
  sampleFileTests();
}
