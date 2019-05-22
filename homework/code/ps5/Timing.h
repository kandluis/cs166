/* This file is adapted from an initial version written by CS166 TA
 * Kevin Gibbons in Spring 2016. Thanks, Kevin!
 */
#ifndef Timing_Included
#define Timing_Included

#include "Hashes.h"
#include "Timer.h"
#include <random>
#include <tuple>
#include <memory>
#include <unordered_set>
#include <map>
#include <vector>
#include <algorithm>

/* Type: HashFamilies
 * ----------------------------------------------------------------------------
 * Type representing a list of hash families.
 */
using HashFamilies = std::vector<std::shared_ptr<HashFamily>>;

/* Type: TimingInfo
 * ----------------------------------------------------------------------------
 * Type representing timing information about how long a run took to complete.
 */
struct TimingInfo {
  double insertTime;
  double successfulQueryTime;
  double unsuccessfulQueryTime;
};

/* Type: ResultsByLoad
 * ----------------------------------------------------------------------------
 * Type representing a map from load factors to timing information.
 */
using ResultsByLoad = std::map<double, TimingInfo>;

/* Type: ResultsByHash
 * ----------------------------------------------------------------------------
 * Type representing a map from hash function types to timing results.
 */
using ResultsByHash = std::map<std::string, ResultsByLoad>;

/* Multiplier used to determine the maximum integer that can be queried on an
 * operation. Higher numbers make for more likely misses in lookups.
 */
static const size_t kSpread = 4;

/* Baseline size of tables for testing. Also matches the number of elements
 * we'll be cramming into the tables.
 */
static const size_t kTableSize = 1u << 14u;

/* Number of iterations to run on each test. */
static const size_t kNumRounds = 1u << 4u;

/* Random device used to seed the random generator for each test. */
static std::random_device rd;

/**
 * Performs the given number of actions on a hash table whose hash functions
 * are sampled from the given family. The distribution of lookups is determined
 * by the distribution parameter.
 */
template <typename HashTable>
TimingInfo runTimeTests(std::shared_ptr<HashFamily> family,
                        double loadFactor) {
  std::mt19937 engine(rd());
  
  Timer insertTimer, successfulQueryTimer, unsuccessfulQueryTimer;
  
  /* Generate a random collection of elements to insert. To do this, we compute
   * a list of the values 0, 1, 2, ..., k for some appropriate k, randomly
   * permute those elements, and insert the first k of them.
   */
  const size_t kNumElems = kTableSize * loadFactor;
  std::vector<int> values(kSpread * std::max(kNumElems, kTableSize));
  std::iota(values.begin(), values.end(), 0);
  std::shuffle(values.begin(), values.end(), engine);

  for (size_t round = 0; round < kNumRounds; round++) {  
    HashTable table(kTableSize, family);
    
    /* Insert the specified number of values into the hash table. */
    for (size_t i = 0; i < kNumElems; i++) {    
      int toInsert = values[i];
      
      insertTimer.start();
      table.insert(toInsert);
      insertTimer.stop();
    }
    
    /* Perform some number of successful lookups. We'll permute the order of
     * the elements we're looking up to avoid any caching effects.
     */
    std::shuffle(values.begin(), values.begin() + kNumElems, engine);
    for (size_t i = 0; i < kNumElems; i++) {
      int toLookup = values[i];
      
      successfulQueryTimer.start();
      table.contains(toLookup);
      successfulQueryTimer.stop();
    }
    
    /* Look up all the elements that weren't in the table. */
    for (size_t i = kNumElems; i < values.size(); i++) {
      int toLookup = values[i];
      
      unsuccessfulQueryTimer.start();
      table.contains(toLookup);
      unsuccessfulQueryTimer.stop();
    }
  }
  
  return { 
    insertTimer.elapsed()            / (kNumElems  * kNumRounds),
    successfulQueryTimer.elapsed()   / (kNumElems * kNumRounds),
    unsuccessfulQueryTimer.elapsed() / ((values.size() - kNumElems) * kNumRounds)
  };
}

template <typename HashTable>
ResultsByHash timeHashTable(const std::vector<std::shared_ptr<HashFamily>>& factories,
                            const std::vector<double>& loadFactors) {
  ResultsByHash result;
  
  for (auto hashFamily: factories) {
    for (auto loadFactor: loadFactors) {
      result[hashFamily->name()][loadFactor] = runTimeTests<HashTable>(hashFamily, loadFactor);
    }
  }
  
  return result;
}


/**
 * Check correctness, using C++'s unordered_set type as an oracle
 */
template <typename HashTable>
bool checkCorrectness(size_t buckets, std::shared_ptr<HashFamily> family, size_t numActions) {
  std::mt19937 engine(rd());
  
  auto gen = std::uniform_int_distribution<int>(0, numActions * kSpread);
  auto coinFlip = std::bernoulli_distribution();
  
  HashTable table(buckets, family);
  std::unordered_set<int> reference;
  
  for (size_t i = 0; i < numActions; i++) {
    int value = gen(engine);
    if ((reference.count(value) > 0) != table.contains(value)) {
      return false;
    }
    if (coinFlip(engine)) {
      reference.insert(value);
      table.insert(value);      
    } else {
      reference.erase(value);
      table.remove(value);
    }
    if ((reference.count(value) > 0) != table.contains(value)) {
      return false;
    }
  }
  return true;
}

template <typename HashTable>
bool checkCorrectness(const std::vector<std::tuple<int, std::shared_ptr<HashFamily>, int>>& params) {
  for (auto param : params) {
    if (!checkCorrectness<HashTable>(std::get<0>(param), std::get<1>(param), std::get<2>(param))) {
      return false;
    }
  }
  return true;
}

template <typename HashTable>
bool checkCorrectness(const std::vector<std::shared_ptr<HashFamily>>& families) {
  for (auto family: families) {
    if (!checkCorrectness<HashTable>({
          std::make_tuple(12, family, 5),
          std::make_tuple(120, family, 50),
          std::make_tuple(12000, family, 5000)
        })) {
      return false;
    }
  }
  return true;
}

#endif
