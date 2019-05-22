#include "Hashes.h"
#include <random>
#include <array>

namespace {
  /* Random number generator used throughout this implementation. */
  std::mt19937 engine(137);

  /* Large prime number, used throughout the implementation. Fortunately,
   * 2^31 - 1, which is the largest positive 32-bit integer, is prime!
   */
  const size_t kLargePrime = (1u << 31) - 1;

  size_t randomFieldElem() {
    std::uniform_int_distribution<size_t> dist(0, kLargePrime - 1);
    return dist(engine);
  }

  size_t random32Bits() {
    std::uniform_int_distribution<size_t> dist;
    return dist(engine);
  }
}

/* 2-independent polynomial hashing. This hash function works by picking a
 * random linear polynomial over the field F_p for some large prime p. Thanks
 * to the Fundamental Theorem of Algebra, knowing only one output of this
 * function makes it impossible to determine any other single output, though
 * knowing two outputs makes it possible to determine where each other output
 * will land.
 */
std::shared_ptr<HashFamily> twoIndependentPolynomialHash() {
  class TwoIndependentHashFamily: public HashFamily {
  public:
    virtual HashFunction get() const {
      size_t a = randomFieldElem();
      size_t b = randomFieldElem();
      
      return [a, b] (int key) {
        return (a * key + b) % kLargePrime;
      };
    }
    
    virtual std::string name() const {
      return "2-Independent Polynomial";
    }
  };
  
  return std::make_shared<TwoIndependentHashFamily>();
}

/* 3-independent polynomial hashing. This hash function works by picking a
 * random quadratic polynomial over the field F_p for some large prime p.
 */
std::shared_ptr<HashFamily> threeIndependentPolynomialHash() {
  class ThreeIndependentHashFamily: public HashFamily {
  public:
    virtual HashFunction get() const {
      size_t a = randomFieldElem();
      size_t b = randomFieldElem();
      size_t c = randomFieldElem();
      
      return [a, b, c] (int key) {
        return (a * key * key + b * key + c) % kLargePrime;
      };
    }
    
    virtual std::string name() const {
      return "3-Independent Polynomial";
    }
  };
  
  return std::make_shared<ThreeIndependentHashFamily>();
}

/* 5-independent polynomial hashing. This hash function works by picking a
 * random quartic polynomial over the field F_p for some large prime p.
 */
std::shared_ptr<HashFamily> fiveIndependentPolynomialHash() {
  class FiveIndependentHashFamily: public HashFamily {
  public:
    virtual HashFunction get() const {
      size_t a = randomFieldElem();
      size_t b = randomFieldElem();
      size_t c = randomFieldElem();
      size_t d = randomFieldElem();
      size_t e = randomFieldElem();
      
      return [a, b, c, d, e] (int key) {
        return (a * key * key * key * key + b * key * key * key + 
                c * key * key + d * key + e) % kLargePrime;
      };
    }
    
    virtual std::string name() const {
      return "5-Independent Polynomial";
    }
  };
  
  return std::make_shared<FiveIndependentHashFamily>();
}

/* Tabulation hashing. The input is broken apart into a series of bytes,
 * with each byte associated with a table of random values. The hash code
 * is formed by XORing all of those bytes together.
 */
std::shared_ptr<HashFamily> tabulationHash() {
  class TabulationHashFamily: public HashFamily {
  public:
    virtual HashFunction get() const {
      std::array<std::array<size_t, 256>, 4> table;
      for (size_t i = 0; i < 4; i++) {
        for (size_t byte = 0; byte < 256; byte++) {
          table[i][byte] = random32Bits();
        }
      }
      
      return [table] (int key) {
        size_t result = 0;
        for (size_t i = 0; i < 4; i++) {
          result ^= table[i][(key & (0xFF << (i * 8))) >> (i * 8)];
        }
        return result % kLargePrime;
      };
    }
    
    virtual std::string name() const {
      return "Tabulation Hash";
    }
  };
  
  return std::make_shared<TabulationHashFamily>();
}

/* The identity hash function. No hashing is done - we rely purely on the
 * randomness inherent in the keys to distribute elements.
 */
std::shared_ptr<HashFamily> identityHash() {
  class IdentityHashFamily: public HashFamily {
  public:
    virtual HashFunction get() const {
      return [] (int key) {
        return key;
      };
    }
    
    virtual std::string name() const {
      return "Identity Hash";
    }
  };
  
  return std::make_shared<IdentityHashFamily>();
}

/* The Jenkins hash function. This is a hash function that's known to have
 * decently good distribution, but doesn't have any probabilistic guarantees
 * associated with it.
 */
std::shared_ptr<HashFamily> jenkinsHash() {
  class JenkinsHashFamily: public HashFamily {
  public:
    virtual HashFunction get() const {
      return [] (int a) {
        a = (a + 0x7ed55d16) + (a << 12);
        a = (a ^ 0xc761c23c) ^ (a >> 19);
        a = (a + 0x165667b1) + (a << 5);
        a = (a + 0xd3a2646c) ^ (a << 9);
        a = (a + 0xfd7046c5) + (a << 3);
        a = (a ^ 0xb55a4f09) ^ (a >> 16);
        return a;
      };
    }
    
    virtual std::string name() const {
      return "Jenkins Hash";
    }
  };
  
  return std::make_shared<JenkinsHashFamily>();
}
