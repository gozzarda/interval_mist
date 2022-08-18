#pragma once

#include <optional>
#include "../graph.hpp"

namespace gozz::interval_mist::naive {

using Graph = gozz::graph::Graph;

std::optional<Graph> interval_mist_naive(Graph g);

}