#include "graph.hpp"
#include "interval.hpp"
#include "solvers.hpp"
#include "tester.hpp"

#include <bits/stdc++.h>

namespace interval_mist {

using Interval = interval::Interval;
using Graph = graph::Graph;

void fuzz_greedy_vs_naive() {
  tester::Solver lhs = solvers::greedy::interval_mist_greedy;
  tester::Solver rhs = solvers::naive::interval_mist_naive;
  size_t num_tests = 100;
  size_t seed = 126835921;
  size_t num_verts = 10;

  auto result =
      tester::fuzz_compare_solvers(lhs, rhs, num_tests, seed, num_verts);
  if (result) {
    result.value().report(std::cout, "GREEDY", "NAIVE");
  }
}

void pc_eq_mist_counterexample() {
  std::set<Interval> vs = {
      Interval(0, 2),  Interval(1, 6), Interval(3, 4),
      Interval(5, 10), Interval(7, 8), Interval(9, 11),
  };
  auto g = Graph::interval_graph_from_set(vs);

  tester::Solver lhs = solvers::path_cover::interval_mist_path_cover;
  tester::Solver rhs = solvers::naive::interval_mist_naive;

  assert(!tester::compare_solvers(lhs, rhs, g).agree());
}

} // namespace interval_mist

int main() {
  interval_mist::pc_eq_mist_counterexample();

  interval_mist::fuzz_greedy_vs_naive();

  return 0;
}