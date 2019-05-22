/**
 * Interactive program to let the user play around with the hash tables
 * directly. The original version of this program was written by Wanqi Zhu,
 * a student in CS166 in Spring 2018. This version has been edited and modified
 * by Keith Schwarz.
 */
#include "ChainedHashTable.h"
#include "CuckooHashTable.h"
#include "LinearProbingHashTable.h"
#include "RobinHoodHashTable.h"
#include "SecondChoiceHashTable.h"
#include "Hashes.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <cctype>
using namespace std;

namespace {
  /* Prompts the user for a string, returning whatever they typed in. */
  string getLine(const string& prompt) {
    cout << prompt;
    
    string result;
    getline(cin, result);
    return result;
  }

  /* Converts a string to an integer, reporting an error if the conversion
   * can't be done.
   *
   * Because our hash tables are never supposed to get negative inputs, we will
   * treat any negative number as a failure case.
   */
  int toInt(const string& str) {
    istringstream converter(str);
    
    int result;
    if (converter >> result, !converter) {
      throw runtime_error("Couldn't interpret that as an integer.");
    }
    
    /* Make sure there's nothing left. */
    for (char ch; converter >> ch; ) throw runtime_error("Couldn't interpret that as an integer.");
    
    /* Make sure the value was nonnegative. */
    if (result < 0) throw runtime_error("Integer must be nonnegative.");
    
    return result;
  }


  /* Prompts the user for an int. */
  int readInt(const string& prompt = "Enter an integer: ") {
    while (true) {
      try {
        string line = getLine(prompt);
        return toInt(line);
      } catch (const exception& e) {
        cout << e.what() << endl;
      }
    }
  }

  /* Interactive environment to play around with the different hash tables. */
  template <typename HashTable> void exploreHashTable(shared_ptr<HashFamily> family) {
    int size = readInt("Enter hash table size: ");
    while (size <= 1) {
      cout << "  For safety, the minimum hash table size is 2." << endl;
      size = readInt("Enter hash table size: ");
    }
    
    cout << "Building hashtable... " << flush;
    HashTable table(size, family);
    cout << "done!" << endl << endl;
    
    cout << "You are now in a REPL-type environment where you can issue individual" << endl;
    cout << "commands to your hash table. To execute a command, type the name of that" << endl;
    cout << "command, then a space, then the argument. Acceptable commands are" << endl;
    cout << endl;
    cout << "                   insert     remove     contains" << endl;
    cout << endl;
    cout << "You can use the first letter of a command as a shorthand for it." << endl;
    cout << "Type Q or enter a blank line to quit." << endl;
    
    /* Let the student make some queries! */
    while (true) {
      try {
        string command = getLine("  Enter command: ");
        if (command.size() == 0 || tolower(command[0]) == 'q') break;
        
        /* Break the command apart into a method call and a command. */
        istringstream extractor(command);
        string method, argument;
        extractor >> method >> argument;
        
        /* Confirm we read something and that there isn't anything left over. */
        for (string leftover; !extractor || extractor >> leftover; ) {
          throw runtime_error("Sorry, I couldn't understand that.");
        }
        
        if (tolower(method[0]) == 'i') {
          table.insert(toInt(argument));
          cout << "    Called table.insert(" << toInt(argument) << ")." << endl;
        } else if (tolower(method[0]) == 'r') {
          table.remove(toInt(argument));
          cout << "    Called table.remove(" << toInt(argument) << ")." << endl;
        } else if (tolower(method[0]) == 'c') {
          auto result = table.contains(toInt(argument));
          cout << "    table.contains(" << toInt(argument) << ") returned "
               << boolalpha << result << "." << endl;
        } else throw runtime_error("Sorry, I couldn't understand that.");
        
      } catch (const exception& e) {
        cout << "    " << e.what() << endl;
      }
    }
      
    cout << endl;
  }
  
  /* Prompts the user to choose a hash table type. Since the return type is a
   * pointer-to-function type with a gnarly signature, we're just going to have
   * the compiler figure it out for us. :-)
   */
  auto userChooseHashTable() {
    cout << "Please select the hash table you'd like to explore." << endl;
    cout << "  (C)hained Hashing" << endl;
    cout << "  (S)econd Choice Hashing" << endl;
    cout << "  (L)inear Probing" << endl;
    cout << "  (R)obin Hood Hashing" << endl;
    cout << "  C(U)ckoo Hashing" << endl;
    cout << "Or just hit ENTER to quit." << endl;
    while (true) {
      string input = getLine("Your choice: ");
      
      if (input == "" || tolower(input[0]) == 'q') exit(0);
            
      if (tolower(input[0]) == 'c') return exploreHashTable<ChainedHashTable>;
      if (tolower(input[0]) == 's') return exploreHashTable<SecondChoiceHashTable>;
      if (tolower(input[0]) == 'l') return exploreHashTable<LinearProbingHashTable>;
      if (tolower(input[0]) == 'r') return exploreHashTable<RobinHoodHashTable>;
      if (tolower(input[0]) == 'u') return exploreHashTable<CuckooHashTable>;

      
      cout << "  Sorry, I didn't understand that." << endl;
    }
  }
  
  /* Prompts the user for a hash family. */
  auto userChooseHashFamily() {
    cout << "Please select the hash family you'd like to use." << endl;
    cout << "  (2)-Independent Polynomial Hashing" << endl;
    cout << "  (3)-Independent Polynomial Hashing" << endl;
    cout << "  (5)-Independent Polynomial Hashing" << endl;
    cout << "  (T)abulation Hashing" << endl;
    cout << "  (I)dentity Hash Function" << endl;
    cout << "  (J)enkins Hashing" << endl;
    
    while (true) {
      string input = getLine("Your choice: ");

      if (input == "2")                 return twoIndependentPolynomialHash();
      if (input == "3")                 return threeIndependentPolynomialHash();
      if (input == "5")                 return fiveIndependentPolynomialHash();
      if (input == "T" || input == "t") return tabulationHash();
      if (input == "I" || input == "i") return identityHash();
      if (input == "J" || input == "j") return jenkinsHash();
      
      cout << "  Sorry, I didn't understand your hash family choice." << endl;
    }
  }
}

int main() {
  while (true) {
    userChooseHashTable()(userChooseHashFamily());
  }
}
