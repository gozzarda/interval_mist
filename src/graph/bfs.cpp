#include "bfs.hpp"

#include <map>
#include <queue>

namespace interval_mist::graph::bfs {

using Edge = Graph::Edge;

std::map<Vertex, Vertex> bfs_parents(Graph g, Vertex u) {
  auto [vs, es] = g;

  if (!vs.count(u)) {
    return {};
  }

  // Construct adjacency list
  std::map<Vertex, std::set<Vertex>> adjlist;
  for (auto [u, v] : es) {
    adjlist[u].insert(v);
    adjlist[v].insert(u);
  }

  std::map<Vertex, Vertex> parents;
  std::queue<Vertex> queue;

  parents.emplace(u, u);
  queue.push(u);

  while (!queue.empty()) {
    Vertex curr = queue.front(); queue.pop();

    for (Vertex next : adjlist[curr]) {
      if (!parents.count(next)) {
        parents.emplace(next, curr);
        queue.push(next);
      }
    }
  }

  return parents;
}

std::map<Vertex, std::vector<Vertex>> leaf_paths(Graph g, Vertex u) {
  auto parents = bfs_parents(g, u);

  std::map<Vertex, size_t> degrees;
  for (auto [u, v] : parents) {
    if (u == v) continue;
    ++degrees[u];
    ++degrees[v];
  }

  std::map<Vertex, std::vector<Vertex>> paths;
  for (auto [leaf, degree] : degrees) {
    if (degree != 1) continue;

    std::vector<Vertex> path = {leaf};
    while (parents.at(path.back()) != path.back()) {
      path.push_back(parents.at(path.back()));
    }

    paths.emplace(leaf, path);
  }

  return paths;
}

} // namespace interval_mist::graph::bfs