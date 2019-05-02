#include "DynamicOverlap.h"
#include <iostream>
#include <vector>
#include <unordered_map>
#include <iomanip>
#include <random>
using namespace std;

namespace {
  /* Type representing an interval. */
  struct Interval {
    int start;
    int end;
  };

  /* Range of values to check. */
  const int kMinValue = -100;
  const int kMaxValue = +100;
  
  /* Likelihood of inserting a new interval (vs. removing) */
  const double kInsertProbability = 0.55; // Slight bias toward adding
  
  /* Number of iterations to run. */
  const size_t kNumIterations = 20000;
  
  /* Confirms that the answers given back by the dynamic structure match the
   * answers obtained from a naive answer.
   */
  void checkAgreement(const DynamicOverlap& d,
                      const vector<Interval>& intervals) {
    /* Step 1: Build up a map of coverage. Keys are integers, values are
     * the number of intervals covering them.
     */
    unordered_map<int, size_t> coverage;
    for (const Interval& interval: intervals) {
      /* Strange way of doing this, but it avoids integer overflow. */
      int curr = interval.start;
      while (true) {
        coverage[curr]++;
        
        if (curr == interval.end) break;
        curr++;
      }
    }
    
    /* Step 2: Confirm all those answers match. */
    int toCheck = kMinValue;
    while (true) {
      /* Confirm the answer matches. */
      auto itr = coverage.find(toCheck);
      int expected = (itr == coverage.end()? 0 : itr->second);
      int actual   = d.intervalsContaining(toCheck);
      
      if (expected != actual) {
        cerr << "Incorrect answer reported." << endl;
        cerr << "Backtrace this program in gdb for more information." << endl;
        abort();
      }
      
      /* Same as above - handles weirdness with INT_MAX. */
      if (toCheck == kMaxValue) break;
      toCheck++;
    }
  }
}

int main() {
  DynamicOverlap d;
  
  /* Container of all intervals used so far. */
  vector<Interval> intervals;
  
  /* Random source and distributions. */
  mt19937 generator(137);
  uniform_int_distribution<int> intDistribution(kMinValue, kMaxValue);
  bernoulli_distribution bernoulli(kInsertProbability); 
  
  cout << "Comparing your solution against a simple reference." << endl;
  cout << "Intervals will be inserted and removed at random." << endl;
  
  for (size_t i = 0; i < kNumIterations; i++) {
    /* Report progress. */
    if (i != 0 && 100 * i / kNumIterations > 100 * (i - 1) / kNumIterations) {
      cout << '\r' << setw(2) << (100 * i / kNumIterations) << "%" << flush;
    }
    
    /* Validate correctness. */
    checkAgreement(d, intervals);
    
    /* Change the intervals. Option 1: add an interval. */
    if (intervals.empty() || bernoulli(generator)) {
      Interval toAdd = {
        intDistribution(generator),
        intDistribution(generator)
      };
      
      /* Make sure the start and end are properly sequenced. */
      if (toAdd.start > toAdd.end) swap(toAdd.start, toAdd.end);
      
      intervals.push_back(toAdd);
      d.insert(toAdd.start, toAdd.end);
    }
    /* Option 2: remove an interval. */
    else {
      uniform_int_distribution<size_t> indexDist(0, intervals.size() - 1);
     
      /* Remove that interval. */
      size_t index = indexDist(generator);
      
      d.remove(intervals[index].start, intervals[index].end);
      swap(intervals[index], intervals.back());
      intervals.pop_back();
    } 
  }
  cout << "\r100%" << endl;
  cout << "All tests passed!" << endl;
}
