#include <gtest/gtest.h>

#include "bfs.hpp"

namespace interval_mist::graph::bfs {

using Vertex = Graph::Vertex;
using Edge = Graph::Edge;

TEST(BfsParentsTest, Empty) {
  Graph g = Graph({}, {});
  std::map<Vertex, Vertex> expected = {};
  std::map<Vertex, Vertex> actual = bfs_parents(g, Vertex(0, 1));
  EXPECT_EQ(expected, actual);
}

TEST(BfsParentsTest, Singleton) {
  Vertex v = Vertex(0, 1);
  Graph g = Graph({v}, {});
  std::map<Vertex, Vertex> expected = {{v, v}};
  std::map<Vertex, Vertex> actual = bfs_parents(g, Vertex(0, 1));
  EXPECT_EQ(expected, actual);
}

TEST(BfsParentsTest, SingleEdge) {
  Vertex u = Vertex(0, 2), v = Vertex(1, 3);
  std::set<Vertex> vs = { u, v };
  std::set<Edge> es = { Edge(u, v) };
  Graph g = Graph(vs, es);
  std::map<Vertex, Vertex> expected = {{u, u}, {v, u}};
  std::map<Vertex, Vertex> actual = bfs_parents(g, u);
  EXPECT_EQ(expected, actual);
}

TEST(BfsParentsTest, SimpleTree) {
  // ___ ___ _________ 
  //  _________ ___ ___
  std::set<Vertex> vs = {
    Vertex(0, 2),
    Vertex(1, 6),
    Vertex(3, 4),
    Vertex(5, 10),
    Vertex(7, 8),
    Vertex(9, 11),
  };
  Graph g = Graph::interval_graph_from_set(vs);
  std::map<Vertex, Vertex> expected = {
    { Vertex(0, 2),  Vertex(1, 6)  },
    { Vertex(1, 6),  Vertex(5, 10) },
    { Vertex(3, 4),  Vertex(1, 6)  },
    { Vertex(5, 10), Vertex(5, 10) },
    { Vertex(7, 8),  Vertex(5, 10) },
    { Vertex(9, 11), Vertex(5, 10) },
  };
  std::map<Vertex, Vertex> actual = bfs_parents(g, Vertex(5, 10));
  EXPECT_EQ(expected, actual);
}

TEST(LeafPathsTest, Empty) {
  Graph g = Graph({}, {});
  std::map<Vertex, std::vector<Vertex>> expected = {};
  std::map<Vertex, std::vector<Vertex>> actual = leaf_paths(g, Vertex(0, 1));
  EXPECT_EQ(expected, actual);
}

TEST(LeafPathsTest, Singleton) {
  Vertex v = Vertex(0, 1);
  Graph g = Graph({v}, {});
  std::map<Vertex, std::vector<Vertex>> expected = {};
  std::map<Vertex, std::vector<Vertex>> actual = leaf_paths(g, Vertex(0, 1));
  EXPECT_EQ(expected, actual);
}

TEST(LeafPathsTest, SingleEdge) {
  Vertex u = Vertex(0, 2), v = Vertex(1, 3);
  std::set<Vertex> vs = { u, v };
  std::set<Edge> es = { Edge(u, v) };
  Graph g = Graph(vs, es);
  std::map<Vertex, std::vector<Vertex>> expected = {
    { u, { u } },
    { v, { v, u } }
  };
  std::map<Vertex, std::vector<Vertex>> actual = leaf_paths(g, u);
  EXPECT_EQ(expected, actual);
}

TEST(LeafPathsTest, SimpleTree) {
  // ___ ___ _________ 
  //  _________ ___ ___
  std::set<Vertex> vs = {
    Vertex(0, 2),
    Vertex(1, 6),
    Vertex(3, 4),
    Vertex(5, 10),
    Vertex(7, 8),
    Vertex(9, 11),
  };
  Graph g = Graph::interval_graph_from_set(vs);
  std::map<Vertex, std::vector<Vertex>> expected = {
    { Vertex(0, 2),  { Vertex(0, 2), Vertex(1, 6), Vertex(5, 10) } },
    { Vertex(3, 4),  { Vertex(3, 4), Vertex(1, 6), Vertex(5, 10) } },
    { Vertex(7, 8),  { Vertex(7, 8), Vertex(5, 10) } },
    { Vertex(9, 11), { Vertex(9, 11), Vertex(5, 10) } },
  };
  std::map<Vertex, std::vector<Vertex>> actual = leaf_paths(g, Vertex(5, 10));
  EXPECT_EQ(expected, actual);
}

TEST(LeafPathsTest, RootLeafTree) {
  // ___ ___ _________ 
  //  _________ ___ ___
  std::set<Vertex> vs = {
    Vertex(0, 2),
    Vertex(1, 6),
    Vertex(3, 4),
    Vertex(5, 10),
    Vertex(7, 8),
    Vertex(9, 11),
  };
  Graph g = Graph::interval_graph_from_set(vs);
  std::map<Vertex, std::vector<Vertex>> expected = {
    { Vertex(0, 2),  { Vertex(0, 2), Vertex(1, 6), Vertex(5, 10), Vertex(9, 11) } },
    { Vertex(3, 4),  { Vertex(3, 4), Vertex(1, 6), Vertex(5, 10), Vertex(9, 11) } },
    { Vertex(7, 8),  { Vertex(7, 8), Vertex(5, 10), Vertex(9, 11) } },
    { Vertex(9, 11), { Vertex(9, 11) } },
  };
  std::map<Vertex, std::vector<Vertex>> actual = leaf_paths(g, Vertex(9, 11));
  EXPECT_EQ(expected, actual);
}

} // namespace interval_mist::graph::bfs