#pragma once
#include "utils/union_find.h"
#include "utils/nn_types.h"

/* 
  Exact CentroidHAC Implementation
  Running time: O(n^2 log n)
*/
template<typename PointRange, typename indexType>
double CentroidHAC_Exact(PointRange &Points, char *DendFile = nullptr) {
  using Point = typename PointRange::pT;
  using distanceType = typename Point::distanceType;
  using kv = std::tuple<distanceType,indexType,indexType>; // Heap element type
  
  parlay::internal::timer t("Centroid HAC (Exact)");

  /* Init */
  size_t n = Points.size();
  // Representative of the cluster containing i
  // Initially, each point is its own representative
  auto rep = parlay::sequence<indexType>::from_function(n, [&](indexType i){return i;});
  auto parent = parlay::sequence<indexType>::from_function(2*n-1, 
    [&](size_t i){return i;});
  auto merge_cost = parlay::sequence<distanceType>::uninitialized(n-1);
  // UnionFind for maintaining the current cluster information
  auto uf = union_find<indexType>(n);
  // Nearest Neighbor DS
  auto NN = nn_exact<PointRange, indexType>();
  auto h = parlay::sequence<kv>::from_function(n,[&](size_t i){
    auto cur_best = NN.nearest_neighbor(i, Points, &uf);
    return std::make_tuple(cur_best.second,i,cur_best.first);
  });
  std::priority_queue<kv, std::vector<kv>, std::greater<kv>> H(h.begin(), h.end());
  t.next("Init Done");

  /* CentroidHAC Process */
  indexType u, v, w, u_orig, v_orig;
  distanceType dist, total_dist = 0;
  size_t cur = 0;
  while (cur < n-1){
    if (cur % 10000 == 0){
      std::cout << "cur: " << cur << ", total_dist: " << total_dist << std::endl;
    }
    kv best = H.top(); H.pop();
    std::tie(dist, u_orig, v_orig) = best;
    u = uf.find_compress(u_orig); // Centroid of u
    v = uf.find_compress(v_orig); // Centroid of v
    // If either u_orig or v_orig is not active, continue
    if ((u!=u_orig) || (v != v_orig)){ 
      continue;
    } else{ // merge step
      auto new_dist = Points[u].distance(Points[v]);
      if (new_dist <= dist){ // If dist is actual distance between u and v
        w = NN.merge_clusters(u, v, Points, &uf);
        parent[rep[u]] = cur + n;
        parent[rep[v]] = cur + n;
        rep[w] = cur + n;
        merge_cost[cur] = new_dist;
        total_dist += new_dist;
        cur++;
      } else{ // Otherwise, go to next pair
        w = u;
      }
      if (cur < n-1){// Add the nearest neighbor of w to the heap
        std::pair<indexType,distanceType> closest_to_w = NN.nearest_neighbor(w, Points, &uf);
        H.push(std::make_tuple(closest_to_w.second, w, closest_to_w.first));
      }
    }
  }
  t.next("Centroid Done");
  double total_time = t.total_time();
  std::cout << std::fixed << "Total Cost: " << total_dist << std::endl;
  /* Write Dendrogram to File */
  if (DendFile != nullptr){
    std::ofstream dendrogram_file;
    dendrogram_file.open(DendFile);
    dendrogram_file << n << std::endl;
    for (size_t i = 0; i < 2*n-1; i++){
      dendrogram_file << parent[i] << std::endl;
    }
    for (size_t i = 0; i < n-1; i++){
      dendrogram_file << merge_cost[i] << std::endl;
    }
  }
  return total_time;
}