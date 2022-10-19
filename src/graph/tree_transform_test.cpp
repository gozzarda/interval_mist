#include <gtest/gtest.h>

#include "tree_transform.hpp"

namespace interval_mist::graph::tree_transform {

using Vertex = Graph::Vertex;
using Edge = Graph::Edge;

TEST(LreLeafTransformTest, Empty) {
  Graph g = Graph({}, {});
  std::optional<Graph> result = lre_leaf_transform(g);
  ASSERT_FALSE(result.has_value());
}

TEST(LreLeafTransformTest, Singleton) {
  Vertex a = Vertex(0, 1);
  Graph g = Graph::interval_graph_from_set({a});
  std::optional<Graph> expected = {g};
  std::optional<Graph> actual = lre_leaf_transform(g);
  EXPECT_EQ(expected, actual);
}

TEST(LreLeafTransformTest, NonTree) {
  std::set<Vertex> verts = {
    Vertex(0, 3),
    Vertex(1, 4),
    Vertex(2, 5),
  };
  Graph g = Graph::interval_graph_from_set(verts);
  std::optional<Graph> result = lre_leaf_transform(g);
  ASSERT_FALSE(result.has_value());
}

TEST(LreLeafTransformTest, Simple) {
  // ___ ___ 
  //   ___ ___
  std::set<Vertex> verts = {
    Vertex(0, 4),
    Vertex(1, 5),
    Vertex(2, 6),
    Vertex(3, 7),
  };
  std::set<Edge> edges = {
    { Vertex(0, 4), Vertex(1, 5) },
    { Vertex(0, 4), Vertex(2, 6) },
    { Vertex(0, 4), Vertex(3, 7) },
  };
  Graph g = Graph(verts, edges);
  ASSERT_FALSE(g.leaves().count(Vertex(0, 4)));
  std::optional<Graph> maybe_result = lre_leaf_transform(g);
  ASSERT_TRUE(maybe_result.has_value());
  Graph result = maybe_result.value();
  ASSERT_TRUE(result.is_tree());
  ASSERT_TRUE(result.leaves().count(Vertex(0, 4)));
  ASSERT_TRUE(result.leaves().size() <= g.leaves().size());
}

} // namespace interval_mist::graph::tree_transform