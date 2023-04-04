#pragma once

#include "../graph.hpp"

#include <map>
#include <vector>

namespace interval_mist::graph::bfs {

using Graph = interval_mist::graph::Graph;
using Vertex = Graph::Vertex;

std::map<Vertex, Vertex> bfs_parents(Graph g, Vertex u);

std::map<Vertex, std::vector<Vertex>> leaf_paths(Graph g, Vertex u);

} // namespace interval_mist::graph::bfs