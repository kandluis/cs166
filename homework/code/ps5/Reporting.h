#ifndef Reporting_Included
#define Reporting_Included

#include "Timing.h"
#include <string>

/* Function: printReport
 * ----------------------------------------------------------------------------
 * Prints out a human-readable report containing the results of running a
 * collection of tests.
 */
void printReport(const std::string& title, const ResultsByHash& results);

#endif
