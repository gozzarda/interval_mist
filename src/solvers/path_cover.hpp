#pragma once

#include "../graph.hpp"
#include <optional>
#include <vector>

namespace interval_mist::solvers::path_cover {

using Graph = interval_mist::graph::Graph;
using Vertex = Graph::Vertex;

std::vector<std::vector<Vertex>> interval_path_cover(Graph g);

std::optional<Graph> interval_mist_path_cover(Graph g);

} // namespace interval_mist::solvers::path_cover