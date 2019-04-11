#include "FastestRMQ.h"
#include "FischerHeunRMQ.h"
#include "HybridRMQ.h"
#include "PrecomputedRMQ.h"
#include "SegmentTreeRMQ.h"
#include "SparseTableRMQ.h"
#include "RMQEntry.h"
#include "Timer.h"
#include <iostream>
#include <vector>
#include <stdexcept>
#include <algorithm>
#include <numeric>
#include <iterator>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <random>
#include <functional>
#include <cctype>
#include <sstream>
#include <memory>
using namespace std;

namespace {
  /* Aborts with a nice error message. */
  [[ noreturn ]] void abortProgram() {
    cout << "Run this program under gdb and backtrace for more information." << endl;
    abort();
  }
  
  /* Adds commas to a numeric value to make it easier to read. */
  template <typename Integer> string addCommasTo(Integer n) {
    /* Negative numbers don't play well with mods. */
    if (n < 0) return "-" + addCommasTo(-n);
    
    /* Build up our target string, but in reverse. */
    string result;
    
    /* Handle all but the first digit, which eliminates an edge case of having
     * a comma at the front.
     */
    for (size_t i = 0; n >= 10; i++) {
      result += static_cast<char>('0' + n % 10);
      n /= 10;
      
      if (i % 3 == 2) result += ',';
    }
    
    /* Handle the first digit. */
    result += static_cast<char>('0' + n);
    
    /* Reverse and return the result. */
    reverse(result.begin(), result.end());
    return result;
  }
  
  /* Master set of all possible command-line switches. */
  const unordered_set<string> kAllSwitches = {
    "-rmq", "-seed", "-output"
  };
  
  /* Type representing something that can print information about how tests are going. */
  class Printer {
  public:
    virtual ~Printer() = default;
  
    virtual void startTest(size_t numElems, size_t numBuilds, size_t numQueries) = 0;
    virtual void reportResult(size_t buildTime, size_t queryTime) = 0;
  };
  
  /* Default printer. */
  class PrettyPrinter: public Printer {
  public:
    void startTest(size_t numElems, size_t numBuilds, size_t numQueries) override {
      cout << "Testing size " << addCommasTo(numElems)
           << " (" << addCommasTo(numBuilds) << " builds, "
           << addCommasTo(numQueries) << " queries / build)" << endl;
    }
    
    virtual void reportResult(size_t buildTime, size_t queryTime) override {
      cout << "  Mean build time: " << addCommasTo(buildTime) << " ns" << endl;
      cout << "  Mean query time: " << addCommasTo(queryTime) << " ns" << endl;
    }
  };
  
  /* CSV printer. */
  class CSVPrinter: public Printer {
  public:
    CSVPrinter() {
      cout << "Elements,Mean Build Time,Mean Query Time" << endl;
    }
  
    void startTest(size_t numElems, size_t numBuilds, size_t numQueries) override {
      cout << numElems << flush;
    }
    
    virtual void reportResult(size_t buildTime, size_t queryTime) override {
      cout << "," << buildTime << "," << queryTime << endl;
    }
  };
  
  
  /* Type representing arguments to the test driver. */
  struct TestParameters {
    size_t seed;
    shared_ptr<Printer> printer;
  };
  
  /* Tests and reports timing information about the specifed RMQ structure. */
  template <typename RMQ> void runTests(size_t min, size_t max, size_t step,
                                        size_t numBuilds, size_t numQueries,
                                        const TestParameters& params) {
    /* We need a random source in order to produce values. */
    mt19937 generator(params.seed);
    
    for (size_t numElems = min; numElems <= max; numElems += step) {
      params.printer->startTest(numElems, numBuilds, numQueries);
    
      Timer buildTimer, queryTimer;
      uniform_int_distribution<size_t> dist(0, numElems - 1);
      
      /* For efficiency, only make one array, and then keep repeatedly filling it in. */
      vector<RMQEntry> data(numElems);
      
      for (size_t build = 0; build < numBuilds; build++) {
        /* Fill our vector with a bunch of random elements. */
        for (size_t i = 0; i < numElems; i++) {
          data[i] = RMQEntry(dist(generator));
        }
        
        /* Our reference answer. */
        SegmentTreeRMQ answer(data.data(), data.size());
        
        /* The answer being tested. */
        buildTimer.start();
        RMQ tested(data.data(), data.size());
        buildTimer.stop();
        
        /* Pummel it with queries. */
        for (size_t query = 0; query < numQueries; query++) {
          /* Pick two points to demarcate the range. */
          size_t low  = dist(generator);
          size_t high = dist(generator);
          if (low > high) swap(low, high);
          
          /* We use open intervals, so high needs to get bumped up a bit. */
          high++;
          
          /* See what answers we get back. */
          size_t ours = answer.rmq(low, high);
          
          queryTimer.start();
          size_t theirs = tested.rmq(low, high);
          queryTimer.stop();
          
          /* Confirm the answer is in range. */
          if (theirs >= numElems) {
            cerr << "Error: query produced an answer that was out of bounds." << endl;
            abortProgram();
          }
          
          /* Confirm that the answers match. */
          if (data[ours] != data[theirs]) {
            cerr << "Error: query produced the wrong answer." << endl;
            abortProgram();
          }
        }
      }
      
      /* Report statistics. */
      params.printer->reportResult(buildTimer.elapsed() / numBuilds, queryTimer.elapsed() / (numQueries * numBuilds));
    }                                                                        
  }
  
  /* Tests the specified RMQ data structure on a variety of inputs, checking the results produced. */
  template <typename RMQ> void testRMQ(const TestParameters& params) {
    /*             min     max     step  builds queries */
    runTests<RMQ>(     1,     25,      1, 10000,    100, params);
    runTests<RMQ>(  1000,   5000,   1000, 1000,   10000, params);
    runTests<RMQ>(100000, 500000, 100000, 5,    1000000, params);
    cout << "All tests completed!" << endl;
  }

  /* Parses the command-line arguments by building a map from flags to values. */
  auto parseArguments(int argc, const char* argv[]) {
    unordered_map<string, string> result;
    
    for (int i = 1; i < argc; i += 2) {
      string flag = argv[i];
      if (!kAllSwitches.count(flag)) throw runtime_error("Unknown command-line switch: " + flag);
      
      if (i + 1 == argc) throw runtime_error("Command line switch " + flag + " missing argument.");
      
      if (result.count(flag)) throw runtime_error("Duplicate command-line switch: " + flag);
      
      result[flag] = argv[i + 1];
    }
    
    return result;
  }
  
  /* Converts a string to lower-case. */
  string toLowerCase(string input) {
    transform(input.begin(), input.end(), input.begin(), ::tolower);
    return input;
  }
  
  /* Picks which test function to run. */
  function<void (const TestParameters&)> selectTestFunction(const unordered_map<string, string>& args) {
    if (!args.count("-rmq")) throw runtime_error("No RMQ type selected. Use the syntax -rmq ClassName to choose an RMQ type.");
    
    string rmqType = toLowerCase(args.at("-rmq"));
    
    if (rmqType == "fastestrmq")     return &testRMQ<FastestRMQ>;
    if (rmqType == "fischerheunrmq") return &testRMQ<FischerHeunRMQ>;
    if (rmqType == "hybridrmq")      return &testRMQ<HybridRMQ>;
    if (rmqType == "precomputedrmq") return &testRMQ<PrecomputedRMQ>;
    if (rmqType == "sparsetablermq") return &testRMQ<SparseTableRMQ>;
    if (rmqType == "segmenttreermq") return &testRMQ<SegmentTreeRMQ>;
    
    throw runtime_error("Unrecognized RMQ type: " + args.at("-rmq") + ". (Check your spelling?)");
  }
  
  /* Converts a string to a size_t. */
  size_t stringToSizeT(const string& input) {
    istringstream converter(input);
    
    size_t result;
    char leftover;
    if (!(converter >> result) || converter >> leftover) throw runtime_error("Could not parse \"" + input + "\" as a size_t.");
    
    return result;
  }
  
  /* Sets up the test arguments. */
  TestParameters selectTestParameters(const unordered_map<string, string>& args) {
    TestParameters result;
    
    /* Set the random seed. */
    result.seed = args.count("-seed")? stringToSizeT(args.at("-seed")) : 0;
    
    /* Set the printer. */
    if (args.count("-output")) {
      if      (args.at("-output") == "default") result.printer = make_shared<PrettyPrinter>();
      else if (args.at("-output") == "csv")     result.printer = make_shared<CSVPrinter>();
      else throw runtime_error("Unknown output type: \"" + args.at("-output") + "\"");
    } else {
      result.printer = make_shared<PrettyPrinter>();
    }
    
    return result;
  }
}

int main(int argc, const char* argv[]) try {
  auto args = parseArguments(argc, argv);
  
  auto testFn    = selectTestFunction(args);
  auto testArgs  = selectTestParameters(args);
  
  testFn(testArgs);
} catch (const exception& e) {
  cout << "Error: " << e.what() << endl;
  return -1;
}
