// This code is part of the Problem Based Benchmark Suite (PBBS)
// Copyright (c) 2011 Guy Blelloch and the PBBS team
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights (to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to
// the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
// LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
// OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#pragma once

#include <algorithm>

#include "parlay/parallel.h"
#include "parlay/primitives.h"
#include "parlay/random.h"

#include "Centroid/utils/NSGDist.h"
#include "Centroid/utils/beamSearch.h"
#include "Centroid/utils/check_nn_recall.h"
#include "Centroid/utils/parse_results.h"
#include "Centroid/utils/stats.h"
#include "Centroid/utils/types.h"
#include "Centroid/utils/graph.h"
#include "Centroid/common/index.h"
#include "Centroid/common/union_find.h"


template<typename Point, typename PointRange, typename indexType>
void Centroid(Graph<indexType> &G, long k, BuildParams &BP,
         char *ofile,
         bool graph_built, PointRange &Points) {
  using findex = knn_index<Point, PointRange, indexType>;
  using distanceType = typename Point::distanceType;
  // parlay::internal::timer t("Centroid");

  // Build Index
  std::cout << "Building Index" << std::endl;
  size_t n = G.size();
  findex I(BP);
  stats<unsigned int> BuildStats(n);
  if(!graph_built){
    I.build_index(G, Points, BuildStats);
    if (ofile != nullptr) G.save(ofile);
  }
  std::cout << "Index Built" << std::endl;

  // Init UF and other stuff
  auto uf = union_find<indexType>(n);

  // Init Priority Queue
  using kv = std::tuple<distanceType,indexType,indexType>;
  I.set_start();
  indexType start_point = I.get_start();
  QueryParams QP((long) 2, BP.L,  (double) 1.00, (long) Points.size(), (long) G.max_degree());
  auto h = parlay::sequence<kv>::from_function(n,[&](size_t i){
    auto out = beam_search(Points[i], G, Points, start_point, QP);
    if (i<10){
      std::cout << i << " " << out.first.first[0].first << " - " << out.first.first[0].second << ", " << out.first.first[1].first << " - " << out.first.first[1].second << std::endl;
    }
    if (out.first.first[0].first!=i){
      return std::make_tuple(out.first.first[0].second,i,out.first.first[0].first);
    } else {
      return std::make_tuple(out.first.first[1].second,i,out.first.first[1].first);
    }
  });
  for (int i=0; i<10; i++){
    std::cout << std::get<0>(h[i]) << " " << std::get<1>(h[i]) << " " << std::get<2>(h[i]) << std::endl;
  }
  std::priority_queue<kv, std::vector<kv>, std::greater<kv>> H(h.begin(), h.end());
  std::cout << "Heap Init Done" << std::endl;

  // Centroid Process
  std::cout << "Starting Centroid Process" << std::endl;
  size_t rem = n;
  indexType u,v,u_orig,v_orig;
  distanceType dist;
  distanceType total_dist = 0;
  while (rem > 1){
    kv best = H.top();
    H.pop();
    u_orig = std::get<1>(best);
    v_orig = std::get<2>(best);
    std::cout << "u: " << u_orig << ", v: " << v_orig << ", dist: " << dist << std::endl;
    u = uf.find_compress(u_orig);
    v = uf.find_compress(v_orig);
    if (u == v || u!=u_orig || v!=v_orig){
      continue;
    } else {
      auto out = beam_search(Points[u], G, Points, I.get_start(), QP);
      std::pair<indexType,distanceType> cur_best;
      if (out.first.first[0].first != u){
        cur_best = out.first.first[0];
      } else{
        cur_best = out.first.first[1];
      }
      if (cur_best.second < dist){ // cur point is not nearest TODO: use a multiplier eps
        H.push(std::make_tuple(cur_best.second, u, cur_best.first));
      } else{
        auto w = uf.unite(u,v);
        Points[w].centroid(Points[w^u^v]); // Update u (or v) to the centroid
        total_dist += dist;
        rem--;
      }
    }
  }
  std::cout << "Centroid Process Done" << std::endl;
  std::cout << "Total Distance: " << total_dist << std::endl;
}