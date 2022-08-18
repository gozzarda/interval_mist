#pragma once

#include "../graph.hpp"
#include <optional>

namespace gozz::interval_mist::naive {

using Graph = gozz::graph::Graph;

std::optional<Graph> interval_mist_naive(Graph g);

} // namespace gozz::interval_mist::naive