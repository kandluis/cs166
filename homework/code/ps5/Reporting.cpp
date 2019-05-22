#include "Timing.h"
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <set>
#include <algorithm>
using namespace std;

namespace {
  /* Given a list of results keyed by hash functions, returns the length of the
   * longest-named hash in the bunch.
   */
  size_t longestNameIn(const ResultsByHash& result) {
    return max_element(result.begin(), result.end(),
                       [](const auto& lhs, const auto& rhs) {
                         return lhs.first.size() < rhs.first.size();
                       })->first.size();
  }

  /* Given a list of results keyed by hash functions, returns a set of all the
   * load factors used.
   */
  set<double> loadFactorsIn(const ResultsByHash& results) {
    set<double> result;
    for (const auto& hashEntry: results) {
      for (const auto& entry: hashEntry.second) {
        result.insert(entry.first);
      }
    }
    return result;
  }

  /* Produces a padded, human-readable representation of a load factor. We will
   * use one leading digit and three decimal points.
   */
  string loadToString(double loadFactor) {
    ostringstream result;
    result << setw(5) << fixed << setprecision(3) << loadFactor;
    return result.str();
  }

  /* Produces a padded, human-readable representation of a nanosecond timing
   * result. We will use no decimal places.
   */
  string runtimeToString(double time) {
    ostringstream result;
    result << fixed << setprecision(0) << time;
    return result.str();
  }

  /* Returns the length of the longest string needed to display the runtime of
   * any operation.
   */
  size_t longestRuntimeIn(const ResultsByHash& results) {
    size_t result = 0;
    
    for (const auto& hashResults: results) {
      for (const auto& loadResults: hashResults.second) {
        result = max(result, runtimeToString(loadResults.second.insertTime).size());
        result = max(result, runtimeToString(loadResults.second.successfulQueryTime).size());
        result = max(result, runtimeToString(loadResults.second.unsuccessfulQueryTime).size());
      }
    }
    
    return result;
  }

  /* Prints out a separator between two table rows. */
  void printRowSeparator(size_t hashNameWidth, size_t columnWidth, size_t numCols) {
    cout << setw(hashNameWidth) << "" // Skip hash function name
         << " "                       // Space after hash function name
         << "+";                      // Table open
    for (size_t i = 0; i < numCols; i++) {
      cout << string(columnWidth, '-') << "+";
    }
    cout << endl;
  }

  /* Prints out a row of a table. */
  void printTableRow(size_t hashNameWidth, size_t columnWidth,
                     const string& rowTitle,
                     const vector<string>& contents,
                     char  colSeparator) {
    cout << setw(hashNameWidth) << rowTitle << " " << colSeparator;
    
    for (const auto& entry: contents) {
      cout << setw(columnWidth) << entry << colSeparator;
    }
    cout << endl;
  }

  /* Given a time in ns, produces a nice, human-readable representation of that
   * time.
   */
  string prettyTime(double time, size_t width, char prefix) {
    ostringstream result;
    result << prefix << ": " << setw(width) << runtimeToString(time) << "ns ";
    return result.str();
  }
  
  /* Given a string, prints that string as the title for the overall table. */
  void printTableTitle(size_t tableWidth, const string& title) {
    cout << string(tableWidth, '#') << endl;
    
    size_t spaceToFront = (tableWidth - title.size()) / 2;
    
    cout << '#' << setw(spaceToFront - 1) << ""
         << title
         << setw(tableWidth - spaceToFront - 1 - title.size()) << "" << '#' << endl;
    
    cout << string(tableWidth, '#') << endl;
  }
}

/* Prints out a nice, human-readable presentation of the results from a set of
 * time trials.
 */
void printReport(const string& title, const ResultsByHash& results) {

  /* Find the length of the longest name in the set of hash families so we know
   * how much to indent by.
   */
  size_t hashNameWidth = longestNameIn(results);
  
  /* Aggregate all the different load factors. We  assume that all load
   * factors will have been tried across all hash functions.
   */
  auto loadFactors = loadFactorsIn(results);
  
  /* Find the length of the longest time result and use that for our width. */
  size_t runtimeWidth = longestRuntimeIn(results);
  
  /* Size of one column is the width of a runtime element, plus space for
   * decorations
   *
   *    | I: ----ns |
   *     ^^^^    ^^^ 
   *
   */
  size_t columnWidth = runtimeWidth + 7;
  
  /* Print the overall table header. */
  size_t totalWidth = hashNameWidth 
                      + 2            // Space to table, first column
                      + (columnWidth + 1) * loadFactors.size();
  printTableTitle(totalWidth, title);
  
  
  /* Print the title bar. */
  vector<string> loads;
  for (auto load: loadFactors) {
    loads.push_back(loadToString(load));
  }
  printTableRow(hashNameWidth, columnWidth,
                "", loads,
                ' ');
  
  printRowSeparator(hashNameWidth, columnWidth, loadFactors.size());

  /* Print out the results from each hash function. */
  for (const auto& hashResults: results) {
    vector<string> insertTimes;
    vector<string> successfulQueryTimes;
    vector<string> unsuccessfulQueryTimes;
    
    for (const auto& loadResults: hashResults.second) {
      insertTimes.push_back(prettyTime(loadResults.second.insertTime,
                                       runtimeWidth, 'I'));
      successfulQueryTimes.push_back (prettyTime(loadResults.second.successfulQueryTime,
                                                 runtimeWidth, 'H'));
      unsuccessfulQueryTimes.push_back (prettyTime(loadResults.second.unsuccessfulQueryTime,
                                         runtimeWidth, 'M'));
    }
    
    printTableRow(hashNameWidth, columnWidth,
                  hashResults.first, insertTimes, '|');
    printTableRow(hashNameWidth, columnWidth,
                  "",                successfulQueryTimes,  '|');
    printTableRow(hashNameWidth, columnWidth,
                  "",                unsuccessfulQueryTimes,  '|');
    
    printRowSeparator(hashNameWidth, columnWidth, loadFactors.size());
  }
  
  cout << endl;
  cout << endl;
}
