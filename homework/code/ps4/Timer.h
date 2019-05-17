#ifndef Timer_Included
#define Timer_Included

#include <chrono>
#include <cstdint>
#include <ostream>
#include <iomanip>
#include <sstream>
#include <string>

/**
 * A type representing a number of milliseconds.
 */
class Milliseconds {
public:
  /* Constructs a new Milliseconds object from a number of milliseconds. */
  explicit Milliseconds(double amount) : amount(amount) {}
  
  /* Prints a Milliseconds object to a stream. */
  friend std::ostream& operator<< (std::ostream& out, const Milliseconds& ms) {
    std::ostringstream converter;
    converter << std::setw(8) << std::fixed << std::setprecision(2) << ms.amount << "ms";
    return out << converter.str();
  }

private:
  double amount;
};

/**
 * A type representing a stopwatch. This is used to time the costs of various
 * operations.
 */
class Timer {
public:
  void start() {
    current = std::chrono::high_resolution_clock::now();
  }
  
  void stop() {
    total += std::chrono::high_resolution_clock::now() - current;
  }
  
  Milliseconds elapsed() const {
    return Milliseconds(std::chrono::duration_cast<std::chrono::nanoseconds>(total).count() / 1.0e6);
  }

private:
  std::chrono::high_resolution_clock::duration total = 
    std::chrono::high_resolution_clock::duration::zero();
  std::chrono::high_resolution_clock::time_point current;
};

#endif
