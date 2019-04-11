/******************************************************************************
 * File: RMQEntry.h
 *
 * Header defining the RMQEntry type. This is a type that can be stored in a
 * range minimum query data structure. The type just wraps an integer and is
 * designed so that you can't accidentally convert integers representing indices
 * to integers representing values.
 *
 * You can compare RMQEntries using the built-in relational operators. If you
 * really want to get the underlying value out, you can do that as well, though
 * you probably shouldn't need to do this.
 */
#ifndef RMQEntry_Included
#define RMQEntry_Included

#include <cstdint> // For std::int34_t

class RMQEntry {
public:
  /**
   * Constructs a new RMQEntry that wraps an integer value. This constructor
   * is marked explicit so that you can't implicitly convert an integer into
   * an entry.
   */
  explicit RMQEntry(std::int32_t value = 0) : mValue(value) {
    // Handled in initializer list
  }

  /**
   * Retrieves the underlying value.
   */
  std::int32_t value() const {
    return mValue;
  }

private:
  std::int32_t mValue;
};

/* Comparison operators */
inline bool operator <  (const RMQEntry& lhs, const RMQEntry& rhs) {
  return lhs.value() <  rhs.value();
}
inline bool operator <= (const RMQEntry& lhs, const RMQEntry& rhs) {
  return lhs.value() <= rhs.value();
}
inline bool operator == (const RMQEntry& lhs, const RMQEntry& rhs) {
  return lhs.value() == rhs.value();
}
inline bool operator != (const RMQEntry& lhs, const RMQEntry& rhs) {
  return lhs.value() != rhs.value();
}
inline bool operator >= (const RMQEntry& lhs, const RMQEntry& rhs) {
  return lhs.value() >= rhs.value();
}
inline bool operator >  (const RMQEntry& lhs, const RMQEntry& rhs) {
  return lhs.value() >  rhs.value();
}

#endif
