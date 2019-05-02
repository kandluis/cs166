/******************************************************************************
 * File: DynamicOverlap.h
 *
 * Data structure for the dynamic overlap problem. This data structure supports
 * insertions and deletions of intervals, along with queries of the form "how
 * many intervals is this particular point contained in?"
 */
#ifndef DynamicOverlap_Included
#define DynamicOverlap_Included

#include <cstddef> // For std::size_t

// The DynamicOverlap is based on the provided Treap implementation. We simply
// augment the trip to handle intervals (including duplicates, using counts).
//
// The intervals themselves form the keys (using standard tuple ordering).
//
// To handle efficient overlap checks, we augment the tree to have each node track
// the maximum endpoint of any intervals contained therein. This allows us to do
// intervalsContaining in O(log n) time. Similarly, insert and remove are expected
// O(log n) time.
class DynamicOverlap {
public:
  /**
   * Constructs a new, empty data structure.
   */
  DynamicOverlap();

  /**
   * Frees all memory used by this data structure.
   */
  ~DynamicOverlap();

  /**
   * Inserts another copy of the specified interval into this data structure.
   *
   * You can assume that start <= end and don't need to handle the case where
   * this isn't true. There may already be a copy of this interval in the
   * data structure, in which case you should insert another copy.
   */
  void insert(int start, int end);

  /**
   * Removes one copy of the specified interval from this data structure.
   *
   * You can assume that remove() will not be called on an interval that isn't
   * currently contained in the data structure.
   */
  void remove(int start, int end);

  /**
   * Returns the number of intervals that contain the specified integer.
   */
  std::size_t intervalsContaining(int key) const;

private:
  // An closed interval [start, end].
  struct Interval {
    int start;
    int end;

    Interval(int start, int end): start(start), end(end) {}

    // Alow arithmetic on operators. We induce a complete ordering.
    bool operator== (const Interval& other) const {
      return this->start == other.start && this->end == other.end;
    }
    bool operator!= (const Interval& other) const {
      return !(*this == other);
    }
    bool operator< (const Interval& other) const {
      return (this->start < other.start ||
              (this->start == other.start && this->end < other.end));
    }
    bool operator<= (const Interval& other) const {
      return *this < other || *this == other;
    }
    bool operator> (const Interval& other) const {
      return !(*this <= other);
    }
    bool operator>= (const Interval& other) const {
      return !(*this < other);
    }

  };
  struct Node {
    // The interval itself. The interval is the key, with a total ordered
    // defined by ordering by start first, then by end.
    Interval interval;
    // An extra annotation in the node specifying the maximum endpoints of
    // all intervals in this tree.
    int maxEndpoint;
    // A counter for duplicate intervals in the tree.
    int count;
    // The random weight assigned to it
    double weight;

    // Child pointers. Not owned.
    Node* left;
    Node* right;

    // Parent pointer. This is used to simplify the insertion
    // procedure's "keep rotating with your parent" logic.
    // Not owned.
    Node* parent;

    // Convenience constructor.
    Node(int start, int end, double weight, Node* parent)
      : interval(start, end), maxEndpoint(end), count(1), weight(weight), parent(parent) {
    }
  };

  Node* root = nullptr;

  /* Rotates a node with its parent. */
  void rotateWithParent(Node* curr);

  /* Assumes the maxEndpoint for node is wrong and recompute it based on children
   * and the interval owned by node.
   */
  int getMaxEndpoint(const Node& node);

  // A recursive helper to make membership queries simpler.
  std::size_t intervalsContainingRecursive(const Node* node, const int key) const;

  /* Disallow copying. */
  DynamicOverlap(const DynamicOverlap &) = delete;
  void operator= (DynamicOverlap) = delete;
};


#endif
