/**
 * Header file exporting timing and testing functions. Because of C++ template
 * linker issues, all template functions are implemented in this header file.
 */
#ifndef Timing_Included
#define Timing_Included

#include "Timer.h"
#include <memory>
#include <random>
#include <vector>
#include <cmath>
#include <cstddef>
#include <iostream>
#include <type_traits>

/**
 * Type: TimingResults
 *
 * A type representing timing information about a particular test, separated
 * into construction and query times.
 */
struct TimingResults {
  Milliseconds buildTime;
  Milliseconds queryTime;
};

std::ostream& operator<< (std::ostream& out, const TimingResults& results);

/* The random seed used throughout the run. */
static const std::size_t kRandomSeed = 137;

/**
 * Returns a random number generator that generates data according to a Zipfian
 * distribution, a type of distribution that occurs frequently in real-world
 * data, especially exst. See https://en.wikipedia.org/wiki/Zipf%27s_law.
 * Higher z values give more uneven data. When z is 0, the distribution is uniform.
 */
std::discrete_distribution<int> zipfian(std::size_t count, double z);

/**
 * This metafunction is designed so that our test driver can determine
 * whether it should, when constructing a tree, pass in the actual weights
 * assigned to each element (true) or whether it should only provide the
 * number of elements (false).
 */
template <typename T> struct NeedsWeights {
  private:
    /* Use SFNIAE overloading to detect which of these two options to pick. */
    struct Yes{};
    struct No {};

    template <typename U>
    static Yes check(int,
                     decltype(new U{std::vector<double>{}}) = 0);
    template <typename U> static No  check(...);

  public:
    static constexpr bool value = std::is_same<decltype(check<T>(0)), Yes>::value;
};

/* Given a list of weights, returns either (1) the weights themselves or
 * (2) the number of weights, based on what the tree type needs.
 */
template <typename Tree>
typename std::enable_if<NeedsWeights<Tree>::value, const std::vector<double>&>::type
toConstructor(const std::vector<double>& weights) {
  return weights;
}

template <typename Tree>
typename std::enable_if<!NeedsWeights<Tree>::value, std::size_t>::type
toConstructor(const std::vector<double>& weights) {
  return weights.size();
}

/**
 * Given a probability distribution and a list of the underlying probabilities,
 * runs a time trial to determine how quickly the indicated number of lookups
 * can be performed on a BST of the specified type. The number of elements in
 * the underlying tree will be equal to the number of distinct probabilities,
 * and the assumption is that the elements will be 0, 1, 2, ...,
 * probabilities.size() - 1.
 */
template <typename BST, typename ProbabilityDistribution>
TimingResults timeDistribution(ProbabilityDistribution& gen,
                               const std::vector<double>& probabilities,
                               std::size_t numLookups) {
  std::default_random_engine engine;
  engine.seed(kRandomSeed);

  Timer buildTimer;
  
  buildTimer.start();
  BST tree{toConstructor<BST>(probabilities)};
  buildTimer.stop();

  Timer queryTimer;
  for (std::size_t i = 0; i < numLookups; i++) {
    auto index = gen(engine);
    
    queryTimer.start();
    tree.contains(index);
    queryTimer.stop();
  }

  return { buildTimer.elapsed(), queryTimer.elapsed() };
}

/**
 * Given a discrete distribution, times the performance of performing BST
 * lookups on that distribution.
 */
template <typename BST>
TimingResults timeDistribution(std::discrete_distribution<int>& gen, std::size_t numLookups) {
  auto probabilities = gen.probabilities();
  return timeDistribution<BST>(gen, probabilities, numLookups);
}

/**
 * Given a uniform distribution, times the performance of performing BST
 * lookups on that distribution.
 */
template <typename BST>
TimingResults timeDistribution(std::uniform_int_distribution<int>& gen,
                               std::size_t numLookups) {
  auto probabilities = std::vector<double>(gen.max() + 1, 1.0 / (1 + gen.max()));
  return timeDistribution<BST>(gen, probabilities, numLookups);
}


/**
 * Given a BST type and a number of elements, reports the time required to
 * visit every element of that BST in sequence, either front-to-back or
 * back-to-front.
 */
template <typename BST>
TimingResults timeSequential(std::size_t count, int start, int step) {
  std::vector<double> probabilities = std::vector<double>(count, 1.0 / count);

  Timer buildTimer;
  
  buildTimer.start();
  BST tree{toConstructor<BST>(probabilities)};
  buildTimer.stop();

  Timer queryTimer;
  for (int i = start; i >= 0 && i < int(count); i += step) {
    queryTimer.start();
    tree.contains(i);
    queryTimer.stop();
  }

  return { buildTimer.elapsed(), queryTimer.elapsed() };
}

/**
 * Given a BST type and a number of elements, reports the time required to
 * visit every element of that BST in order.
 */
template <typename BST>
TimingResults timeSequential(std::size_t count) {
  return timeSequential<BST>(count, 0, 1);
}


/**
 * Given a BST type and a number of elements, reports the time required to
 * visit every element of that BST in reverse order.
 */
template <typename BST>
TimingResults timeReverseSequential(std::size_t count) {
  return timeSequential<BST>(count, count - 1, -1);
}

/**
 * Given a BST type, a number of lookups to perform, and a number of working
 * sets, runs a time trial by repeatedly selecting a random group of elements
 * to form a working set, then doing all queries within that working set. This
 * essentially tests for temporal locality.
 */
template <typename BST>
TimingResults timeWorkingSets(std::size_t numElems,
                              std::size_t numWorkingSets,
                              std::size_t workingSetSize,
                              std::size_t queriesPerSet) {
  std::default_random_engine engine;
  engine.seed(kRandomSeed);
  auto gen = std::uniform_int_distribution<size_t>(0, numElems);

  /* Access probabilities are essentially uniform. */
  std::vector<double> probabilities(numElems, 1.0 / numElems);

  Timer buildTimer;
  
  buildTimer.start();
  BST tree{toConstructor<BST>(probabilities)};
  buildTimer.stop();

  Timer queryTimer;
  
  for(std::size_t i = 0; i < numWorkingSets; i++) {
    /* Sample the working set. */
    std::vector<std::size_t> workingSet;
    for (std::size_t j = 0; j < workingSetSize; j++) {
      workingSet.push_back(gen(engine));
    }
    
    /* Run a bunch of queries on that set. */
    std::uniform_int_distribution<size_t> setPicker(0, workingSetSize - 1);
    for (size_t j = 0; j < queriesPerSet; j++) {
      std::size_t index = workingSet[setPicker(engine)];
      
      queryTimer.start();
      tree.contains(index);
      queryTimer.stop();
    }
  }
  
  return { buildTimer.elapsed(), queryTimer.elapsed() };
}

/**
 * Runs a test in which elements tend to have low rank difference from
 * preceding elements, which highlights the effects of trees with the
 * dynamic finger property.
 */
template <typename BST>
TimingResults timeDynamicFinger(std::size_t numElems,
                                std::size_t numQueries,
                                std::size_t maxStepSize) {
  std::default_random_engine engine;
  engine.seed(kRandomSeed);
  auto step = std::uniform_int_distribution<int>(-int(maxStepSize), +int(maxStepSize));

  /* Access probabilities are essentially uniform. */
  std::vector<double> probabilities(numElems, 1.0 / numElems);

  Timer buildTimer;
  
  buildTimer.start();
  BST tree{toConstructor<BST>(probabilities)};
  buildTimer.stop();

  Timer queryTimer;
  
  int index = int(numElems) / 2;
  
  for(std::size_t i = 0; i < numQueries; i++) {
    /* Do the lookup. */
    queryTimer.start();
    tree.contains(size_t(index));
    queryTimer.stop();
    
    /* Take a step. */
    index += step(engine);
    if (index < 0) index += int(numElems);
    if (index >= int(numElems)) index -= int(numElems);
  }
  
  return { buildTimer.elapsed(), queryTimer.elapsed() };
}


/**
 * Runs some basic correctness checks to ensure that the tree works correctly.
 * This involves looking up all the expected elements and a few that aren't
 * there.
 */
template <typename BST>
bool checkCorrectness(std::size_t count, std::size_t lookups) {
  std::default_random_engine engine;
  engine.seed(kRandomSeed);
  auto gen = std::uniform_int_distribution<int>(0, count - 1);
  auto probabilities = std::vector<double>(gen.max() + 1, 1.0 / (1 + gen.max()));

  BST tree{toConstructor<BST>(probabilities)};

  bool passed = true;
  
  /* Generate a bunch of random in-range keys and confirm that we can find all
   * of them.
   */
  for (std::size_t i = 0; i < lookups; i++) {
    passed = passed && tree.contains(gen(engine));
  }

  /* Check out of range to make sure we don't find anything. */
  passed = passed && !tree.contains(-1);
  passed = passed && !tree.contains(count);
  passed = passed && !tree.contains(count+1);

  /* Do one final pass checking that all the values in range are indeed present
   * that we expect to be present.
   */
  for (std::size_t i = 0; i < count; i++) {
    passed = passed && tree.contains(i);
  }
  return passed;
}

#endif
