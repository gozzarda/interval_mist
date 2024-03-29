#pragma once

#include "interval.hpp"

#include <set>

namespace interval_mist::graph {

struct Graph {
  using Vertex = interval_mist::interval::Interval;

  struct Edge {
    Vertex src, dst;

    Edge(Vertex, Vertex);

    friend auto operator<=>(const Edge &, const Edge &) = default;

    friend std::ostream &operator<<(std::ostream &os, const Edge &val);
  };

  std::set<Vertex> verts;
  std::set<Edge> edges;

  Graph(std::set<Vertex>, std::set<Edge>);

  static Graph
      interval_graph_from_set(std::set<interval_mist::interval::Interval>);

  static Graph random_connected_interval_graph(size_t seed, size_t verts);

  void insert_vertex(Vertex);

  void insert_edge(Edge);

  void insert_edge(Vertex, Vertex);

  bool is_connected() const;

  bool is_tree() const;

  bool is_spanning_tree_of(const std::set<Vertex> &) const;

  std::set<Vertex> leaves() const;

  size_t num_leaves() const;

  void report_verts(std::ostream &os) const;

  void report_edges(std::ostream &os) const;

  void report(std::ostream &os) const;

  bool operator==(const Graph& rhs) const;

  friend std::ostream &operator<<(std::ostream &os, const Graph &g);
};

} // namespace interval_mist::graph