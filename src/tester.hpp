#pragma once

#include "graph.hpp"

#include <optional>

namespace interval_mist::tester {

using Graph = interval_mist::graph::Graph;

using Solver = std::optional<Graph> (*)(Graph);

struct Result {
  Graph input;
  std::optional<Graph> lhs, rhs;

  bool agree() const;

  void report(std::ostream &os, std::string lhs_name,
              std::string rhs_name) const;
};

Result compare_solvers(Solver lhs, Solver rhs, Graph graph);

Result compare_solvers(Solver lhs, Solver rhs, size_t seed, size_t num_verts);

std::optional<Result> fuzz_compare_solvers(Solver lhs, Solver rhs,
                                           size_t num_tests, size_t seed,
                                           size_t num_verts);

} // namespace interval_mist::tester