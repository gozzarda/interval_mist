#include "graph.hpp"
#include <map>
#include <stack>
#include <vector>
#include <cassert>

namespace gozz::graph {

using Vertex = Graph::Vertex;
using Edge = Graph::Edge;

// Canonical orientation of undirected edges
Edge::Edge(Vertex u, Vertex v) : src(std::min(u, v)), dst(std::max(u, v)) {}

Graph::Graph(std::set<Vertex> vs, std::set<Edge> es) : verts(vs), edges(es) {
  for (Edge e : edges) {
    assert(verts.count(e.src));
    assert(verts.count(e.dst));
  }
}

Graph Graph::interval_graph_from_set(std::set<Vertex> vs) {
  struct Event {
    Vertex vert;
    bool open;

    Event(Vertex vert, bool open) : vert(vert), open(open) {}

    std::weak_ordering operator<=>(const Event &rhs) const {
      Vertex::Coord lc = open ? vert.lower : vert.upper;
      Vertex::Coord rc = rhs.open ? rhs.vert.lower : rhs.vert.upper;
      if (auto cmp = lc <=> rc; cmp != 0)
        return cmp;
      return rhs.open <=> open;
    }
  };

  std::vector<Event> events;
  for (auto vert : vs) {
    events.emplace_back(vert, true);
    events.emplace_back(vert, false);
  }
  sort(events.begin(), events.end());

  std::set<Edge> es;
  std::set<Vertex> curr;
  for (auto event : events) {
    if (event.open) {
      for (auto vert : curr) {
        es.insert(Edge(event.vert, vert));
      }
      curr.insert(event.vert);
    } else {
      curr.erase(event.vert);
    }
  }

  return Graph(vs, es);
}

Graph Graph::random_connected_interval_graph(size_t seed, size_t verts) {
  return Graph::interval_graph_from_set(gozz::interval::random_connected_interval_set(seed, verts));
}

void Graph::insert_vertex(Vertex v) { verts.insert(v); }

void Graph::insert_edge(Edge e) {
  verts.insert(e.src);
  verts.insert(e.dst);
  edges.insert(e);
}

void Graph::insert_edge(Vertex u, Vertex v) { insert_edge(Edge(u, v)); }

bool Graph::is_connected() const {
  if (verts.empty())
    return true;

  std::map<Vertex, std::vector<Vertex>> adjlist;
  for (Edge e : edges) {
    adjlist[e.src].push_back(e.dst);
    adjlist[e.dst].push_back(e.src);
  }

  std::stack<Vertex> stack;
  std::set<Vertex> seen;

  stack.push(*verts.begin());
  seen.insert(stack.top());

  while (!stack.empty()) {
    Vertex u = stack.top();
    stack.pop();

    for (Vertex v : adjlist[u]) {
      if (seen.count(v))
        continue;
      seen.insert(v);
      stack.push(v);
    }
  }

  return seen.size() == verts.size();
}

bool Graph::is_tree() const {
  return edges.size() == verts.size() - 1 && is_connected();
}

bool Graph::is_spanning_tree_of(const std::set<Vertex> &vs) const {
  return verts == vs && is_tree();
}

size_t Graph::num_leaves() const {
  std::map<Vertex, size_t> cardinality;
  for (auto [u, v] : edges) {
    ++cardinality[u];
    ++cardinality[v];
  }
  size_t count = 0;
  for (auto [k, v] : cardinality) {
    if (v == 1)
      ++count;
  }
  return count;
}

} // namespace gozz::graph