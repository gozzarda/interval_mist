#pragma once

#include "../graph.hpp"

#include <optional>

namespace interval_mist::graph::tree_transform {

using Graph = interval_mist::graph::Graph;

std::optional<Graph> lre_leaf_transform(Graph g);

} // namespace interval_mist::graph::tree_transform