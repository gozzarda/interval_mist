#pragma once

#include "../graph.hpp"

#include <optional>

namespace interval_mist::solvers::dp {

using Graph = interval_mist::graph::Graph;

std::optional<Graph> interval_mist_dp(Graph g);

} // namespace interval_mist::solvers::dp