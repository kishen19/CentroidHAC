#pragma once
#include <algorithm>
#include <iomanip>

#include "src/vamana/utils/beamSearch.h"
#include "src/vamana/utils/stats.h"
#include "src/vamana/utils/types.h"
#include "src/utils/graph.h"
#include "src/utils/union_find.h"
#include "src/utils/nn_types.h"

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
  parlay::internal::timer pt("Timer");

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
  size_t num_searches = 0, num_missed_merges=0, num_merges = 0, uf_ops = 0, parallel_searches = 0;
  double merge_time = 0.0, uf_time = 0.0, search_time = 0.0;

  auto h = parlay::sequence<kv>::from_function(n,[&](indexType i){
    if (uf.find_compress(i) != i){
      return std::make_tuple(std::numeric_limits<distanceType>::max(),i,i);
    } // Ignore non-centroids (non-representatives)
    auto cur_best = NN.nearest_neighbor(i, Points, &uf);
    return std::make_tuple(cur_best.second,i,cur_best.first);
  });
  parallel_searches+=n;
  auto threshold = std::get<0>(parlay::reduce(h, parlay::minm<kv>()))*one_plus_eps;
  auto bucket = parlay::filter(h, [&](kv x){
    return std::get<0>(x) <= threshold;
  });
  size_t bucket_size = bucket.size();

  auto repeated_merge = [&](indexType u_){
    pt.start();
    std::pair<indexType,distanceType> cur_best = NN.nearest_neighbor(u_, Points, &uf);
    search_time += pt.stop();
    num_searches++;
    while (cur < n-1 && cur_best.second <= threshold){
      pt.start();
      auto w_ = NN.merge_clusters(u_, cur_best.first, Points, &uf);
      merge_time += pt.stop();
      num_merges++;
      parent[rep[u_]] = cur + n;
      parent[rep[cur_best.first]] = cur + n;
      rep[w_] = cur + n;
      merge_cost[cur] = cur_best.second;
      total_dist += cur_best.second;
      cur++;
      u_ = w_;
      if (cur < n-1) {
        pt.start();
        cur_best = NN.nearest_neighbor(u_, Points, &uf);
        search_time += pt.stop();
        num_searches++;
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
      pt.start();
      u = uf.find_compress(u_orig); // Centroid of u
      v = uf.find_compress(v_orig); // Centroid of v
      uf_time += pt.stop();
      uf_ops++;
      if (u == v){ // u_orig & v_orig in same cluster already, skip
        continue;
      } else {
        // recompute dist and check if within 1+eps (instead of beam_search)
        auto new_dist = Points[u].distance(Points[v]);
        if (new_dist <= threshold){ // If d(u,v) is still within threshold, merge
          pt.start();
          w = NN.merge_clusters(u, v, Points, &uf);
          merge_time += pt.stop();
          num_merges++;
          parent[rep[u]] = cur + n;
          parent[rep[v]] = cur + n;
          rep[w] = cur + n;
          merge_cost[cur] = new_dist;
          total_dist += new_dist; // TODO: deprecate after testing
          cur++;
          if (cur < n-1) repeated_merge(w);
        } else { // Search nearest point to u, say v', and check if d(u,v') is still within threshold
          pt.start();
          std::pair<indexType,distanceType> cur_best = NN.nearest_neighbor(u, Points, &uf);
          search_time += pt.stop();
          num_searches++;
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
            num_missed_merges++;
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
    auto par_searches = parlay::count_if(h, [&](kv x){
      return std::get<0>(x) != std::numeric_limits<distanceType>::max();
    });
    parallel_searches+=par_searches;
    if (threshold == 0){
      threshold = std::get<0>(parlay::reduce(h, parlay::minm<kv>()))*one_plus_eps;
    }
    bucket = parlay::filter(h, [&](kv x){
      return std::get<0>(x) <= threshold;
    });
    num_missed_merges += (par_searches - bucket.size());
    bucket_size = bucket.size();
    iter++;
  }
  t.next("Centroid Done");
  double total_time = t.total_time();
  std::cout << std::fixed << "Total Cost: " << total_dist << std::endl;
  std::cout << std::fixed << "Iterations: " << iter << std::endl;
  std::cout << "Number of Searches: " << num_searches << std::endl;
  std::cout << "Number of Merges: " << num_merges << std::endl;
  std::cout << "Number of Missed Merges: " << num_missed_merges << std::endl;
  std::cout << "UF Operations: " << uf_ops << std::endl;
  std::cout << "Parallel Searches: " << parallel_searches << std::endl;
  std::cout << "Search Time: " << search_time << std::endl;
  std::cout << "Merge Time: " << merge_time << std::endl;
  std::cout << "UF Time: " << uf_time << std::endl;
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