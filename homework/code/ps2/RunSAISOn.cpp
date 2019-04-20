#include "SAIS.h"
#include <iostream>
using namespace std;

namespace { // Private helper functions
  void printSA(const string& message, const SuffixArray& sa) {
    cout << message << " [";
    for (size_t i = 0; i < sa.size(); i++) {
      cout << sa[i] << (i == sa.size() - 1? "" : ", ");
    }
    cout << "]" << endl;
  }
}

int main(int argc, const char* argv[]) {
  if (argc != 2) {
    cerr << "Usage: run-sais-on [test-string]" << endl;
    cerr << "  Don't include a sentinel at the end of your string; we'll do that for you." << endl;
    exit(-1);
  }
  
  printSA("Result: ", sais(toRankedArray(argv[1])));
}
