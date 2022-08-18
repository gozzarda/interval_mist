#include "path_cover.hpp"
#include <iostream>
#include <map>

namespace interval_mist::solvers::path_cover {

using Edge = Graph::Edge;

std::vector<std::vector<Vertex>> interval_path_cover(Graph g) {
  auto [vs, es] = g;

  std::map<Vertex, std::vector<Vertex>> adjlist;
  for (auto [u, v] : es) {
    assert(vs.count(u));
    assert(vs.count(v));
    adjlist[u].push_back(v);
    adjlist[v].push_back(u);
  }

  for (auto &[k, v] : adjlist) {
    sort(v.begin(), v.end());
  }

  std::set<Vertex> covered;
  std::set<Vertex> ordered(vs.begin(), vs.end());

  std::vector<std::vector<Vertex>> paths(1);
  paths.back().push_back(*ordered.begin());
  covered.insert(paths.back().back());
  ordered.erase(ordered.begin());

  while (!ordered.empty()) {
    Vertex curr = paths.back().back();
    std::optional<Vertex> succ;
    for (auto v : adjlist[curr]) {
      if (!covered.count(v)) {
        succ = v;
        break;
      }
    }
    if (!succ) {
      succ = *ordered.begin();
      paths.push_back(std::vector<Vertex>());
    }
    paths.back().push_back(succ.value());
    ordered.erase(succ.value());
    covered.insert(succ.value());
  }

  return paths;
}

std::set<Edge> path_to_edge_set(std::vector<Vertex> p) {
  std::set<Edge> result;
  for (size_t i = 1; i < p.size(); ++i) {
    result.insert(Edge(p[i - 1], p[i]));
  }
  return result;
}

std::optional<Graph> interval_mist_path_cover(Graph g) {
  auto [vs, es] = g;

  // Find a path cover P* of G
  auto pc = interval_path_cover(g);
  std::cerr << "PATH COVER:" << std::endl;
  for (auto p : pc) {
    for (auto v : p) {
      std::cerr << "  (" << v.lower << ", " << v.upper << ")";
    }
    std::cerr << std::endl;
  }
  // Tc <- {p | p \in P*}, Pc <- P* \ {p}
  auto tvs = std::set<Vertex>(pc.back().begin(), pc.back().end());
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
      //       std::cerr << "\t(" << tv.lower << ", " << tv.upper << ")
      //       intersects ("
      //       << qv.lower << ", " << qv.upper << ")" << std::endl; intersects =
      //       true; break;
      //     }
      //   }
      //   if (intersects) break;
      // }
      // if (intersects) break;
    }
    if (qit == pc.end())
      return {};
    auto qvs = std::set<Vertex>(qit->begin(), qit->end());
    auto qes = path_to_edge_set(*qit);
    // get leftMost(Tc) and leftMost(q) for next steps
    Vertex tv_lmost = *tvs.begin();
    Vertex qv_lmost = *qvs.begin();
    // if leftMost(q) < leftMost(Tc):
    if (qv_lmost.upper < tv_lmost.upper) {
      // Choose w \in V(q) adjacent to v_leftMost(Tc)
      std::optional<Vertex> w;
      for (auto qv : qvs) {
        if (qv.intersects(tv_lmost)) {
          w = qv;
          break;
        }
      }
      if (!w) {
        std::cerr << "ERROR:" << std::endl;
        std::cerr << "None of" << std::endl;
        for (auto qv : qvs) {
          std::cerr << "\t(" << qv.lower << ", " << qv.upper << ")"
                    << std::endl;
        }
        std::cerr << "intersect (" << tv_lmost.lower << ", " << tv_lmost.upper
                  << ")" << std::endl;
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
      std::optional<Vertex> w;
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
  return Graph(std::set<Vertex>(tvs.begin(), tvs.end()), tes);
}

} // namespace interval_mist::solvers::path_cover