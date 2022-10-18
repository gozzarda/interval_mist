#include "graph.hpp"
#include "graph/bfs.hpp"
#include "graph/hamiltonian.hpp"
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

    // Repeatedly transform leaf to leaf path through LRE vertex to greedy hamiltonian order
    // This should strictly decrease the degree of the LRE vertex without increasing number of leaves
    auto prev_mist = orig_mist;
    while (!prev_mist.leaves().count(lre)) {
      // Get paths from every leaf to LRE and pick any two to find a leaf to leaf path through LRE
      auto leaf_paths = graph::bfs::leaf_paths(prev_mist, lre);
      assert(!leaf_paths.count(lre));
      assert(leaf_paths.size() >= 2);
      auto path = leaf_paths.begin()->second;
      path.pop_back();
      for (auto &[leaf, leaf_path] : leaf_paths) {
        if (leaf_path[leaf_path.size()-2] != path.back()) {
          path.insert(path.end(), leaf_path.rbegin(), leaf_path.rend());
          break;
        }
      }
      assert(path.size() > leaf_paths.begin()->second.size());

      // Erase every edge in path from MIST
      auto curr_mist = prev_mist;
      Vertex prev = path.front();
      for (Vertex curr : path) {
        if (prev != curr) curr_mist.edges.erase(Edge(prev, curr));
        prev = curr;
      }

      // Find the greedy hamiltonian ordering of the path
      auto maybe_ham_path = graph::hamiltonian::hamiltonian(Graph::interval_graph_from_set(std::set<Vertex>(path.begin(), path.end())));
      assert(maybe_ham_path.has_value());
      auto ham_path = maybe_ham_path.value();

      // Insert new path edges into MIST
      prev = ham_path.front();
      for (Vertex curr : ham_path) {
        if (prev != curr) curr_mist.edges.insert(Edge(prev, curr));
        prev = curr;
      }

      // Test that MIST is still a tree
      if (!curr_mist.is_tree()) {
        std::cerr << std::endl;
        std::cerr << "ERROR: MIST is no longer tree" << std::endl;
        return;
      }

      // Test that number of leaves has not increased
      if (curr_mist.leaves().size() > prev_mist.leaves().size()) {
        std::cerr << std::endl;
        std::cerr << "ERROR: Number of leaves in MIST has increased" << std::endl;
        return;
      }

      // Test that degree of LRE has decreased
      size_t prev_degree = 0, curr_degree = 0;
      for (auto [u, v] : prev_mist.edges) {
        if (u == lre) ++prev_degree;
        if (v == lre) ++prev_degree;
      }
      for (auto [u, v] : curr_mist.edges) {
        if (u == lre) ++curr_degree;
        if (v == lre) ++curr_degree;
      }
      if (curr_degree >= prev_degree) {
        std::cerr << std::endl;
        std::cerr << "ERROR: Degree of LRE vertex has not decreased" << std::endl;
        return;
      }

      prev_mist = curr_mist;
    }

    if (!prev_mist.leaves().count(lre)) {
      std::cerr << std::endl;
      std::cerr << "ERROR: Failed to convert MIST to LRE-leaf form" << std::endl;
      return;
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
