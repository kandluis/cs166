#include "StdSetTree.h"
using namespace std;

StdSetTree::StdSetTree(size_t nElems) {
  for (size_t i = 0; i < nElems; i++) {
    elems.insert(i);
  }
}

bool StdSetTree::contains(size_t key) const {
  return elems.find(key) != elems.end();
}
