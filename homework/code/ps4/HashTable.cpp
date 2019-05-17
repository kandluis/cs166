#include "HashTable.h"
using namespace std;

HashTable::HashTable(size_t nElems) {
  for (size_t i = 0; i < nElems; i++) {
    elems.insert(i);
  }
}

bool HashTable::contains(size_t key) const {
  return elems.find(key) != elems.end();
}
