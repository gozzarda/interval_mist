#include "naive.hpp"
#include "../graph.hpp"
#include <limits>
#include <vector>
#include <set>
#include <utility>
#include <stack>

namespace gozz::interval_mist::naive {

using Graph = gozz::graph::Graph;
using Vertex = Graph::Vertex;
using Edge = Graph::Edge;

std::optional<Graph> interval_mist_naive(Graph g) {
  size_t best_leaves = std::numeric_limits<size_t>::max();
  std::optional<Graph> best;

  auto [vs, es] = g;
  size_t limit = vs.size() - 1;
  std::vector<Edge> edges(es.begin(), es.end());

  std::stack<std::pair<std::set<Edge>, size_t>> s;
  s.push(make_pair(std::set<Edge>(), 0));
  while (!s.empty()) {
    auto [es_prime, i] = s.top();
    s.pop();
    if (es_prime.size() == limit) {
      auto g_prime = Graph(vs, es_prime);
      if (g_prime.is_spanning_tree_of(g.verts)) {
        size_t leaves = g_prime.num_leaves();
        if (leaves < best_leaves) {
          best_leaves = leaves;
          best = g_prime;
        }
      }
    } else if (es_prime.size() < limit && i < edges.size()) {
      s.push(make_pair(es_prime, i + 1));
      es_prime.insert(edges[i]);
      s.push(make_pair(es_prime, i + 1));
    }
  }

  return best;
}

}