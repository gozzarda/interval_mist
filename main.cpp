#include <bits/stdc++.h>

using namespace std;

typedef size_t coord_t;
struct Interval {
  coord_t lower, upper;
  Interval(coord_t lwr, coord_t upr) : lower(lwr), upper(upr) {}
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

  map<vertex_t, set<vertex_t>> adjlist;
  for (auto [u, v] : es) {
    assert(vs.count(u));
    assert(vs.count(v));
    adjlist[u].insert(v);
    adjlist[v].insert(u);
  }

  set<vertex_t> todo(vs.begin(), vs.end());
  set<edge_t> tes;

  vertex_t prev = *todo.begin();
  todo.erase(todo.begin());

  while (!todo.empty()) {
    optional<vertex_t> curr;
    for (auto v : adjlist[prev]) {
      if (todo.count(v)) {
        curr = v;
        break;
      }
    }
    if (curr) {
      tes.insert(make_edge(prev, curr.value()));
      todo.erase(curr.value());
      prev = curr.value();
    } else {
      bool done = false;
      for (auto u : todo) {
        for (auto v : adjlist[u]) {
          if (!todo.count(v)) {
            tes.insert(make_edge(u, v));
            prev = v;
            done = true;
            break;
          }
        }
        if (done) break;
      }
    }
  }

  return {{vs, tes}};
}

int main() {
  default_random_engine rng(792451839);

  for (size_t i = 0; i < 1000; ++i) {
    size_t num = 10;
    auto g = random_connected_interval_graph(rng(), num);
    auto [vs, es] = g;
    assert(vs.size() == num);
    for (auto v : vs) {
      cerr << "(" << v.lower << ", " << v.upper << ")" << endl;
    }
    cerr << endl;
    for (auto e : es) {
      cerr << "(" << e.first.lower << ", " << e.first.upper << ") - ";
      cerr << "(" << e.second.lower << ", " << e.second.upper << ")" << endl;
    }
    auto t = interval_mist_naive(g).value();
    assert(is_spanning_tree(t, g));
    cerr << endl << "MIST: " << num_leaves(t) << " leaves" << endl;
    auto maybe_tt = interval_mist_path_cover(g);
    assert(maybe_tt.has_value());
    auto tt = maybe_tt.value();
    cerr << "TEST: " << num_leaves(tt) << " leaves" << endl;
    auto [tvs, tes] = tt;
    for (auto e : tes) {
      cerr << "(" << e.first.lower << ", " << e.first.upper << ") - ";
      cerr << "(" << e.second.lower << ", " << e.second.upper << ")" << endl;
    }
    assert(is_spanning_tree(tt, g));
    assert(num_leaves(t) == num_leaves(tt));
    // auto pc = interval_path_cover(g);
    // cerr << endl << "Path Cover:" << endl;
    // for (auto p : pc) {
    //   for (auto v : p) {
    //     cerr << "  (" << v.lower << ", " << v.upper << ")";
    //   }
    //   cerr << endl;
    // }
  }
}
