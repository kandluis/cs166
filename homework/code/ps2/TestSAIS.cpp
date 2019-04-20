#include "SuffixArray.h"
#include "ManberMyers.h"
#include "DC3.h"
#include "SAIS.h"
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

  void printSA(const string& message, const SuffixArray& sa) {
    cout << message << " [";
    for (size_t i = 0; i < sa.size(); i++) {
      cout << sa[i] << (i == sa.size() - 1? "" : ", ");
    }
    cout << "]" << endl;
  }

  void assertSAISValidOn(const string& str) {
    const auto text    = toRankedArray(str);
    const auto student = sais(text);
    const auto ours    = dc3(text);
    
    if (student != ours) {
      cout << "Your suffix array does not match our expected reference." << endl;
      cout << "String: " << str << endl;
      printSA("Your suffix array: ", student);
      printSA("Our suffix array:  ", ours);
      abortProgram();
    }
  }

  void passTest() {
    cout << "Pass!" << endl << endl;
  }

  /* Constant: kDNAChars
   * ---------------------------------------------------------------------------
   * String containing all possible letters that can appear in a DNA strand.
   */

  const string kDNAChars = "ACGT";

  /* Constant: kFixedTestSize
   * ---------------------------------------------------------------------------
   * Size to use when brute-forcing all possible strings of a given length.
   */
  const size_t kFixedTestSize = 10;

  void exhaustiveTests() {
    cout << "Exhaustively testing your SA-IS implementation on all possible DNA strands of length "
         << kFixedTestSize << "." << endl;
    cout << "(This might take a few minutes.)" << endl;
    
    /* Enumerate all possible sequences. */
    vector<size_t> text(kFixedTestSize);
    
    while (true) {
      string str;
      transform(text.begin(), text.end(), back_inserter(str), [&](size_t index) { return kDNAChars[index]; });
      assertSAISValidOn(str);
      
      /* Advance to the next string. */
      size_t i = text.size();
      while (i >= 1 && text[i - 1] == kDNAChars.size() - 1) {
        text[i - 1] = 0;
        i--;
      }
      
      if (i == 0) break;
      text[i - 1]++;
    }
    
    passTest();
  }

  /* Constant: kDNAReferences
   * ---------------------------------------------------------------------------
   * The sample strings we've given out as test cases.
   */
  const vector<string> kDNAReferences = { 
    "ACGTGCCTAGCCTACCGTGCC", // From the handout
    "GTCCCGATGTCATGTCAGGA",  // From lecture
  };

  void dnaReferenceTests() {
    for (const string& test: kDNAReferences) {
      cout << "Testing SA-IS against our sample DNA string " << test << "." << endl;
      assertSAISValidOn(test);
    }
    
    passTest();
  }

  /* Constant: kLargeStringLength
   * ---------------------------------------------------------------------------
   * Length of the large strings to use when testing SA-IS against random
   * strings.
   */
  const size_t kLargeStringLength = 10000;

  /* Constant: kNumLargeTests
   * ---------------------------------------------------------------------------
   * How many tests should be run on strings of the longer length.
   */
  const size_t kNumLargeTests = 1000;

  /* Runs tests on giant strings with a large alphabet. */
  void sparseRandomTests() {
    cout << "Testing your SA-IS implementation against a collection of large, random strings of length " << endl; 
    cout << kLargeStringLength << " using characters drawn from a relatively large alphabet." << endl;
    
    for (size_t i = 0; i < kNumLargeTests; i++) { 
      string str;
      
      /* Build a string from printable, non-space characters. */
      while (str.size() < kLargeStringLength) {
        char ch = rand() % 128;
        if (isgraph(ch) && !isspace(ch)) str += ch;
      }
      
      assertSAISValidOn(str);
    }
    passTest();
  }

  /* Runs tests on giant strings with a small alphabet. */
  void denseRandomTests() {
    cout << "Testing your SA-IS implementation against a collection of large, random strings of length " << endl; 
    cout << kLargeStringLength << " using characters drawn from a relatively small alphabet." << endl;
    
    for (size_t i = 0; i < kNumLargeTests; i++) {    
      string str;
      for (size_t i = 0; i < kLargeStringLength; i++) {
        str += kDNAChars[rand() % kDNAChars.size()];
      }
      
      assertSAISValidOn(str);
    }
    passTest();
  }

  /* Constant: TEST_DIRECTORY
   * ---------------------------------------------------------------------------
   * Name of the directory containing test cases to run. This must end with a
   * slash.
   */
  #define TEST_DIRECTORY "Tests/"

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

  /* Reports the size of a file using nice units rather than just raw bytes. */
  string prettySizeOf(size_t bytes) {
    const vector<string> suffixes = { "B", "KB", "MB", "GB", "TB", "PB" };
    const size_t kMultiplier = 1024;
    
    size_t index = 0;
    size_t base  = 1;
    
    /* Keep advancing in size until we find the right threshold. */
    while (bytes >= base * kMultiplier) {
      index++;
      base *= kMultiplier;
    }
    
    return to_string(bytes / base) + suffixes[index];  
  }

  /* Constant: kTestFileSuffix
   * ---------------------------------------------------------------------------
   * The expected suffix for a test file. Only files with this suffix will get
   * tested.
   */
  const string kTestFileSuffix = ".txt";

  /* Tests SA-IS against all of the test files in the tests directory. */
  void sampleFileTests() {
    cout << "Testing your SA-IS implementation against a collection of real-world text files." << endl;
    
    DIR* tests = opendir(TEST_DIRECTORY);
    if (tests == NULL) error("Could not open " TEST_DIRECTORY " for reading.");
    
    for (struct dirent* entry; (entry = readdir(tests)) != NULL; ) {
      /* Make sure that this file is a valid test case. */
      string testFile = TEST_DIRECTORY + string(entry->d_name);
      if (testFile.size() < kTestFileSuffix.size() ||
          testFile.substr(testFile.size() - kTestFileSuffix.size()) != kTestFileSuffix) {
        continue;   
      }
      
      string fileContents = contentsOf(testFile);
      cout << "  File: " << testFile << endl;
      cout << "  Size: " << prettySizeOf(fileContents.size()) << endl;
      assertSAISValidOn(contentsOf(testFile));
      cout << endl;
    }
    if (errno != 0) error("Error traversing the " TEST_DIRECTORY " directory.");
    closedir(tests);
    
    passTest();
  }
}

int main() {
  dnaReferenceTests();
  exhaustiveTests();
  sparseRandomTests();
  denseRandomTests();
  sampleFileTests();
}
