#pragma once
#include <algorithm>
#include <iomanip>

#include "src/vamana/utils/beamSearch.h"
#include "src/vamana/utils/stats.h"
#include "src/vamana/utils/types.h"
#include "src/utils/graph.h"
#include "src/utils/union_find.h"
#include "src/utils/nn_types.h"
#include "src/utils/heap.h"

/*
  Approximate CentroidHAC Implementation
  Running time: O(n log n)
*/
template<typename PointRange, typename indexType, class nn_type>
double CentroidHAC(PointRange &Points, nn_type &NN, 
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
  size_t num_searches = 0, num_merges=0, num_missed_merges = 0, num_heap_ops = 0, uf_ops = 0, parallel_searches = 0;
  double search_time = 0.0, merge_time = 0.0, heap_time = 0.0, uf_time = 0.0;
  // Representative of the cluster containing i
  // Initially, each point is its own representative
  auto rep = parlay::sequence<indexType>::from_function(n, [&](size_t i){return i;});
  auto parent = parlay::sequence<indexType>::from_function(2*n-1, 
    [&](size_t i){return i;});
  auto merge_cost = parlay::sequence<distanceType>::uninitialized(n-1);
  // UnionFind for maintaining the current cluster information
  auto uf = union_find<indexType>(n);
  auto h = parlay::sequence<kv>::from_function(n,[&](size_t i){
    auto cur_best = NN.nearest_neighbor(i, Points, &uf);
    return std::make_tuple(cur_best.second,i,cur_best.first);
  });
  parallel_searches+=n;
  pt.start();
  std::priority_queue<kv, std::vector<kv>, std::greater<kv>> H(h.begin(), h.end());
  heap_time += pt.stop();
  // pairing_heap::heap<kv> H;
  // H.init(parlay::make_slice(h));
  t.next("Init Done");

  /* CentroidHAC Process */
  indexType u, v, w, u_orig, v_orig;
  distanceType dist, total_dist = 0;
  double one_plus_eps = 1+eps;
  size_t cur = 0;
  while (cur < n-1){
    if (cur % 10000 == 0){
      std::cout << "cur: " << cur << ", total_dist: " << total_dist << std::endl;
    }
    pt.start();
    kv best = H.top(); H.pop();
    heap_time += pt.stop();
    num_heap_ops++;
    std::tie(dist, u_orig, v_orig) = best;
    pt.start();
    u = uf.find_compress(u_orig); // Centroid of u
    v = uf.find_compress(v_orig); // Centroid of v
    uf_time = pt.stop();
    uf_ops++;
    if (u == v){ // u_orig & v_orig in same cluster already
      continue;
    } else {
      // recompute dist and check if within 1+eps (instead of beam_search)
      auto new_dist = Points[u].distance(Points[v]);
      if (new_dist <= one_plus_eps*dist){ // If d(u,v) is at most (1+eps)*d(u_orig,v_orig)<=(1+eps)*opt
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
      } else { // Search nearest point to u, say v', and check if d(u,v')<=(1+eps)*dist <= (1+eps)opt
        pt.start();
        std::pair<indexType,distanceType> cur_best = NN.nearest_neighbor(u, Points, &uf);
        search_time += pt.stop();
        num_searches++;
        if (cur_best.second <= one_plus_eps*dist){
          pt.start();
          w = NN.merge_clusters(u, cur_best.first, Points, &uf);
          merge_time += pt.stop();
          num_merges++;
          parent[rep[u]] = cur + n;
          parent[rep[cur_best.first]] = cur + n;
          rep[w] = cur + n;
          merge_cost[cur] = cur_best.second;
          total_dist += cur_best.second;
          cur++;
        } else{
          num_missed_merges++;
          w = u;
        }
      }
      if (cur < n-1){
        pt.start();
        std::pair<indexType,distanceType> closest_to_w = NN.nearest_neighbor(w, Points, &uf);
        search_time += pt.stop();
        num_searches++;
        H.push(std::make_tuple(closest_to_w.second, w, closest_to_w.first));
      }
    }
  }
  t.next("Centroid Done");
  double total_time = t.total_time();
  std::cout << std::fixed << "Total Cost: " << total_dist << std::endl;
  std::cout << "Number of Searches: " << num_searches << std::endl;
  std::cout << "Number of Merges: " << num_merges << std::endl;
  std::cout << "Number of Missed Merges: " << num_missed_merges << std::endl;
  std::cout << "Number of Heap Operations: " << num_heap_ops << std::endl;
  std::cout << "Number of UnionFind Operations: " << uf_ops << std::endl;
  std::cout << "Number of Parallel Searches: " << parallel_searches << std::endl;
  std::cout << "Search Time: " << search_time << std::endl;
  std::cout << "Merge Time: " << merge_time << std::endl;
  std::cout << "Heap Time: " << heap_time << std::endl;
  std::cout << "UnionFind Time: " << uf_time << std::endl;
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

/*
  Approximate CentroidHAC Implementation
  Computing Per round Errors
  1) Query Error
  2) Round Error
*/

template<typename PointRange, typename indexType, class nn_type>
void CentroidHAC_errors(PointRange &Points, nn_type &NN, 
      double eps = 0.2, char *IndexFile = nullptr, char *ErrorsFile = nullptr) {
  using Point = typename PointRange::pT;
  using distanceType = typename Point::distanceType;
  using kv = std::tuple<distanceType,indexType,indexType>; // Heap element type
  
  parlay::internal::timer t("Centroid HAC (Approx)");

  /* Build Index */
  NN.init(Points, IndexFile);
  auto NNexact = nn_exact<PointRange, indexType>();
  t.next("Index Built");


  /* Init */
  size_t n = Points.size();
  auto uf = union_find<indexType>(n);
  auto h = parlay::sequence<kv>::from_function(n,[&](size_t i){
    auto cur_best = NN.nearest_neighbor(i, Points, &uf);
    return std::make_tuple(cur_best.second,i,cur_best.first);
  });
  std::priority_queue<kv, std::vector<kv>, std::greater<kv>> H(h.begin(), h.end());
  t.next("Init Done");

  /* CentroidHAC Process */

  // Query Error: d(u,v)/d(u,v_exact)
  auto query_error = parlay::sequence<double>();
  auto compute_nearest_error = [&](indexType i, distanceType distance) {
    auto closest = NNexact.nearest_neighbor(i, Points, &uf).second;
    if (closest == 0){
      if (distance == 0){
        query_error.push_back(1.0);
      } else{
        query_error.push_back(0.0);
      }
    } else{
      query_error.push_back(distance/closest);
    }
  };
  // Round Error: d(u,v)/d(u_exact,v_exact)
  auto round_error = parlay::sequence<double>();
  auto compute_closest_error = [&](distanceType distance) {
    auto dists = parlay::sequence<distanceType>::from_function(Points.size(), [&](size_t i){
      return std::numeric_limits<distanceType>::max();
    });
    parlay::parallel_for(0, Points.size(), [&](size_t i){
      if (uf.find_compress(i) == i){
        dists[i] = NN.nearest_neighbor(i, Points, &uf).second;
      }
    });
    auto closest = parlay::reduce(dists, parlay::minm<distanceType>());
    if (closest == 0){
      if (distance == 0){
        round_error.push_back(1.0);
      } else{
        round_error.push_back(0.0);
      }
    } else{
      round_error.push_back(distance/closest);
    }
  };

  std::cout << "check" << std::endl;
  indexType u, v, w, u_orig, v_orig;
  distanceType dist, total_dist = 0;
  double one_plus_eps = 1+eps;
  size_t cur = 0;
  size_t prog1 = std::max((size_t)n/1000,(size_t)1); // Frequency of computing query error
  size_t prog2 = std::max((size_t)n/100,(size_t)1); // Frequency of computing round error
  std::cout << "prog1: " << prog1 << ", prog2: " << prog2 << std::endl;
  while (cur < n-1){
    kv best = H.top(); H.pop();
    std::tie(dist, u_orig, v_orig) = best;
    u = uf.find_compress(u_orig); // Centroid of u
    v = uf.find_compress(v_orig); // Centroid of v
    std::cout << "u_orig: " << u_orig << ", v_orig: " << v_orig << ", u: " << u << ", v: " << v << ", dist: " << dist << std::endl;
    if (u == v){ // u_orig & v_orig in same cluster already
      continue;
    } else {
      // recompute dist and check if within 1+eps (instead of beam_search)
      auto new_dist = Points[u].distance(Points[v]);
      if (new_dist <= one_plus_eps*dist){ // If d(u,v) is at most (1+eps)*d(u,v_orig)<=(1+eps)*opt
        if (cur%prog1 == 0) { compute_nearest_error(u, new_dist); }
        if (cur%prog2 == 0){ compute_closest_error(new_dist); }
        w = NN.merge_clusters(u, v, Points, &uf);
        total_dist += new_dist;
        cur++;
      } else { // Search nearest point to u, say v', and check if d(u,v')<=(1+eps)*dist <= (1+eps)opt
        std::pair<indexType,distanceType> cur_best = NN.nearest_neighbor(u, Points, &uf);
        if (cur_best.second <= one_plus_eps*dist){
          if (cur%prog1 == 0){ compute_nearest_error(u, cur_best.second);}
          if (cur%prog2 == 0){ compute_closest_error(cur_best.second); }
          w = NN.merge_clusters(u, cur_best.first, Points, &uf);
          total_dist += cur_best.second;
          cur++;
        } else{
          w = u;
        }
      }
      if (cur < n-1){
        std::pair<indexType,distanceType> closest_to_w = NN.nearest_neighbor(w, Points, &uf);
        H.push(std::make_tuple(closest_to_w.second, w, closest_to_w.first));
      }
    }
    if (cur % prog1 == 0){
      std::cout << "cur: " << cur << ", total_dist: " << total_dist << std::endl;
    }
  }
  t.next("Centroid Done");
  double total_time = t.total_time();
  std::cout << "Total Distance: " << total_dist << std::endl;
  std::cout << "Number of Query Errors computed: " << query_error.size() << ", Average Query Error: " << parlay::reduce(query_error) / query_error.size() << std::endl;
  std::cout << "Number of Round Errors computed: " << round_error.size() << ", Average Round Error: " << parlay::reduce(round_error) / round_error.size() << std::endl;
  
  if (ErrorsFile){
    // Saving Query Errors
    std::ofstream errors_file;
    errors_file.open(ErrorsFile);
    errors_file << query_error.size() << std::endl;
    for (const auto& error : query_error) {
      errors_file << error << " ";
    }
    errors_file << std::endl;
    // Saving Round Errors
    errors_file << round_error.size() << std::endl;
    for (const auto& error : round_error) {
      errors_file << error << " ";
    }
    errors_file << std::endl;
    errors_file.flush();
    errors_file.close();
  }
}