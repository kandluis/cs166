#include "SplayTree.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <vector>
#include <stdexcept>
using namespace std;

namespace {  
  /* Converts a string to an natural number, reporting an error if the conversion
   * can't be done.
   */
  size_t toNatural(const string& str) {
    /* If this value contains a negative sign, something is wrong. */
    if (str.find('-') != string::npos) throw runtime_error("Couldn't interpret that as a natural number.");
  
    istringstream converter(str);
    
    size_t result;
    if (converter >> result, !converter) {
      throw runtime_error("Couldn't interpret that as a natural number.");
    }
    
    /* Make sure there's nothing left. */
    for (char ch; converter >> ch; ) throw runtime_error("Couldn't interpret that as a natural number.");
    
    return result;
  }
  
  /* Reads a line of text from the user. */
  string getLine(const string& prompt) {
    cout << prompt;
    
    string result;
    getline(cin, result);
    return result;
  }
  
  /* Prompts the user for a number of elements, reprompting as necessary. */
  size_t readSize() {
    cout << "This program constructs a splay tree with N elements, namely" << endl;
    cout << "0, 1, 2, ..., N - 1, and lets you perform queries on it." << endl;
    cout << endl;
    
    while (true) {
      try {
        return toNatural(getLine("Please enter N: "));
      } catch (const exception& e) {
        cout << e.what() << endl;
      }
    }
  }
  
  
  template <typename BST> void exploreTree() {
    auto size = readSize();
    cout << endl;
    
    cout << "Building tree... " << flush;
    BST tree(size);
    cout << "done!" << endl << endl;
    
    /* Let the student make some queries! */
    while (true) {
      string input = getLine("Enter a natural number to search for (or ENTER if you're done): ");
      if (input == "") break;
      
      /* Treat the input as an integer. */
      try {
        bool present = tree.contains(toNatural(input));
        cout << "  Is that key present? " << (present? "Yes" : "No") << endl;
      } catch (const exception& e) {
        cout << e.what() << endl;
      }
    }
    
    cout << endl;
  }
}

int main() {
  exploreTree<SplayTree>();
}
