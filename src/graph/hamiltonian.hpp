#pragma once

#include "../graph.hpp"

#include <optional>
#include <vector>

namespace interval_mist::graph::hamiltonian {

using Graph = interval_mist::graph::Graph;

std::optional<std::vector<Graph::Vertex>> hamiltonian(Graph g);

} // namespace interval_mist::graph::hamiltonian