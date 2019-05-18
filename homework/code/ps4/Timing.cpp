#include "Timing.h"
#include <algorithm>
#include <sstream>

/**
 * Prints a TimingResults to an output stream.
 */
std::ostream& operator<< (std::ostream& out, const TimingResults& results) {
  std::ostringstream builder;
  builder << "Build time: " << results.buildTime << "  "
          << "Query time: " << results.queryTime;
  return out << builder.str();
}

/**
 * Returns a random number generator that generates data according to a Zipfian
 * distribution, a type of distribution that occurs frequently in real-world
 * data, especially exst. See https://en.wikipedia.org/wiki/Zipf%27s_law.
 * Higher z values give more uneven data. When z is 0, the distribution is uniform.
 */
std::discrete_distribution<int> zipfian(std::size_t count, double z) {
  std::vector<double> weights(count);
  for (uint i = 0; i < count; i++) {
    weights[i] = 1 / pow(i + 1, z);
  }
  
  /* Permute the elements. This makes it unlikely that the elements that will
   * be looked up will be anywhere near one another.
   */
  std::random_device rd;
  std::mt19937 generator(rd());
  std::shuffle(weights.begin(), weights.end(), generator);
  return std::discrete_distribution<int>(weights.begin(), weights.end());
}
