#ifndef Hashes_Included
#define Hashes_Included

#include <string>
#include <functional>
#include <memory>

/* Alias: HashFunction
 * ----------------------------------------------------------------------------
 * A type representing an object that can be called as a hash function. This
 * uses the C++ std::function type, which is essentially a smarter version of
 * a function pointer.
 */
using HashFunction = std::function<size_t(int)>;

/* Interface: HashFamily
 * ----------------------------------------------------------------------------
 * An interface representing a family of hash functions. To sample from the
 * family uniformly at random, invoke the 'get' function, which will return a
 * random hash function from the family.
 *
 * It is possible that the HashFactory wraps a family containing a single hash
 * function. You can assume that, for hash tables that require a family of
 * multiple hash functions, the hash factory will actually represent such a
 * family. For the other hash tables, it is entirely possible that there is
 * just a single hash function that will alwayas be returned.
 */
class HashFamily {
public:
  /* C++ism: Interface classes need virtual destructors. */
  virtual ~HashFamily() = default;
  
  /**
   * Function: get()
   * --------------------------------------------------------------------------
   * Returns a uniformly-random hash function from the family of hash
   * functions. An example usage:
   *
   *    HashFunction h = hashFamily->get();
   *    size_t bucket = h(key) % numBuckets;
   */
  virtual HashFunction get() const = 0;
  
  /**
   * Function: name()
   * --------------------------------------------------------------------------
   * Returns the name of the family of hash functions. Used for debugging and
   * logging purposes.
   */
  virtual std::string name() const = 0; // Purely for testing purposes 
};

/**
 * These functions return pointers to specific families of hash functions.
 *
 *   twoIndependentPolynomialHash:
 *      A family of 2-independent hash functions using random polynomials.
 *   threeIndependentPolynomialHash:
 *      A family of 3-independent hash functions using random polynomials.
 *   fiveIndependentPolynomialHash:
 *      A family of 5-independent hash functions using random polynomials.
 *   tabulationHash:
 *      A family of 3-independent hash functions that are known to have
 *      excellent performance and theoretical guarantees stronger than
 *      their independence would normally imply. See "The Power of Simple
 *      Tabulation Hashing" by Patrascu and Thorup.
 *   identityHash:
 *      A single hash function that's the identity function: h(x) = x.
 *   jenkinsHash:
 *      A single hash function that's known to, in practice, have nice
 *      statistical dispersion.
 */
std::shared_ptr<HashFamily> twoIndependentPolynomialHash();
std::shared_ptr<HashFamily> threeIndependentPolynomialHash();
std::shared_ptr<HashFamily> fiveIndependentPolynomialHash();
std::shared_ptr<HashFamily> tabulationHash();
std::shared_ptr<HashFamily> identityHash();
std::shared_ptr<HashFamily> jenkinsHash();

#endif
