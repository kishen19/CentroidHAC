#pragma once
#include <algorithm>

#include "Centroid/ANN/utils/beamSearch.h"
#include "Centroid/ANN/utils/stats.h"
#include "Centroid/ANN/utils/types.h"
// #include "Centroid/utils/check_nn_recall.h"
// #include "Centroid/utils/parse_results.h"
#include "Centroid/utils/graph.h"
#include "Centroid/utils/union_find.h"


template<typename Point, typename PointRange, typename indexType, class nn_type>
void Centroid(char *ofile,
    PointRange &Points, nn_type &NN) {
  
  using distanceType = typename Point::distanceType;
  using kv = std::tuple<distanceType,indexType,indexType>; // Heap element type
  // parlay::internal::timer t("Centroid");

  // Build Index
  NN.init(Points, ofile);

  // Init UF and other stuff
  size_t n = Points.size();
  auto uf = union_find<indexType>(n);
  
  // Init Priority Queue
  auto h = parlay::sequence<kv>::from_function(n,[&](size_t i){
    auto cur_best = NN.nearest_neighbor(i, Points, &uf, n);
    return std::make_tuple(cur_best.second,i,cur_best.first);
  });
  std::priority_queue<kv, std::vector<kv>, std::greater<kv>> H(h.begin(), h.end());
  std::cout << "Heap Init Done" << std::endl;

  // Centroid Process
  std::cout << "Starting Centroid Process" << std::endl;
  indexType u, v, w, u_orig, v_orig;
  distanceType dist;
  distanceType total_dist = 0;
  double eps = 0.1;
  size_t rem = n;
  // main loop
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
        w = NN.merge_clusters(u, v, Points, &uf);
        total_dist += new_dist;
        rem--;
      } else { // Search nearest point to u, say v', and check if d(u,v')<=(1+eps)*dist <= (1+eps)opt
        std::pair<indexType,distanceType> cur_best = NN.nearest_neighbor(u, Points, &uf, rem);
        if (cur_best.second <= (1+eps)*dist){
          w = NN.merge_clusters(u, cur_best.first, Points, &uf);
          total_dist += cur_best.second;
          rem--;
        } else{
          w = u;
        }
      }
      if (rem == 1){ break; }
      std::pair<indexType,distanceType> closest_to_w = NN.nearest_neighbor(w, Points, &uf, rem);
      H.push(std::make_tuple(closest_to_w.second, w, closest_to_w.first));
    } else{ // Always merge
      auto w = NN.merge_clusters(u, v, Points, &uf);
      total_dist += dist;
      rem--;
      if (rem == 1){ break; }
      std::pair<indexType,distanceType> closest_to_w = NN.nearest_neighbor(w, Points, &uf, rem);
      H.push(std::make_tuple(closest_to_w.second, w, closest_to_w.first));
    }
  }
  std::cout << "Centroid Process Done" << std::endl;
  std::cout << "Total Distance: " << total_dist << std::endl;
}