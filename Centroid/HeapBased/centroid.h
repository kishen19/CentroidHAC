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
  I.set_start();
  indexType start_point = I.get_start();
  QueryParams QP((long) 2, BP.L,  (double) 10.00, (long) Points.size(), (long) G.max_degree());
  size_t rem = n;
  auto nearest_neighbor = [&](indexType u) {
    auto out = beam_search(Points[u], G, Points, start_point, QP, &uf).first.first;
    parlay::sort_inplace(out, [&](auto x, auto y){
      return (x.second < y.second)?1:((x.second == y.second && x.first < y.first)?1:0);
    });
    int ind = 0;
    while (ind < out.size()){
      if (out[ind].first != u){ return out[ind]; }
      ind++;
    }
    std::cout << "No NN found for " << u << std::endl;
    std::cout << "rem = " << rem << std::endl; 
    for (int i=0; i< G[u].size(); i++){
      std::cout << G[u][i] << " " << uf.find_compress(G[u][i]) << std::endl; 
    }
    abort();
  };

  // Init Priority Queue
  using kv = std::tuple<distanceType,indexType,indexType>;
  auto h = parlay::sequence<kv>::from_function(n,[&](size_t i){
    auto cur_best = nearest_neighbor(i);
    return std::make_tuple(cur_best.second,i,cur_best.first);
  });
  std::priority_queue<kv, std::vector<kv>, std::greater<kv>> H(h.begin(), h.end());
  std::cout << "Heap Init Done" << std::endl;

  // Centroid Process
  std::cout << "Starting Centroid Process" << std::endl;
  auto merge_clusters = [&](indexType u, indexType v) -> indexType {
    indexType w = uf.unite(u,v);
    Points[w].centroid(Points[w^u^v]); // Update u (or v) to the centroid
    auto cand = parlay::sequence<indexType>::from_function(G[w].size()+G[w^u^v].size(),[&](size_t i){
      if (i < G[w].size()) {return uf.find_compress(G[w][i]);}
      else {return uf.find_compress(G[w^u^v][i-G[w].size()]);}
    });
    auto filtered_cand = parlay::filter(cand, [&](indexType x){return x != w;});
    auto new_nbhs = I.robustPrune(w, filtered_cand, G, Points, BP.alpha, false);
    G[w].update_neighbors(new_nbhs);
    return w;
  };
  
  indexType u,v,w,u_orig,v_orig;
  distanceType dist;
  distanceType total_dist = 0;
  double eps = 0.1;
  while (rem > 1){
    kv best = H.top();
    H.pop();
    std::tie(dist, u_orig, v_orig) = best;
    u = uf.find_compress(u_orig); // Centroid of u
    v = uf.find_compress(v_orig); // Centroid of v
    if (u == v || u!= u_orig){ // u_orig & v_orig in same cluster already, or u_orig is not active
      continue;
    } else if (v != v_orig) { // v_orig is not active
      // recompute dist and check if within 1+eps (instead of beam_search)
      auto new_dist = Points[u].distance(Points[v]);
      if (new_dist <= (1+eps)*dist){ // If d(u,v) is at most (1+eps)*d(u,v_orig)<=(1+eps)*opt
        w = merge_clusters(u,v);
        total_dist += new_dist;
        rem--;
      } else { // Search nearest point to u, say v', and check if d(u,v')<=(1+eps)*dist <= (1+eps)opt
        std::pair<indexType,distanceType> cur_best = nearest_neighbor(u);
        if (cur_best.second <= (1+eps)*dist){
          w = merge_clusters(u,cur_best.first);
          total_dist += cur_best.second;
          rem--;
        } else{
          w = u;
        }
      }
      if (rem == 1){ break; }
      std::pair<indexType,distanceType> closest_to_w = nearest_neighbor(w);
      H.push(std::make_tuple(closest_to_w.second, w, closest_to_w.first));
    } else{ // Always merge
      auto w = merge_clusters(u,v);
      total_dist += dist;
      rem--;
      if (rem == 1){ break; }
      std::pair<indexType,distanceType> closest_to_w = nearest_neighbor(w);
      H.push(std::make_tuple(closest_to_w.second, w, closest_to_w.first));
    }
  }
  std::cout << "Centroid Process Done" << std::endl;
  std::cout << "Total Distance: " << total_dist << std::endl;
}