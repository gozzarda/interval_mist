#pragma once

#include <cassert>
#include <ostream>
#include <set>

namespace gozz::interval {

// Interval representing a nonempty inclusive-exclusive range of the natural
// numbers
struct Interval {
  using Coord = uint32_t;

  // upper declared before lower so default comparison is by right endpoint
  Coord upper, lower;

  Interval(Coord lwr, Coord upr);

  // Intervals are canonically ordered by right endpoint, tie-broken by left
  friend auto operator<=>(const Interval &, const Interval &) = default;

  friend std::ostream &operator<<(std::ostream &os, const Interval &val);

  bool intersects(const Interval &rhs);
};

std::set<Interval> random_connected_interval_set(size_t seed, size_t num);

} // namespace gozz::interval