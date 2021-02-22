#include <bits/stdc++.h>

using namespace std;

typedef size_t coord_t;
typedef pair<coord_t, coord_t> interval_t;
typedef interval_t vertex_t;
typedef pair<vertex_t, vertex_t> edge_t;
typedef pair<set<vertex_t>, set<edge_t>> graph_t;

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

int main() {
  default_random_engine rng(792451836);

  for (size_t i = 0; i < 10000; ++i) {
    size_t num = 1000;
    auto is = random_connected_interval_set(rng(), num);
    assert(is.size() == num);
  }
}