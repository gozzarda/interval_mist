#pragma once

#include "../graph.hpp"

#include <optional>

namespace interval_mist::solvers::greedy {

using Graph = interval_mist::graph::Graph;

std::optional<Graph> interval_mist_greedy(Graph g);

} // namespace interval_mist::solvers::greedy