#include "tree_transform.hpp"
#include "bfs.hpp"
#include "hamiltonian.hpp"

#include <iostream>
#include <map>
#include <set>

namespace interval_mist::graph::tree_transform {

using Vertex = Graph::Vertex;
using Edge = Graph::Edge;

std::optional<Graph> lre_leaf_transform(Graph g) {
  if (!g.is_tree()) return {};
  if (!g.is_connected()) return {};
  if (g.verts.size() <= 1) return {g};

  // Repeatedly transform leaf to leaf path through LRE vertex to greedy hamiltonian order
  // This should strictly decrease the degree of the LRE vertex without increasing number of leaves
  auto lre = *g.verts.begin();
  auto curr_tree = g;
  while (!curr_tree.leaves().count(lre)) {
    // Get paths from every leaf to LRE and pick any two to find a leaf to leaf path through LRE
    auto leaf_paths = bfs::leaf_paths(curr_tree, lre);
    assert(!leaf_paths.count(lre));
    assert(leaf_paths.size() >= 2);
    auto path = leaf_paths.begin()->second;
    path.pop_back();
    for (auto &[leaf, leaf_path] : leaf_paths) {
      if (leaf_path[leaf_path.size()-2] != path.back()) {
        path.insert(path.end(), leaf_path.rbegin(), leaf_path.rend());
        break;
      }
    }
    assert(path.size() > leaf_paths.begin()->second.size());

    // Erase every edge in path from tree
    auto next_tree = curr_tree;
    Vertex prev = path.front();
    for (Vertex curr : path) {
      if (prev != curr) next_tree.edges.erase(Edge(prev, curr));
      prev = curr;
    }

    // Find the greedy hamiltonian ordering of the path
    auto maybe_ham_path = hamiltonian::hamiltonian(Graph::interval_graph_from_set(std::set<Vertex>(path.begin(), path.end())));
    assert(maybe_ham_path.has_value());
    auto ham_path = maybe_ham_path.value();

    // Insert new path edges into tree
    prev = ham_path.front();
    for (Vertex curr : ham_path) {
      if (prev != curr) next_tree.edges.insert(Edge(prev, curr));
      prev = curr;
    }

    // Test that graph is still a tree
    if (!next_tree.is_tree()) {
      std::cerr << "ERROR: lre_leaf_transform failed: Graph is no longer a tree" << std::endl;
      return {};
    }

    // Test that number of leaves has not increased
    if (next_tree.leaves().size() > curr_tree.leaves().size()) {
      std::cerr << "ERROR: lre_leaf_transform failed: Number of leaves in tree has increased" << std::endl;
      return {};
    }

    // Test that degree of LRE has decreased
    size_t prev_degree = 0, curr_degree = 0;
    for (auto [u, v] : curr_tree.edges) {
      if (u == lre) ++prev_degree;
      if (v == lre) ++prev_degree;
    }
    for (auto [u, v] : next_tree.edges) {
      if (u == lre) ++curr_degree;
      if (v == lre) ++curr_degree;
    }
    if (curr_degree >= prev_degree) {
      std::cerr << "ERROR: lre_leaf_transform failed: Degree of LRE vertex has not decreased" << std::endl;
      return {};
    }

    curr_tree = next_tree;
  }

  return { curr_tree };
}

} // namespace interval_mist::graph::tree_transform