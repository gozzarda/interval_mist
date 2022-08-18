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

vector<vector<Vertex>> interval_path_cover(Graph g) {
  auto [vs, es] = g;

  map<Vertex, vector<Vertex>> adjlist;
  for (auto [u, v] : es) {
    assert(vs.count(u));
    assert(vs.count(v));
    adjlist[u].push_back(v);
    adjlist[v].push_back(u);
  }

  for (auto &[k, v] : adjlist) {
    sort(v.begin(), v.end());
  }

  set<Vertex> covered;
  set<Vertex> ordered(vs.begin(), vs.end());

  vector<vector<Vertex>> paths(1);
  paths.back().push_back(*ordered.begin());
  covered.insert(paths.back().back());
  ordered.erase(ordered.begin());

  while (!ordered.empty()) {
    Vertex curr = paths.back().back();
    optional<Vertex> succ;
    for (auto v : adjlist[curr]) {
      if (!covered.count(v)) {
        succ = v;
        break;
      }
    }
    if (!succ) {
      succ = *ordered.begin();
      paths.push_back(vector<Vertex>());
    }
    paths.back().push_back(succ.value());
    ordered.erase(succ.value());
    covered.insert(succ.value());
  }

  return paths;
}

set<Edge> path_to_edge_set(vector<Vertex> p) {
  set<Edge> result;
  for (size_t i = 1; i < p.size(); ++i) {
    result.insert(Edge(p[i - 1], p[i]));
  }
  return result;
}

optional<Graph> interval_mist_path_cover(Graph g) {
  auto [vs, es] = g;

  // Find a path cover P* of G
  auto pc = interval_path_cover(g);
  cerr << "PATH COVER:" << endl;
  for (auto p : pc) {
    for (auto v : p) {
      cerr << "  (" << v.lower << ", " << v.upper << ")";
    }
    cerr << endl;
  }
  // Tc <- {p | p \in P*}, Pc <- P* \ {p}
  auto tvs = set<Vertex>(pc.back().begin(), pc.back().end());
  auto tes = path_to_edge_set(pc.back());
  pc.pop_back();
  // while Pc not empty:
  while (!pc.empty()) {
    // Choose q from Pc where q intersects Tc
    auto qit = pc.begin();
    for (; qit != pc.end(); ++qit) {
      Vertex tv_lmost = *tvs.begin();
      Vertex tv_rmost = *tvs.rbegin();
      if (any_of(qit->begin(), qit->end(), [tv_lmost, tv_rmost](Vertex qv) {
            return tv_lmost < qv && qv < tv_rmost;
          }))
        break;

      Vertex qv_lmost = *min_element(qit->begin(), qit->end());
      Vertex qv_rmost = *max_element(qit->begin(), qit->end());
      if (any_of(tvs.begin(), tvs.end(), [qv_lmost, qv_rmost](Vertex tv) {
            return qv_lmost < tv && tv < qv_rmost;
          }))
        break;

      // TODO: Their definition of intersects doesn't actually work here, try
      // with normal definition? bool intersects = false; for (auto tv : tvs) {
      //   for (auto qv : *qit) {
      //     if (intervals_intersect(tv, qv)) {
      //       cerr << "\t(" << tv.lower << ", " << tv.upper << ") intersects ("
      //       << qv.lower << ", " << qv.upper << ")" << endl; intersects =
      //       true; break;
      //     }
      //   }
      //   if (intersects) break;
      // }
      // if (intersects) break;
    }
    if (qit == pc.end())
      return {};
    auto qvs = set<Vertex>(qit->begin(), qit->end());
    auto qes = path_to_edge_set(*qit);
    // get leftMost(Tc) and leftMost(q) for next steps
    Vertex tv_lmost = *tvs.begin();
    Vertex qv_lmost = *qvs.begin();
    // if leftMost(q) < leftMost(Tc):
    if (qv_lmost.upper < tv_lmost.upper) {
      // Choose w \in V(q) adjacent to v_leftMost(Tc)
      optional<Vertex> w;
      for (auto qv : qvs) {
        if (qv.intersects(tv_lmost)) {
          w = qv;
          break;
        }
      }
      if (!w) {
        cerr << "ERROR:" << endl;
        cerr << "None of" << endl;
        for (auto qv : qvs) {
          cerr << "\t(" << qv.lower << ", " << qv.upper << ")" << endl;
        }
        cerr << "intersect (" << tv_lmost.lower << ", " << tv_lmost.upper << ")"
             << endl;
        return {};
      }
      // Update Tc by adding edge between these to connect q to Tc tree
      tvs.insert(qvs.begin(), qvs.end());
      tes.insert(qes.begin(), qes.end());
      tes.insert(Edge(w.value(), tv_lmost));
    }
    // if leftMost(q) > leftMost(Tc):
    if (qv_lmost.upper > tv_lmost.upper) {
      // Choose w \in V(Tc) adjacent to v_leftMost(q)
      optional<Vertex> w;
      for (auto tv : tvs) {
        if (tv.intersects(qv_lmost)) {
          w = tv;
          break;
        }
      }
      if (!w)
        return {};
      // Update Tc by adding edge between these to connect q to Tc tree
      tvs.insert(qvs.begin(), qvs.end());
      tes.insert(qes.begin(), qes.end());
      tes.insert(Edge(w.value(), qv_lmost));
    }
    // Pc <- Pc \ {q}
    pc.erase(qit);
  }
  // return Tc
  return Graph(set<Vertex>(tvs.begin(), tvs.end()), tes);
}

optional<Graph> interval_mist_greedy(Graph g) {
  auto [vs, es] = g;

  // Construct adjacency list
  map<Vertex, set<Vertex>> adjlist;
  for (auto [u, v] : es) {
    assert(vs.count(u));
    assert(vs.count(v));
    adjlist[u].insert(v);
    adjlist[v].insert(u);
  }

  // Remaining vertices to add to tree and edges in tree so far
  set<Vertex> todo(vs.begin(), vs.end());
  set<Edge> tes;
  set<Vertex> tvs;

  // Start with naive tree of interval with leftmost right endpoint (LRE)
  // Invariant: prev is always a leaf in the tree
  Vertex prev = *todo.begin();
  todo.erase(todo.begin());
  tvs.insert(prev);

  // While there are vertices not yet in tree
  while (!todo.empty()) {
    // Select adjacent vertex to prev with LRE
    optional<Vertex> curr;
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
        gozz::interval_mist::naive::interval_mist_naive(tg).value();
    assert(t.num_leaves() == mist_naive.num_leaves());
  }

  return {{vs, tes}};
}

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
  auto maybe_mist_greedy = interval_mist_greedy(g);
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
  auto pc = interval_path_cover(g);
  auto maybe_mist_greedy = interval_mist_greedy(g);
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
