#include "tester.hpp"

#include <iostream>
#include <random>

namespace interval_mist::tester {

bool Result::agree() const {
  if (lhs.has_value() != rhs.has_value())
    return false;
  if (!lhs.has_value())
    return true;
  return lhs.value().num_leaves() == rhs.value().num_leaves();
}

void Result::report(std::ostream &os, std::string lhs_name,
                    std::string rhs_name) const {
  input.report(os);
  os << std::endl << std::endl;
  os << lhs_name << ": ";
  if (lhs) {
    os << "found MIST with " << lhs.value().num_leaves() << " leaves"
       << std::endl;
    lhs.value().report_edges(os);
  } else {
    os << "no MIST found";
  }
  os << std::endl << std::endl;
  os << rhs_name << ": ";
  if (rhs) {
    os << "found MIST with " << rhs.value().num_leaves() << " leaves"
       << std::endl;
    rhs.value().report_edges(os);
  } else {
    os << "no MIST found";
  }
  os << std::endl;
}

Result compare_solvers(Solver lhs, Solver rhs, Graph graph) {
  return Result{
      .input = graph,
      .lhs = lhs(graph),
      .rhs = rhs(graph),
  };
}

Result compare_solvers(Solver lhs, Solver rhs, size_t seed, size_t num_verts) {
  auto g = Graph::random_connected_interval_graph(seed, num_verts);
  return compare_solvers(lhs, rhs, g);
}

std::optional<Result> fuzz_compare_solvers(Solver lhs, Solver rhs,
                                           size_t num_tests, size_t seed,
                                           size_t num_verts) {
  std::default_random_engine rng(seed);

  for (size_t test = 0; test < num_tests; ++test) {
    std::cerr << "Running test " << test << " / " << num_tests << "...";

    size_t seed = rng();
    Result result = compare_solvers(lhs, rhs, seed, num_verts);
    if (!result.agree()) {
      std::cerr << std::endl;
      std::cerr << "Solvers disagree for seed = " << seed
                << ", num_verts = " << num_verts << std::endl;
      return result;
    }

    std::cerr << '\r';
  }

  std::cerr << std::endl;

  return {};
}

} // namespace interval_mist::tester