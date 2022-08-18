#pragma once

#include "../graph.hpp"
#include <optional>

namespace gozz::interval_mist::greedy {

using Graph = gozz::graph::Graph;

std::optional<Graph> interval_mist_greedy(Graph g);

} // namespace gozz::interval_mist::greedy