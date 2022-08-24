#include "dp.hpp"

#include <array>
#include <limits>
#include <map>
#include <set>
#include <unordered_map>
#include <vector>

namespace interval_mist::solvers::dp {

using Vertex = Graph::Vertex;
using Edge = Graph::Edge;

static constexpr size_t max_pow(uint64_t base) {
  size_t result = 0;
  uint64_t curr = std::numeric_limits<uint64_t>::max();
  while (curr > 1) {
    curr /= base;
    ++result;
  }
  return result;
}

// State representation is a base-3 number where:
// - 0 is isolated
// - 1 is leaf
// - 2 is internal
// for each vertex in the order they appear in verts
// 3^40 is largest power of 3 that can fit in a 64-bit int, so we are limited
// to 40 vertices.
struct State {
  typedef uint64_t Repr;

  static constexpr Repr base = 3;
  static constexpr size_t max_verts = max_pow(base);

  Repr data;
  size_t num_verts;

  State(size_t num_verts) : num_verts(num_verts), data(0) {
    assert(num_verts <= max_verts);
  }

  Repr pow(size_t p) const {
    Repr result = 1;
    Repr curr_base = base;
    while (p > 0) {
      if (p & 1)
        result *= curr_base;
      curr_base *= curr_base;
      p >>= 1;
    }
    return result;
  }

  void increment(size_t i) {
    Repr exp = pow(i);
    Repr dig = (data / exp) % base;
    if (dig + 1 < base)
      data += exp;
  }

  std::vector<Repr> degrees() const {
    std::vector<Repr> result(num_verts);
    Repr curr = data;
    for (size_t i = 0; i < num_verts; ++i) {
      result[i] = curr % base;
      curr /= base;
    }
    return result;
  }

  std::array<size_t, base> counts() const {
    std::array<size_t, base> result{};
    Repr curr = data;
    for (size_t i = 0; i < num_verts; ++i) {
      ++result[curr % base];
      curr /= base;
    }
    return result;
  }

  friend auto operator<=>(const State &, const State &) = default;

  size_t hash() const { return ::std::hash<Repr>{}(data); }
};

struct StateHash {
  size_t operator()(State const &state) const noexcept { return state.hash(); }
};

// DP to find maximum number of internal nodes in a spanning tree of a graph
// Returns {} if no spanning tree can be found
// Case for state > 0: Some edges already exist in the tree
std::optional<size_t>
dpf(std::unordered_map<State, std::optional<size_t>, StateHash> &dpt,
    std::unordered_map<State, std::pair<size_t, size_t>, StateHash> &trace,
    const std::vector<std::vector<size_t>> &adj, State state) {
  // Use memoized value if available
  if (auto it = dpt.find(state); it != dpt.end())
    return it->second;

  // Base case: Graph is already connected
  if (auto counts = state.counts(); counts[0] == 0) {
    return dpt[state] = counts[2];
  }

  std::optional<size_t> best;
  std::optional<std::pair<size_t, size_t>> edge;

  bool first_edge = state.data == 0;
  auto degrees = state.degrees();

  for (size_t u = 0; u < adj.size(); ++u) {
    if (!first_edge && degrees[u] == 0)
      continue;

    for (size_t v : adj[u]) {
      if (degrees[v] != 0)
        continue;

      State substate = state;
      substate.increment(u);
      substate.increment(v);

      auto option = dpf(dpt, trace, adj, substate);
      if (!option)
        continue;

      if (!best || option.value() > best.value()) {
        best = option;
        edge = std::make_pair(u, v);
      }
    }
  }

  if (edge)
    trace[state] = edge.value();
  return dpt[state] = best;
}

std::optional<Graph> interval_mist_dp(Graph g) {
  // Build vertex <-> id mapping
  std::vector<Vertex> id_to_vert;
  std::map<Vertex, size_t> vert_to_id;
  for (Vertex v : g.verts) {
    vert_to_id[v] = id_to_vert.size();
    id_to_vert.push_back(v);
  }

  // Build adjacency list
  std::vector<std::vector<size_t>> adj(id_to_vert.size());
  for (Edge e : g.edges) {
    size_t u = vert_to_id[e.src];
    size_t v = vert_to_id[e.dst];
    adj[u].push_back(v);
    adj[v].push_back(u);
  }

  std::unordered_map<State, std::optional<size_t>, StateHash> dpt;
  std::unordered_map<State, std::pair<size_t, size_t>, StateHash> trace;

  State initial_state(adj.size());

  if (!dpf(dpt, trace, adj, initial_state))
    return {};

  std::set<Edge> tree_edges;
  for (State state = initial_state; trace.count(state);) {
    size_t u, v;
    std::tie(u, v) = trace.at(state);
    tree_edges.insert(Edge(id_to_vert[u], id_to_vert[v]));
    state.increment(u);
    state.increment(v);
  }

  return Graph(g.verts, tree_edges);
}

} // namespace interval_mist::solvers::dp