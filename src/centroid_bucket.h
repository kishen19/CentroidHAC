#pragma once
#include <algorithm>
#include <iomanip>

#include "vamana/utils/beamSearch.h"
#include "vamana/utils/stats.h"
#include "vamana/utils/types.h"
#include "utils/graph.h"
#include "utils/union_find.h"
#include "utils/nn_types.h"

/*
  Approximate CentroidHAC Implementation
  Running time: O(n log n)
*/
template<typename PointRange, typename indexType, class nn_type>
double CentroidHAC_Bucket(PointRange &Points, nn_type &NN, 
      double eps = 0.2, char *IndexFile = nullptr, char *DendFile = nullptr){
  using Point = typename PointRange::pT;
  using distanceType = typename Point::distanceType;
  using kv = std::tuple<distanceType,indexType,indexType>; // Heap element type
  
  parlay::internal::timer t("Centroid HAC (Approx)");

  /* Build Index */
  NN.init(Points, IndexFile);
  t.next("Index Built");

  /* Init */
  size_t n = Points.size();
  // Representative of the cluster containing i
  // Initially, each point is its own representative
  auto rep = parlay::sequence<indexType>::from_function(n, [&](size_t i){return i;});
  auto parent = parlay::sequence<indexType>::from_function(2*n-1, 
    [&](size_t i){return i;});
  auto merge_cost = parlay::sequence<distanceType>::uninitialized(n-1);
  // UnionFind for maintaining the current cluster information
  auto uf = union_find<indexType>(n);
  t.next("Init Done");

  /* CentroidHAC Process */
  indexType u, v, w, u_orig, v_orig;
  distanceType dist, total_dist = 0;
  size_t cur = 0;
  double one_plus_eps = 1+eps;

  auto h = parlay::sequence<kv>::from_function(n,[&](indexType i){
    if (uf.find_compress(i) != i){
      return std::make_tuple(std::numeric_limits<distanceType>::max(),i,i);
    } // Ignore non-centroids (non-representatives)
    auto cur_best = NN.nearest_neighbor(i, Points, &uf);
    return std::make_tuple(cur_best.second,i,cur_best.first);
  });
  auto threshold = std::get<0>(parlay::reduce(h, parlay::minm<kv>()))*one_plus_eps;
  auto bucket = parlay::filter(h, [&](kv x){
    return std::get<0>(x) <= threshold;
  });
  size_t bucket_size = bucket.size();

  auto repeated_merge = [&](indexType u_){
    std::pair<indexType,distanceType> cur_best = NN.nearest_neighbor(u_, Points, &uf);
    while (cur < n-1 && cur_best.second <= threshold){
      auto w_ = NN.merge_clusters(u_, cur_best.first, Points, &uf);
      parent[rep[u_]] = cur + n;
      parent[rep[cur_best.first]] = cur + n;
      rep[w_] = cur + n;
      merge_cost[cur] = cur_best.second;
      total_dist += cur_best.second;
      cur++;
      u_ = w_;
      if (cur < n-1) {
        cur_best = NN.nearest_neighbor(u_, Points, &uf);
      }
    }
  };
  size_t iter = 0;
  while (cur < n-1){
    // Until bucket is empty
    while(bucket_size>0){
      kv best = bucket[bucket_size-1];
      bucket.pop_back();
      bucket_size--;
      std::tie(dist, u_orig, v_orig) = best;
      u = uf.find_compress(u_orig); // Centroid of u
      v = uf.find_compress(v_orig); // Centroid of v
      if (u == v){ // u_orig & v_orig in same cluster already, skip
        continue;
      } else {
        // recompute dist and check if within 1+eps (instead of beam_search)
        auto new_dist = Points[u].distance(Points[v]);
        if (new_dist <= threshold){ // If d(u,v) is still within threshold, merge
          w = NN.merge_clusters(u, v, Points, &uf);
          parent[rep[u]] = cur + n;
          parent[rep[v]] = cur + n;
          rep[w] = cur + n;
          merge_cost[cur] = new_dist;
          total_dist += new_dist; // TODO: deprecate after testing
          cur++;
          if (cur < n-1) repeated_merge(w);
        } else { // Search nearest point to u, say v', and check if d(u,v') is still within threshold
          std::pair<indexType,distanceType> cur_best = NN.nearest_neighbor(u, Points, &uf);
          if (cur_best.second <= threshold){
            w = NN.merge_clusters(u, cur_best.first, Points, &uf);
            parent[rep[u]] = cur + n;
            parent[rep[cur_best.first]] = cur + n;
            rep[w] = cur + n;
            merge_cost[cur] = cur_best.second;
            total_dist += cur_best.second;
            cur++;
            if (cur < n-1) repeated_merge(w);
          } else{
            continue;
          }
        }
      }
    }
    if (cur == n-1) break;
    threshold*=one_plus_eps;
    h = parlay::sequence<kv>::from_function(n,[&](indexType i){
      if (uf.find_compress(i) != i){
        return std::make_tuple(std::numeric_limits<distanceType>::max(),i,i);
      } // Ignore non-centroids (non-representatives)
      auto cur_best = NN.nearest_neighbor(i, Points, &uf);
      return std::make_tuple(cur_best.second,i,cur_best.first);
    });
    bucket = parlay::filter(h, [&](kv x){
      return std::get<0>(x) <= threshold;
    });
    bucket_size = bucket.size();
    iter++;
  }
  t.next("Centroid Done");
  double total_time = t.total_time();
  std::cout << std::fixed << "Total Cost: " << total_dist << std::endl;
  std::cout << std::fixed << "Iterations: " << iter << std::endl;
  /* Write Dendrogram to File */
  if (DendFile){
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