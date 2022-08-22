#include "interval.hpp"

#include <random>
#include <vector>

namespace interval_mist::interval {

// TODO: Consider implementing hash and using unordered maps and sets where
// appropriate template<> struct std::hash<Interval> {
//   std::size_t operator()(const Interval &interval) const noexcept {
//     const uint64_t upr = static_cast<uint64_t>(interval.upper);
//     const uint64_t lwr = static_cast<uint64_t>(interval.lower);
//     return std::hash<uint64_t>(upr << 32 | lwr);
//   }
// }

using Coord = Interval::Coord;

Interval::Interval(Coord lwr, Coord upr) : lower(lwr), upper(upr) {
  assert(lower <= upper);
}

std::ostream &operator<<(std::ostream &os, const Interval &val) {
  os << '[' << val.lower << ", " << val.upper << ')';
  return os;
}

bool Interval::intersects(const Interval &rhs) {
  return lower <= rhs.upper && rhs.lower <= upper;
}

std::set<Interval> random_connected_interval_set(size_t seed, size_t num) {
  std::default_random_engine rng(seed);
  std::uniform_int_distribution<uint8_t> bool_dist(0, 1);

  std::vector<Coord> open;
  std::set<Interval> is;
  Coord limit = 2 * num;
  for (Coord coord = 0; coord < limit; ++coord) {
    bool must_close = (limit - coord) == open.size();
    bool must_open = open.size() <= 1 && !must_close;
    bool do_open = must_open || (!must_close && bool_dist(rng));
    if (do_open) {
      open.push_back(coord);
    } else {
      auto it = open.begin() +
                std::uniform_int_distribution<size_t>(0, open.size() - 1)(rng);
      is.emplace(*it, coord);
      open.erase(it);
    }
  }

  return is;
}

} // namespace interval_mist::interval