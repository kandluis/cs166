#include "Hashes.h"
#include "ChainedHashTable.h"
#include "SecondChoiceHashTable.h"
#include "LinearProbingHashTable.h"
#include "RobinHoodHashTable.h"
#include "CuckooHashTable.h"
#include "Timing.h"
#include "Reporting.h"
#include <iostream>
#include <typeinfo>
using namespace std;

/* Constant: kAllHashFamilies
 * ----------------------------------------------------------------------------
 * A list of all true families of hash functions (that is, hash families
 * where we can sample as many hash functions as we need.) These hash
 * families can be used in any hash table type.
 */
const HashFamilies kAllHashFamilies  = {
  twoIndependentPolynomialHash(),
  threeIndependentPolynomialHash(),
  fiveIndependentPolynomialHash(),
  tabulationHash()
};

/* Constant: kAllHashFunctions
 * ----------------------------------------------------------------------------
 * A list of all types of hash functions available, including families with
 * just a single hash function in them. These are suitable for everything
 * except second-choice hashing and cuckoo hashing.
 */
const HashFamilies kAllHashFunctions = {
  twoIndependentPolynomialHash(),
  threeIndependentPolynomialHash(),
  fiveIndependentPolynomialHash(),
  tabulationHash(),
  identityHash(),
  jenkinsHash()
};

/* Type: TypeList
 * --------------------------------------------------------------------------
 * A C++ typelist. This is a variadic template (a template parameterized over a
 * variable number of arguments) that essentially is used to carry around type
 * information. We'll use it to make lists of all the hash tables we want to
 * test.
 */
template <typename... Types> struct TypeList {};

/* Type: HashTraits
 * ----------------------------------------------------------------------------
 * Type exporting a bunch of information about each hash table type. The top
 * entry on this list contains a collection of default values, and each
 * specialization is tuned to a specific type of hash table.
 */
template <typename HashTable> struct HashTraits {
  /* Human-readable name of the type. By default, we use the typeid operator,
   * which isn't great but works as a fallback.
   */
  static string name() {
    return typeid(HashTable).name();
  }
  
  /* Which set of hash functions this type can run on. By default, we go
   * conservative and assume we can only use infinite families.
   */
  static HashFamilies hashFamilies() {
    return kAllHashFamilies;
  }
  
  /* Which load factors we can use. By default, we go conservative and never
   * use a load factor of above 0.5.
   */
  static vector<double> loadFactors() {
    return { 0.2, 0.3, 0.4, 0.45, 0.47 };
  }
};

template<> struct HashTraits<ChainedHashTable> {
  static string name()                { return "Chained Hashing"; }
  static HashFamilies hashFamilies()  { return kAllHashFunctions; }
  static vector<double> loadFactors() { return { 0.3, 0.5, 0.7, 1.0, 2.0, 3.0 }; }
};
template<> struct HashTraits<SecondChoiceHashTable> {
  static string name()                { return "Second-Choice Hashing"; }
  static HashFamilies hashFamilies()  { return kAllHashFamilies; }
  static vector<double> loadFactors() { return { 0.3, 0.5, 0.7, 1.0, 2.0, 3.0 }; }
};
template<> struct HashTraits<LinearProbingHashTable> {
  static string name()                { return "Linear Probing"; }
  static HashFamilies hashFamilies()  { return kAllHashFunctions; }
  static vector<double> loadFactors() { return { 0.3, 0.5, 0.7, 0.9, 0.99 }; }
};
template<> struct HashTraits<RobinHoodHashTable> {
  static string name()                { return "Robin Hood Hashing"; }
  static HashFamilies hashFamilies()  { return kAllHashFunctions; }
  static vector<double> loadFactors() { return { 0.3, 0.5, 0.7, 0.9, 0.99 }; }
};
template<> struct HashTraits<CuckooHashTable> {
  static string name()                { return "Cuckoo Hashing"; }
  static HashFamilies hashFamilies()  { return kAllHashFamilies; }
  static vector<double> loadFactors() { return { 0.2, 0.3, 0.4, 0.45, 0.47, 0.499 }; }
};

/**
 * This next section introduces the tests that we're going to run.
 *
 * In order to play well with the type system, we've packaged each test as its
 * own type. Each type needs to export a function
 *
 *   template <typename HashTable> static void test()
 *
 * Notice that the test function itself is a template, with the type of hash table
 * to test being specified as a template argument.
 */
struct CorrectnessTests {
  template <typename HashTable> static void test() {
    const size_t kNameLength = 25;
    
    ostringstream header;
    header << "Correctness Tests for " << setw(kNameLength) << left 
           << (HashTraits<HashTable>::name() + ": ");
    cout << header.str()
         << (checkCorrectness<HashTable>(HashTraits<HashTable>::hashFamilies())? "pass" : "fail")
         << endl;
  }
};

struct RuntimeTests {
  template <typename HashTable> static void test() {
    cout << "Timing " << HashTraits<HashTable>::name() << "...\r" << flush;
    printReport("Timing Report for " + HashTraits<HashTable>::name(),
                timeHashTable<HashTable>(HashTraits<HashTable>::hashFamilies(),
                                         HashTraits<HashTable>::loadFactors()));
  }
};

/* This is a recursive function called applyTo that, intuitively, takes in a
 * list of types of tables, then runs the given test case on tables of each of
 * those types.
 *
 * This function is a little different than the recursive functions you've
 * seen in CS106B/X in that it uses compile-time recursion.
 *
 * The argument to this function is a TypeList containing the names of a bunch
 * of different table types to test. For example, we might get a
 * TypeList<LinearProbingHashTable, CuckooHashTable>. The recursion unfolds so that
 *
 *   applyTo(TypeList<LinearProbingHashTable, CuckooHashTable>) calls
 *   applyTo(TypeList<CuckooHashTable>) calls
 *   applyTo(TypeList<>) is the base case.
 *
 * In each recursive step, we apply the testing function to one of the table
 * types.
 */ 
template <typename TestCase> void applyTo(TypeList<>) {
  /* Base case: There are no types to apply the testing function to. */
}

/* Recursive case: There's at least one type to test. */
template <typename TestCase, typename HashTable, typename... HashTables>
void applyTo(TypeList<HashTable, HashTables...>) {
  /* Invoke the testing function on this type */
  TestCase::template test<HashTable>();
  
  /* Recursively invoke applyTo, passing in the tail end of the list of tree
   * types.
   */
  applyTo<TestCase>(TypeList<HashTables...>());
}

/* Recursive function to apply all the testing types given the specified
 * TypeList to the types specified in another TypeList.
 *
 * This function uses compile-time recursion to expand out into calls to
 * runTest invoked on each of the testing types.
 */
template <typename... HashTables> void runTests(TypeList<HashTables...>, TypeList<>) {
  /* Base case: If there are no testing types to run, we don't need to do
   * anything.
   */
}

/* Recursive step: If the list of types to test is nonempty, run tests using
 * the first type, then recursively process the rest.
 */
template <typename... HashTables, typename FirstTest, typename... Tests>
void runTests(TypeList<HashTables...> tables, TypeList<FirstTest, Tests...>) {
  /* Run a test on an object of the first testing type. */
  applyTo<FirstTest>(tables);
  
  /* Recursively process the remaining types. */
  runTests(tables, TypeList<Tests...>());
}

/* Type: AllHashTables
 * ----------------------------------------------------------------------------
 * Typelist representing all the types of hash tables to test. To selectively
 * enable or disable tests, add or remove entries from this list.
 */
using AllHashTables = TypeList<
                        ChainedHashTable,
                        SecondChoiceHashTable,
                        LinearProbingHashTable,
                        RobinHoodHashTable,
                        CuckooHashTable
                      >;

/* Type: AllTests
 * ----------------------------------------------------------------------------
 * Typelist representing all the test cases to run. To selectively enable or
 * disable tests, add or remove entries from this list.
 */
using AllTests = TypeList<
                   CorrectnessTests,
                   RuntimeTests
                 >;
 
int main() {  
  runTests(AllHashTables(), AllTests());
}
