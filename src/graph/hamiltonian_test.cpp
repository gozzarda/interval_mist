#include <gtest/gtest.h>

#include "hamiltonian.hpp"

namespace interval_mist::graph::hamiltonian {

using Vertex = Graph::Vertex;

TEST(HamiltonianTest, Empty) {
  Graph g = Graph({}, {});
  std::optional<std::vector<Vertex>> expected = {{}};
  std::optional<std::vector<Vertex>> actual = hamiltonian(g);
  EXPECT_EQ(actual, expected);
}

TEST(HamiltonianTest, Singleton) {
  Vertex a = Vertex(0, 1);
  Graph g = Graph::interval_graph_from_set({a});
  std::optional<std::vector<Vertex>> expected = {{a}};
  std::optional<std::vector<Vertex>> actual = hamiltonian(g);
  EXPECT_EQ(actual, expected);
}

TEST(HamiltonianTest, NoHamiltonian) {
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
  std::optional<std::vector<Vertex>> expected = {};
  std::optional<std::vector<Vertex>> actual = hamiltonian(g);
  EXPECT_EQ(actual, expected);
}

TEST(HamiltonianTest, Simple) {
  // ___ ___ 
  //   ___ ___
  std::vector<Vertex> expected = {
    Vertex(0, 2),
    Vertex(1, 4),
    Vertex(3, 6),
    Vertex(5, 7),
  };
  Graph g = Graph::interval_graph_from_set(std::set(expected.begin(), expected.end()));
  std::optional<std::vector<Vertex>> actual = hamiltonian(g);
  EXPECT_EQ(actual, expected);
}

} // namespace interval_mist::graph::hamiltonian