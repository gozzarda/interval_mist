#include "graph.hpp"
#include "graph/tree_transform.hpp"
#include "interval.hpp"
#include "solvers.hpp"
#include "tester.hpp"

#include <iostream>
#include <random>
#include <set>

namespace interval_mist {

using Interval = interval::Interval;
using Graph = graph::Graph;
using Vertex = Graph::Vertex;
using Edge = Graph::Edge;

void fuzz_greedy_vs_naive() {
  std::cerr << "Fuzz testing greedy solver vs naive" << std::endl;

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

void fuzz_greedy_vs_dp() {
  std::cerr << "Fuzz testing greedy solver vs dp" << std::endl;

  tester::Solver lhs = solvers::greedy::interval_mist_greedy;
  tester::Solver rhs = solvers::dp::interval_mist_dp;
  size_t num_tests = 5000;
  size_t seed = 283947130;
  size_t num_verts = 16;

  auto result =
      tester::fuzz_compare_solvers(lhs, rhs, num_tests, seed, num_verts);
  if (result) {
    result.value().report(std::cout, "GREEDY", "DP");
  }
}

void pc_eq_mist_counterexample() {
  std::cerr << "Verifying counterexample that min path cover does not solve MIST" << std::endl;

  std::set<Interval> vs = {
      Interval(0, 2),  Interval(1, 6), Interval(3, 4),
      Interval(5, 10), Interval(7, 8), Interval(9, 11),
  };
  auto g = Graph::interval_graph_from_set(vs);

  tester::Solver lhs = solvers::path_cover::interval_mist_path_cover;
  tester::Solver rhs = solvers::naive::interval_mist_naive;

  assert(!tester::compare_solvers(lhs, rhs, g).agree());
}

// TODO: Refactor experiments into separate files?
void validate_lre_leaf_transform() {
  std::cerr << "Verifying any MIST can be reoriented to have the LRE vertex as a leaf" << std::endl;

  size_t num_tests = 5000;
  size_t seed = 239476502;
  size_t num_verts = 16;

  std::default_random_engine rng(seed);

  size_t trivial_passes = 0;
  for (size_t test = 0; test < num_tests; ++test) {
    std::cerr << '\r' << "Running test " << (test + 1) << " / " << num_tests << ", " << trivial_passes << " passed trivially ...";

    // Get a random MIST
    auto g = Graph::random_connected_interval_graph(rng(), num_verts);
    auto maybe_mist = solvers::dp::interval_mist_dp(g);
    assert(maybe_mist.has_value());
    auto orig_mist = maybe_mist.value();

    // Early exit trivially if LRE is already leaf
    auto lre = *g.verts.begin();
    if (orig_mist.leaves().count(lre)) {
      ++trivial_passes;
      continue;
    }

    if (!graph::tree_transform::lre_leaf_transform(orig_mist).has_value()) {
      std::cerr << std::endl;
      std::cerr << "ERROR: validate_lre_leaf_transform: Failed to transform MIST to LRE form" << std::endl;
      orig_mist.report(std::cerr);
      std::cerr << std::endl;
    }
  }

  std::cerr << std::endl;
}

} // namespace interval_mist

int main() {
  interval_mist::pc_eq_mist_counterexample();

  // interval_mist::fuzz_greedy_vs_naive();

  // interval_mist::fuzz_greedy_vs_dp();

  interval_mist::validate_lre_leaf_transform();

  return 0;
}
