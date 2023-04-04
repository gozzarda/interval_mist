#include "hamiltonian.hpp"

#include <map>
#include <set>

namespace interval_mist::graph::hamiltonian {

using Vertex = Graph::Vertex;
using Edge = Graph::Edge;

std::optional<std::vector<Graph::Vertex>> hamiltonian(Graph g) {
  auto [vs, es] = g;

  if (vs.empty()) {
    return {{}};
  }

  // Construct adjacency list
  std::map<Vertex, std::set<Vertex>> adjlist;
  for (auto [u, v] : es) {
    assert(vs.count(u));
    assert(vs.count(v));
    adjlist[u].insert(v);
    adjlist[v].insert(u);
  }

  // Remaining vertices to add to path so far
  std::set<Vertex> todo(vs.begin(), vs.end());

  // Path so far
  std::vector<Vertex> path;

  // Start with LRE vertex
  path.push_back(*todo.begin());
  todo.erase(todo.begin());

  // While there are vertices not yet in the path
  while (!todo.empty()) {
    // Select adjacent vertex to prev with LRE
    std::optional<Vertex> curr;
    for (auto v : adjlist[path.back()]) {
      if (todo.count(v)) {
        curr = v;
        break;
      }
    }

    if (curr) {
      path.push_back(curr.value());
      todo.erase(curr.value());
    } else {
      return {};
    }
  }

  return {path};
}

} // namespace interval_mist::graph::hamiltonian