#include "FingerArray.h"
#include "HashTable.h"
#include "PerfectlyBalancedTree.h"
#include "SortedArray.h"
#include "SplayTree.h"
#include "StdSetTree.h"
#include "WeightBalancedTree.h"
#include "Timing.h"
#include <iostream>
#include <cstddef>
#include <iomanip>
using namespace std;

namespace {
  /* Constant: kTreeSize
   * --------------------------------------------------------------------------
   * Size of the trees that will be created for testing purposes. Larger trees
   * have more nodes, but take a little more time to create.
   */
  const size_t kTreeSize = 1u << 18u;

  /* Constant: kNumLookups 
   * --------------------------------------------------------------------------
   * Number of lookups to perform on each of the trees during testing.
   */
  const size_t kNumLookups = kTreeSize << 2u;

  /* Constants: kNumWorkingSets, kWorkingSetSize, kQueriesPerWorkingSet
   * --------------------------------------------------------------------------
   * Constants controlling parameters of the working set tests.
   */
  const size_t kNumWorkingSets = 1 << 6;
  const size_t kWorkingSetSize = 1 << 6;
  const size_t kQueriesPerWorkingSet = 1 << 12;
  
  /* Constant: kMaxDispersal
   * --------------------------------------------------------------------------
   * For the dynamic finger test, the maximum rank difference allowed in
   * moving from one element to the next.
   */
  const size_t kMaxDispersal = 1 << 6;

  /* Type: TypeList
   * --------------------------------------------------------------------------
   * A C++ typelist. This is a variadic template (a template parameterized over a
   * variable number of arguments) that essentially is used to carry around type
   * information. We'll use it to make lists of all the tree types we want to
   * test, of all the test cases we want to run, etc., and we'll use it in
   * conjunction with some template code later on to automatically generate a
   * Cartesian product between types and tests.
   */
  template <typename... Types> struct TypeList {};

  /* Constant: kDoubleWidth
   * ----------------------------------------------------------------------------
   * How many digits are to be used when displaying a double representing a value
   * in milliseconds.
   */
  const size_t kDoubleWidth = 8;

  /* Prints out the name of a tree alongside the given test result. */
  template <typename ResultType>
  void reportResult(const string& name, const ResultType& result) {
    static const size_t kFieldWidth = 25;
    cout << "  " << left << setw(kFieldWidth) << (name + ":")
         << setw(kDoubleWidth) << right << result << endl;
  }

  /**
   * This next section introduces the tests that we're going to run.
   *
   * In order to play well with the type system, we've packaged each test as its
   * own type. Each type needs to export two functions:
   *
   *   static string name(): Return the name of the test.
   *   template <typename Tree> void test(): Run the test on a tree of type Tree.
   *
   * Notice that the test function itself is a template, with the type of tree
   * to test being specified as a template argument.
   */
  struct CorrectnessTests {
    static string name() { return "Correctness Tests"; }
    template <typename Tree> void test() {
      reportResult(Tree::name(), checkCorrectness<Tree>(kTreeSize, kNumLookups)? "Pass" : "Fail");
    }
  };
  struct SequentialAccessTests {
    static string name() { return "Sequential Access"; }
    template <typename Tree> void test() {
      reportResult(Tree::name(), timeSequential<Tree>(kTreeSize));
    }
  };
  struct ReverseSequentialAccessTests {
    static string name() { return "Reverse Sequential Access"; }
    template <typename Tree> void test() {
      reportResult(Tree::name(), timeReverseSequential<Tree>(kTreeSize));
    }
  };
  struct WorkingSetTests {
    static string name() { return "Accessing Randomly-Chosen Working Sets"; }
    template <typename Tree> void test() {
      reportResult(Tree::name(), timeWorkingSets<Tree>(kTreeSize, kNumWorkingSets,
                                                       kWorkingSetSize, kQueriesPerWorkingSet));
    }
  };
  struct DynamicFingerTests {
    static string name() { return "Accessing Elements with Low Rank Differences"; }
    template <typename Tree> void test() {
      reportResult(Tree::name(), timeDynamicFinger<Tree>(kTreeSize, kNumLookups, kMaxDispersal));
    }
  };
  struct UniformDistributionTests {
    uniform_int_distribution<int> distribution{ 0, kTreeSize - 1 };
    static string name() { return "Accessing Elements With a Uniform Distribution"; }
    template <typename Tree> void test() {
      reportResult(Tree::name(), timeDistribution<Tree>(distribution, kNumLookups));
    }
  };

  /* This one is a bit weird. We'd like to be able to run tests on Zipfian
   * distributions, which are parameterized over some real number z. The larger
   * z gets, the more skewed the data are.
   *
   * The problem is that we need to embed the parameter z into the type system,
   * and C++ doesn't let you parameterize templates over doubles. Instead, we'll
   * have the user provide us 1000z, rounded to an integer, and then do the
   * division to undo it here.
   */
  template <int KiloZ> struct ZipfianTests {
    discrete_distribution<int> distribution{ zipfian(kTreeSize, KiloZ / 1000.0) };
    static string name() {
      ostringstream result;
      result << "Accessing Elements with A Zipf("
             << (KiloZ / 1000) << "." 
             << setw(3) << setfill('0') << (KiloZ % 1000)
             << ") Distribution";
      return result.str();
    }
    template <typename Tree> void test() {
      reportResult(Tree::name(), timeDistribution<Tree>(distribution, kNumLookups));
    }
  };

  /* This is a recursive function called applyTo that, intuitively, takes in a
   * list of types of trees, then runs the given test case on trees of each of
   * those types.
   *
   * This function is a little different than the recursive functions you've
   * seen in CS106B/X in that it uses compile-time recursion.
   *
   * The argument to this function is a TypeList containing the names of a bunch
   * of different tree types to test. For example, we might get a
   * TypeList<WeightBalancedTree, SplayTree>. The recursion unfolds so that
   *
   *   applyTo(TypeList<WeightBalancedTree, SplayTree>, tester) calls
   *   applyTo(TypeList<SplayTree>,                     tester) calls
   *   applyTo(TypeList<>,                              tester) is the base case.
   *
   * In each recursive step, we apply the testing function to one of the tree
   * types.
   */ 
  template <typename Tester> void applyTo(TypeList<>, Tester&) {
    /* Base case: There are no types to apply the testing function to. */
  }

  /* Recursive case: There's at least one tree type to test. */
  template <typename Tree, typename... Trees, typename Tester>
  void applyTo(TypeList<Tree, Trees...>, Tester& tester) {
    /* Invoke the testing function on this tree type. Because Tester is a
     * template type and we're instantiating a nested template in it, we have to
     * bring in a rarely-used instance of the template keyword in which we say
     * "this use of angle braces represents a template instantiation, not a
     * comparison."
     */
    tester.template test<Tree>();
    
    /* Recursively invoke applyTo, passing in the tail end of the list of tree
     * types.
     */
    applyTo(TypeList<Trees...>(), tester);
  }

  /* Runs a given testing object on a collection of different tree types. The
   * types are specified via a TypeList.
   */
  template <typename... Trees, typename Tester>
  void runTest(TypeList<Trees...> trees, Tester tester) {
    cout << Tester::name() << endl;
    applyTo(trees, tester);
    cout << endl;
  }

  /* Recursive function to apply all the testing types given the specified
   * TypeList to the tree types specified in another TypeList.
   *
   * This function uses compile-time recursion to expand out into calls to
   * runTest invoked on each of the testing types.
   */
  template <typename... Trees> void runTests(TypeList<Trees...>, TypeList<>) {
    /* Base case: If there are no testing types to run, we don't need to do
     * anything.
     */
  }

  /* Recursive step: If the list of types to test is nonempty, run tests using
   * the first type, then recursively process the rest.
   */
  template <typename... Trees, typename FirstTest, typename... Tests>
  void runTests(TypeList<Trees...> trees, TypeList<FirstTest, Tests...>) {
    /* Run a test on an object of the first testing type. */
    runTest(trees, FirstTest());
    
    /* Recursively process the remaining types. */
    runTests(trees, TypeList<Tests...>());
  }

  /* Type: AllTrees
   * ----------------------------------------------------------------------------
   * A TypeList representing all the tree types that we'll be running tests on.
   * To enable or disable testing of certain tree types, just change the contents
   * of this list.
   */
  using AllTrees = TypeList<
                     PerfectlyBalancedTree,
                     WeightBalancedTree,
                     SplayTree,
                     StdSetTree,
                     SortedArray,
                     FingerArray,
                     HashTable
                   >;

  /* Type: AllTests
   * ----------------------------------------------------------------------------
   * A TypeList representing all of the test cases we want to run. To enable or
   * disable test cases, edit this type list to add or remove them.
   */
  using AllTests = TypeList<
                     CorrectnessTests,
                     UniformDistributionTests,
                     SequentialAccessTests,
                     ReverseSequentialAccessTests,
                     WorkingSetTests,
                     DynamicFingerTests,
                     ZipfianTests< 500>,
                     ZipfianTests< 750>,
                     ZipfianTests<1000>,
                     ZipfianTests<1200>,
                     ZipfianTests<1300>
                   >;
}

int main() {
  /* Run all the tests we have on all the types of trees we care about. */
  runTests(AllTrees(), AllTests());
}
