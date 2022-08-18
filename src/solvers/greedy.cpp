#include "greedy.hpp"

#include "naive.hpp"

#include <map>
#include <set>

namespace interval_mist::solvers::greedy {

using Vertex = Graph::Vertex;
using Edge = Graph::Edge;

std::optional<Graph> interval_mist_greedy(Graph g) {
  auto [vs, es] = g;

  // Construct adjacency list
  std::map<Vertex, std::set<Vertex>> adjlist;
  for (auto [u, v] : es) {
    assert(vs.count(u));
    assert(vs.count(v));
    adjlist[u].insert(v);
    adjlist[v].insert(u);
  }

  // Remaining vertices to add to tree and edges in tree so far
  std::set<Vertex> todo(vs.begin(), vs.end());
  std::set<Edge> tes;
  std::set<Vertex> tvs;

  // Start with naive tree of interval with leftmost right endpoint (LRE)
  // Invariant: prev is always a leaf in the tree
  Vertex prev = *todo.begin();
  todo.erase(todo.begin());
  tvs.insert(prev);

  // While there are vertices not yet in tree
  while (!todo.empty()) {
    // Select adjacent vertex to prev with LRE
    std::optional<Vertex> curr;
    for (auto v : adjlist[prev]) {
      if (todo.count(v)) {
        curr = v;
        break;
      }
    }

    if (curr) {
      // Greedily attach curr to leaf, making prev internal (unless root)
      tvs.insert(curr.value());
      tes.insert(Edge(prev, curr.value()));
      todo.erase(curr.value());
      prev = curr.value();
    } else {
      // Pick LRE interval not in tree that can be connected to tree
      // Connect to tree via LRE neighbour in tree
      // Must not be able to be connected via leaf (since no leaf took it)
      // Increases number of leaves, since it must branch off internal
      bool done = false;
      for (auto u : todo) {
        for (auto v : adjlist[u]) {
          if (!todo.count(v)) {
            tvs.insert(u);
            tes.insert(Edge(u, v));
            todo.erase(u);
            prev = u;
            done = true;
            break;
          }
        }
        if (done)
          break;
      }
    }

    Graph t = {tvs, tes};
    auto tg = Graph::interval_graph_from_set(tvs);
    assert(t.is_spanning_tree_of(tvs));
    auto mist_naive =
        interval_mist::solvers::naive::interval_mist_naive(tg).value();
    assert(t.num_leaves() == mist_naive.num_leaves());
  }

  return {{vs, tes}};
}

} // namespace interval_mist::solvers::greedy