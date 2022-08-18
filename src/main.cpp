#include "graph.hpp"
#include "interval.hpp"
#include "interval_mist.hpp"
#include <bits/stdc++.h>

using namespace std;

using Interval = gozz::interval::Interval;
using Coord = Interval::Coord;
using Graph = gozz::graph::Graph;
using Vertex = Graph::Vertex;
using Edge = Graph::Edge;

void cerr_edges(set<Edge> es) {
  for (auto e : es) {
    cerr << e.src << " - " << e.dst << endl;
  }
}

void cerr_graph(Graph g) {
  auto [vs, es] = g;

  for (auto v : vs) {
    cerr << v << endl;
  }
  cerr << endl;
  cerr_edges(g.edges);
}

void cerr_pc(vector<vector<Vertex>> pc) {
  cerr << "PATH COVER: " << pc.size() << " paths" << endl;
  for (auto p : pc) {
    for (auto v : p) {
      cerr << "  " << v;
    }
    cerr << endl;
  }
}

/*
 * FUZZ TEST GREEDY AGAINST NAIVE
 */

bool test_greedy_eq_mist(Graph g) {
  auto [vs, es] = g;
  auto maybe_mist_greedy = gozz::interval_mist::greedy::interval_mist_greedy(g);
  auto maybe_mist_naive = gozz::interval_mist::naive::interval_mist_naive(g);

  if (!maybe_mist_greedy.has_value() && !maybe_mist_naive.has_value()) {
    assert(false);
    return true;
  }
  if (maybe_mist_greedy.has_value() != maybe_mist_naive.has_value()) {
    cerr_graph(g);
    cerr << endl;
    cerr << "GREEDY: "
         << (maybe_mist_greedy.has_value() ? "found solution"
                                           : "no solution found")
         << endl;
    cerr << "NAIVE: "
         << (maybe_mist_naive.has_value() ? "found solution"
                                          : "no solution found")
         << endl;
    return false;
  }

  auto mist_greedy = maybe_mist_greedy.value();
  auto mist_naive = maybe_mist_naive.value();
  if (mist_greedy.num_leaves() != mist_naive.num_leaves()) {
    cerr_graph(g);
    cerr << endl;
    cerr << "GREEDY: " << mist_greedy.num_leaves() << " leaves" << endl;
    cerr_edges(mist_greedy.edges);
    cerr << endl;
    cerr << "NAIVE: " << mist_naive.num_leaves() << " leaves" << endl;
    return false;
  }

  return true;
}

bool test_greedy_eq_mist(size_t seed, size_t num) {
  auto g = Graph::random_connected_interval_graph(seed, num);
  if (!test_greedy_eq_mist(g)) {
    cerr << "test_greedy_eq_mist failed with seed = " << seed
         << ", num = " << num << endl;
    return false;
  }
  return true;
}

bool fuzz_greedy_eq_mist(size_t seed, size_t count, size_t num) {
  default_random_engine rng(seed);

  for (size_t i = 0; i < count; ++i) {
    cerr << i << " / " << count;
    if (!test_greedy_eq_mist(rng(), num)) {
      cerr << endl;
      return false;
    }
    cerr << '\r';
  }
  return true;
}

/*
 * FUZZ TEST PC AGAINST NAIVE
 */

bool test_pc_eq_mist(Graph g) {
  auto [vs, es] = g;
  auto pc = gozz::interval_mist::path_cover::interval_path_cover(g);
  auto maybe_mist_greedy = gozz::interval_mist::greedy::interval_mist_greedy(g);
  assert(maybe_mist_greedy.has_value());
  auto mist_greedy = maybe_mist_greedy.value();
  assert(mist_greedy.is_spanning_tree_of(g.verts));
  if (pc.size() + 1 != mist_greedy.num_leaves()) {
    auto mist_naive =
        gozz::interval_mist::naive::interval_mist_naive(g).value();
    assert(mist_naive.is_spanning_tree_of(g.verts));
    assert(mist_greedy.num_leaves() == mist_naive.num_leaves());

    cerr_graph(g);
    cerr << endl;
    cerr_pc(pc);
    cerr << endl;
    cerr << "GREEDY: " << mist_greedy.num_leaves() << " leaves" << endl;
    cerr_edges(mist_greedy.edges);
    cerr << endl;
    cerr << "NAIVE: " << mist_naive.num_leaves() << " leaves" << endl;

    return false;
  }
  return true;
}

bool test_pc_eq_mist(size_t seed, size_t num) {
  auto g = Graph::random_connected_interval_graph(seed, num);
  if (!test_pc_eq_mist(g)) {
    cerr << "test_pc_eq_mist failed with seed = " << seed << ", num = " << num
         << endl;
    return false;
  }
  return true;
}

bool fuzz_pc_eq_mist(size_t seed, size_t count, size_t num) {
  default_random_engine rng(seed);

  for (size_t i = 0; i < count; ++i) {
    if (!test_pc_eq_mist(rng(), num))
      return false;
  }
  return true;
}

void pc_eq_mist_counterexample() {
  set<Interval> vs = {
      Interval(0, 2),  Interval(1, 6), Interval(3, 4),
      Interval(5, 10), Interval(7, 8), Interval(9, 11),
  };
  auto g = Graph::interval_graph_from_set(vs);
  // auto mt = interval_mist_path_cover(g);
  // assert(mt.has_value());
  assert(!test_pc_eq_mist(g));
}

int main() {
  if (fuzz_greedy_eq_mist(126835921, 100, 10)) {
    cerr << "fuzz_greedy_eq_mist passed" << endl;
  }
  // if (fuzz_pc_eq_mist(792451839, 1000, 10)) {
  //   cerr << "fuzz_pc_eq_mist passed" << endl;
  // }
  // test_pc_eq_mist(402518898, 10);
  // pc_eq_mist_counterexample();
}
