#pragma once

#include "../graph.hpp"
#include <optional>
#include <vector>

namespace gozz::interval_mist::path_cover {

using Graph = gozz::graph::Graph;
using Vertex = Graph::Vertex;

std::vector<std::vector<Vertex>> interval_path_cover(Graph g);

std::optional<Graph> interval_mist_path_cover(Graph g);

} // namespace gozz::interval_mist::path_cover