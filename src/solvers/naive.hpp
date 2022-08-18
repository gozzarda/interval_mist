#pragma once

#include "../graph.hpp"
#include <optional>

namespace interval_mist::solvers::naive {

using Graph = interval_mist::graph::Graph;

std::optional<Graph> interval_mist_naive(Graph g);

} // namespace interval_mist::solvers::naive