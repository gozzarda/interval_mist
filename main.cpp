#include <bits/stdc++.h>

using namespace std;

typedef size_t coord_t;
struct Interval {
  coord_t lower, upper;
  Interval(coord_t lwr, coord_t upr) : lower(min(lwr, upr)), upper(max(lwr, upr)) {}
  bool operator==(const Interval& rhs) const { return lower == rhs.lower && upper == rhs.upper; }
  auto operator<=>(const Interval& rhs) const {
    if (upper == rhs.upper) return lower <=> rhs.lower;
    return upper <=> rhs.upper;
  }
  friend ostream& operator<<(ostream& os, const Interval& val) {
    os << '(' << val.lower << ", " << val.upper << ')';
    return os;
  }
};
typedef Interval interval_t;
typedef interval_t vertex_t;
typedef pair<vertex_t, vertex_t> edge_t;
typedef pair<set<vertex_t>, set<edge_t>> graph_t;

bool intervals_intersect(interval_t lhs, interval_t rhs) {
  if (lhs.upper < rhs.lower) return false;
  if (rhs.upper < lhs.lower) return false;
  return true;
}

edge_t make_edge(vertex_t u, vertex_t v) {
  return make_pair(min(u, v), max(u, v));
}

bool is_subgraph(graph_t l, graph_t r) {
  auto [lvs, les] = l;
  auto [rvs, res] = r;
  if (!includes(rvs.begin(), rvs.end(), lvs.begin(), lvs.end())) return false;
  if (!includes(res.begin(), res.end(), les.begin(), les.end())) return false;
  return true;
}

bool is_spanning_tree(graph_t l, graph_t r) {
  auto [lvs, les] = l;
  auto [rvs, res] = r;
  if (lvs != rvs) return false;
  if (les.size() != lvs.size() - 1) return false;
  if (lvs.empty()) return true;

  map<vertex_t, vector<vertex_t>> adjlist;
  for (auto [u, v] : les) {
    if (!lvs.count(u) || !lvs.count(v)) return false;
    adjlist[u].push_back(v);
    adjlist[v].push_back(u);
  }

  set<vertex_t> seen;
  stack<vertex_t> s;
  seen.insert(*lvs.begin());
  s.push(*lvs.begin());
  while (!s.empty()) {
    vertex_t u = s.top(); s.pop();
    for (vertex_t v : adjlist[u]) {
      if (seen.count(v)) continue;
      seen.insert(v);
      s.push(v);
    }
  }

  return seen.size() == lvs.size();
}

size_t num_leaves(graph_t g) {
  auto [vs, es] = g;
  map<vertex_t, size_t> cardinality;
  for (auto [u, v] : es) {
    ++cardinality[u];
    ++cardinality[v];
  }
  size_t count = 0;
  for (auto [k, v] : cardinality) {
    if (v == 1) ++count;
  }
  return count;
}

set<interval_t> random_connected_interval_set(size_t seed, size_t num) {
  default_random_engine rng(seed);
  uniform_int_distribution<uint8_t> bool_dist(0, 1);

  vector<coord_t> open;
  set<interval_t> is;
  coord_t limit = 2 * num;
  for (coord_t coord = 0; coord < limit; ++coord) {
    bool must_close = (limit - coord) == open.size();
    bool must_open = open.size() <= 1 && !must_close;
    bool do_open = must_open || (!must_close && bool_dist(rng));
    if (do_open) {
      open.push_back(coord);
    } else {
      auto it = open.begin() + uniform_int_distribution<size_t>(0, open.size()-1)(rng);
      is.emplace(*it, coord);
      open.erase(it);
    }
  }

  return is;
}

graph_t interval_graph_from_set(set<interval_t> verts) {
  struct event_t {
    interval_t interval;
    bool open;
    event_t(interval_t interval, bool open) : interval(interval), open(open) {}
    weak_ordering operator<=>(const event_t& rhs) const {
      coord_t lc = open ? interval.lower : interval.upper;
      coord_t rc = rhs.open ? rhs.interval.lower : rhs.interval.upper;
      if (lc == rc) return rhs.open <=> open;
      return lc <=> rc;
    }
  };

  vector<event_t> events;
  for (auto interval : verts) {
    events.emplace_back(interval, true);
    events.emplace_back(interval, false);
  }
  sort(events.begin(), events.end());

  set<edge_t> edges;
  set<interval_t> curr;
  for (auto event : events) {
    if (event.open) {
      for (auto interval : curr) {
        edges.insert(make_edge(event.interval, interval));
      }
      curr.insert(event.interval);
    } else {
      curr.erase(event.interval);
    }
  }

  return {verts, edges};
}

graph_t random_connected_interval_graph(size_t seed, size_t verts) {
  return interval_graph_from_set(random_connected_interval_set(seed, verts));
}

optional<graph_t> interval_mist_naive(graph_t g) {
  size_t best_leaves = numeric_limits<size_t>::max();
  optional<graph_t> best;

  auto [vs, es] = g;
  size_t limit = vs.size() - 1;
  vector<edge_t> edges(es.begin(), es.end());

  typedef pair<set<edge_t>, size_t> state_t;
  stack<state_t> s;
  s.push(make_pair(set<edge_t>(), 0));
  while (!s.empty()) {
    auto [es_prime, i] = s.top(); s.pop();
    if (es_prime.size() == limit) {
      auto g_prime = make_pair(vs, es_prime);
      if (is_spanning_tree(g_prime, g)) {
        size_t leaves = num_leaves(g_prime);
        if (leaves < best_leaves) {
          best_leaves = leaves;
          best = g_prime;
        }
      }
    } else if (es_prime.size() < limit && i < edges.size()) {
      s.push(make_pair(es_prime, i+1));
      es_prime.insert(edges[i]);
      s.push(make_pair(es_prime, i+1));
    }
  }

  return best;
}

vector<vector<vertex_t>> interval_path_cover(graph_t g) {
  auto [vs, es] = g;

  map<vertex_t, vector<vertex_t>> adjlist;
  for (auto [u, v] : es) {
    assert(vs.count(u));
    assert(vs.count(v));
    adjlist[u].push_back(v);
    adjlist[v].push_back(u);
  }

  for (auto& [k, v] : adjlist) {
    sort(v.begin(), v.end());
  }

  set<vertex_t> covered;
  set<vertex_t> ordered(vs.begin(), vs.end());

  vector<vector<vertex_t>> paths(1);
  paths.back().push_back(*ordered.begin());
  covered.insert(paths.back().back());
  ordered.erase(ordered.begin());

  while (!ordered.empty()) {
    vertex_t curr = paths.back().back();
    optional<vertex_t> succ;
    for (auto v : adjlist[curr]) {
      if (!covered.count(v)) {
        succ = v;
        break;
      }
    }
    if (!succ) {
      succ = *ordered.begin();
      paths.push_back(vector<vertex_t>());
    }
    paths.back().push_back(succ.value());
    ordered.erase(succ.value());
    covered.insert(succ.value());
  }

  return paths;
}

set<edge_t> path_to_edge_set(vector<vertex_t> p) {
  set<edge_t> result;
  for (size_t i = 1; i < p.size(); ++i) {
    result.insert(make_edge(p[i-1], p[i]));
  }
  return result;
}

optional<graph_t> interval_mist_path_cover(graph_t g) {
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
  auto tvs = set<vertex_t>(pc.back().begin(), pc.back().end());
  auto tes = path_to_edge_set(pc.back());
  pc.pop_back();
  // while Pc not empty:
  while (!pc.empty()) {
    // Choose q from Pc where q intersects Tc
    auto qit = pc.begin();
    for (; qit != pc.end(); ++qit) {
      vertex_t tv_lmost = *tvs.begin();
      vertex_t tv_rmost = *tvs.rbegin();
      if (any_of(qit->begin(), qit->end(), [tv_lmost, tv_rmost](vertex_t qv){ return tv_lmost < qv && qv < tv_rmost; })) break;

      vertex_t qv_lmost = *min_element(qit->begin(), qit->end());
      vertex_t qv_rmost = *max_element(qit->begin(), qit->end());
      if (any_of(tvs.begin(), tvs.end(), [qv_lmost, qv_rmost](vertex_t tv){ return qv_lmost < tv && tv < qv_rmost; })) break;

      // TODO: Their definition of intersects doesn't actually work here, try with normal definition?
      // bool intersects = false;
      // for (auto tv : tvs) {
      //   for (auto qv : *qit) {
      //     if (intervals_intersect(tv, qv)) {
      //       cerr << "\t(" << tv.lower << ", " << tv.upper << ") intersects (" << qv.lower << ", " << qv.upper << ")" << endl;
      //       intersects = true;
      //       break;
      //     }
      //   }
      //   if (intersects) break;
      // }
      // if (intersects) break;
    }
    if (qit == pc.end()) return {};
    auto qvs = set<vertex_t>(qit->begin(), qit->end());
    auto qes = path_to_edge_set(*qit);
    // get leftMost(Tc) and leftMost(q) for next steps
    vertex_t tv_lmost = *tvs.begin();
    vertex_t qv_lmost = *qvs.begin();
    // if leftMost(q) < leftMost(Tc):
    if (qv_lmost.upper < tv_lmost.upper) {
      // Choose w \in V(q) adjacent to v_leftMost(Tc)
      optional<vertex_t> w;
      for (auto qv : qvs) {
        if (intervals_intersect(qv, tv_lmost)) {
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
        cerr << "intersect (" << tv_lmost.lower << ", " << tv_lmost.upper << ")" << endl;
        return {};
      }
      // Update Tc by adding edge between these to connect q to Tc tree
      tvs.insert(qvs.begin(), qvs.end());
      tes.insert(qes.begin(), qes.end());
      tes.insert(make_edge(w.value(), tv_lmost));
    }
    // if leftMost(q) > leftMost(Tc):
    if (qv_lmost.upper > tv_lmost.upper) {
      // Choose w \in V(Tc) adjacent to v_leftMost(q)
      optional<vertex_t> w;
      for (auto tv : tvs) {
        if (intervals_intersect(tv, qv_lmost)) {
          w = tv;
          break;
        }
      }
      if (!w) return {};
      // Update Tc by adding edge between these to connect q to Tc tree
      tvs.insert(qvs.begin(), qvs.end());
      tes.insert(qes.begin(), qes.end());
      tes.insert(make_edge(w.value(), qv_lmost));
    }
    // Pc <- Pc \ {q}
    pc.erase(qit);
  }
  // return Tc
  return make_pair(set<vertex_t>(tvs.begin(), tvs.end()), tes);
}

optional<graph_t> interval_mist_greedy(graph_t g) {
  auto [vs, es] = g;

  // Construct adjacency list
  map<vertex_t, set<vertex_t>> adjlist;
  for (auto [u, v] : es) {
    assert(vs.count(u));
    assert(vs.count(v));
    adjlist[u].insert(v);
    adjlist[v].insert(u);
  }

  // Remaining vertices to add to tree and edges in tree so far
  set<vertex_t> todo(vs.begin(), vs.end());
  set<edge_t> tes;
  set<vertex_t> tvs;

  // Start with naive tree of interval with leftmost right endpoint (LRE)
  // Invariant: prev is always a leaf in the tree
  vertex_t prev = *todo.begin();
  todo.erase(todo.begin());
  tvs.insert(prev);

  // While there are vertices not yet in tree
  while (!todo.empty()) {
    // Select adjacent vertex to prev with LRE
    optional<vertex_t> curr;
    for (auto v : adjlist[prev]) {
      if (todo.count(v)) {
        curr = v;
        break;
      }
    }

    if (curr) {
      // Greedily attach curr to leaf, making prev internal (unless root)
      tvs.insert(curr.value());
      tes.insert(make_edge(prev, curr.value()));
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
            tes.insert(make_edge(u, v));
            todo.erase(u);
            prev = u;
            done = true;
            break;
          }
        }
        if (done) break;
      }
    }

    graph_t t = {tvs, tes};
    auto tg = interval_graph_from_set(tvs);
    assert(is_spanning_tree(t, tg));
    auto mist_naive = interval_mist_naive(tg).value();
    assert(num_leaves(t) == num_leaves(mist_naive));
  }

  return {{vs, tes}};
}

void cerr_edges(set<edge_t> es) {
  for (auto e : es) {
    cerr << e.first << " - " << e.second << endl;
  }
}

void cerr_graph(graph_t g) {
  auto [vs, es] = g;

  for (auto v : vs) {
    cerr << v << endl;
  }
  cerr << endl;
  cerr_edges(g.second);
}

void cerr_pc(vector<vector<vertex_t>> pc) {
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

bool test_greedy_eq_mist(graph_t g) {
  auto [vs, es] = g;
  auto maybe_mist_greedy = interval_mist_greedy(g);
  auto maybe_mist_naive = interval_mist_naive(g);

  if (!maybe_mist_greedy.has_value() && !maybe_mist_naive.has_value()) {
    assert(false);
    return true;
  }
  if (maybe_mist_greedy.has_value() != maybe_mist_naive.has_value()) {
    cerr_graph(g);
    cerr << endl;
    cerr << "GREEDY: " << (maybe_mist_greedy.has_value() ? "found solution" : "no solution found") << endl;
    cerr << "NAIVE: " << (maybe_mist_naive.has_value() ? "found solution" : "no solution found") << endl;
    return false;
  }

  auto mist_greedy = maybe_mist_greedy.value();
  auto mist_naive = maybe_mist_naive.value();
  if (num_leaves(mist_greedy) != num_leaves(mist_naive)) {
    cerr_graph(g);
    cerr << endl;
    cerr << "GREEDY: " << num_leaves(mist_greedy) << " leaves" << endl;
    cerr_edges(mist_greedy.second);
    cerr << endl;
    cerr << "NAIVE: " << num_leaves(mist_naive) << " leaves" << endl;
    return false;
  }

  return true;
}

bool test_greedy_eq_mist(size_t seed, size_t num) {
  auto g = random_connected_interval_graph(seed, num);
  if (!test_greedy_eq_mist(g)) {
    cerr << "test_greedy_eq_mist failed with seed = " << seed << ", num = " << num << endl;
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

bool test_pc_eq_mist(graph_t g) {
  auto [vs, es] = g;
  auto pc = interval_path_cover(g);
  auto maybe_mist_greedy = interval_mist_greedy(g);
  assert(maybe_mist_greedy.has_value());
  auto mist_greedy = maybe_mist_greedy.value();
  assert(is_spanning_tree(mist_greedy, g));
  if (pc.size() + 1 != num_leaves(mist_greedy)) {
    auto mist_naive = interval_mist_naive(g).value();
    assert(is_spanning_tree(mist_naive, g));
    assert(num_leaves(mist_greedy) == num_leaves(mist_naive));

    cerr_graph(g);
    cerr << endl;
    cerr_pc(pc);
    cerr << endl;
    cerr << "GREEDY: " << num_leaves(mist_greedy) << " leaves" << endl;
    cerr_edges(mist_greedy.second);
    cerr << endl;
    cerr << "NAIVE: " << num_leaves(mist_naive) << " leaves" << endl;

    return false;
  }
  return true;
}

bool test_pc_eq_mist(size_t seed, size_t num) {
  auto g = random_connected_interval_graph(seed, num);
  if (!test_pc_eq_mist(g)) {
    cerr << "test_pc_eq_mist failed with seed = " << seed << ", num = " << num << endl;
    return false;
  }
  return true;
}

bool fuzz_pc_eq_mist(size_t seed, size_t count, size_t num) {
  default_random_engine rng(seed);

  for (size_t i = 0; i < count; ++i) {
    if (!test_pc_eq_mist(rng(), num)) return false;
  }
  return true;
}

void pc_eq_mist_counterexample() {
  set<interval_t> vs = {
    Interval(0, 2),
    Interval(1, 6),
    Interval(3, 4),
    Interval(5, 10),
    Interval(7, 8),
    Interval(9, 11),
  };
  auto g = interval_graph_from_set(vs);
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
