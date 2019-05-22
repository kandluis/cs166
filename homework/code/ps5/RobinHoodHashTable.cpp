#include "RobinHoodHashTable.h"

RobinHoodHashTable::RobinHoodHashTable(size_t numBuckets, std::shared_ptr<HashFamily> family) {
  // TODO: Implement this
}

RobinHoodHashTable::~RobinHoodHashTable() {
  // TODO: Implement this
}

void RobinHoodHashTable::insert(int data) {
  // TODO: Implement this
}

bool RobinHoodHashTable::contains(int data) const {
  // TODO: Implement this
  return false;
}

/**
 * You should implement this operation using backward-shift deletion: once
 * you've found the element to remove, continue scanning forward until you
 * find an element that is at its home location or an empty cell, then shift
 * each element up to that point backwards by one step.
 */
void RobinHoodHashTable::remove(int data) {
  // TODO: Implement this
}
