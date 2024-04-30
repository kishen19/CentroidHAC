#pragma once
#include <algorithm>

#include "Centroid/vamana/utils/beamSearch.h"
#include "Centroid/vamana/utils/stats.h"
#include "Centroid/vamana/utils/types.h"
// #include "Centroid/utils/check_nn_recall.h"
// #include "Centroid/utils/parse_results.h"
#include "Centroid/utils/graph.h"
#include "Centroid/utils/union_find.h"

#include "nn_types.h"

/*
  Approximate CentroidHAC Implementation
  Running time: O(n log n)
*/
template<typename PointRange, typename indexType, class nn_type>
typename PointRange::pT::distanceType CentroidHAC(char *ofile, PointRange &Points, nn_type &NN, double eps = 0.2) {
  using Point = typename PointRange::pT;
  using distanceType = typename Point::distanceType;
  using kv = std::tuple<distanceType,indexType,indexType>; // Heap element type
  
  parlay::internal::timer t("Centroid HAC (Approx)");

  /* Build Index */
  NN.init(Points, ofile);
  t.next("Index Built");
  return 0.0;

  /* Init */
  size_t n = Points.size();
  auto uf = union_find<indexType>(n);
  auto h = parlay::sequence<kv>::from_function(n,[&](size_t i){
    auto cur_best = NN.nearest_neighbor(i, Points, &uf, n);
    return std::make_tuple(cur_best.second,i,cur_best.first);
  });
  std::priority_queue<kv, std::vector<kv>, std::greater<kv>> H(h.begin(), h.end());
  t.next("Init Done");

  /* CentroidHAC Process */
  indexType u, v, w, u_orig, v_orig;
  distanceType dist, total_dist = 0;
  size_t rem = n;
  // float n_ = n;
  // float fac = 0.5;
  while (rem > 1){
    // if ((float)rem < n_*fac){
    //   std::cout << "rem: " << rem << ", n_: " << n_ << std::endl;
      // NN.rebuild_graph(Points, &uf);
      // n_ = rem;
    /*} else */if (rem % 10000 == 0){
      std::cout << "rem: " << rem << ", total_dist: " << total_dist << std::endl;
    }
    kv best = H.top();
    H.pop();
    std::tie(dist, u_orig, v_orig) = best;
    u = uf.find_compress(u_orig); // Centroid of u
    v = uf.find_compress(v_orig); // Centroid of v
    if (u!= u_orig) {
      w = u; u = v; v = w; // swap u and v
      w = u_orig; u_orig = v_orig; v_orig = w; // swap u_orig and v_orig
    }
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
      if (rem > 1){
        std::pair<indexType,distanceType> closest_to_w = NN.nearest_neighbor(w, Points, &uf, rem);
        H.push(std::make_tuple(closest_to_w.second, w, closest_to_w.first));
      }
    } else{ // Always merge
      auto w = NN.merge_clusters(u, v, Points, &uf);
      total_dist += dist;
      rem--;
      if (rem > 1){
        std::pair<indexType,distanceType> closest_to_w = NN.nearest_neighbor(w, Points, &uf, rem);
        H.push(std::make_tuple(closest_to_w.second, w, closest_to_w.first));
      }
    }
  }
  t.next("Centroid Done");
  std::cout << std::fixed << "Total Distance: " << total_dist << std::endl;
  return total_dist;
}

/*
  Approximate CentroidHAC Implementation
  with Recall Stats
  Running time: O(n log n)
*/

template<typename PointRange, typename indexType, class nn_type>
void CentroidHAC_check_recall(char *ofile, PointRange &Points, nn_type &NN, double eps = 0.2) {
  using Point = typename PointRange::pT;
  using distanceType = typename Point::distanceType;
  using kv = std::tuple<distanceType,indexType,indexType>; // Heap element type
  
  parlay::internal::timer t("Centroid HAC (Approx)");

  /* Build Index */
  NN.init(Points, ofile);
  t.next("Index Built");

  /* Init */
  size_t n = Points.size();
  auto uf = union_find<indexType>(n);
  auto h = parlay::sequence<kv>::from_function(n,[&](size_t i){
    auto cur_best = NN.nearest_neighbor(i, Points, &uf, n);
    return std::make_tuple(cur_best.second,i,cur_best.first);
  });
  std::priority_queue<kv, std::vector<kv>, std::greater<kv>> H(h.begin(), h.end());
  t.next("Init Done");

  /* CentroidHAC Process */
  indexType u, v, w, u_orig, v_orig;
  distanceType dist, total_dist = 0;
  size_t rem = n;
  // float n_ = n;
  // float fac = 0.5;
  while (rem > 1){
    // if ((float)rem < n_*fac){
    //   std::cout << "rem: " << rem << ", n_: " << n_ << std::endl;
      // NN.rebuild_graph(Points, &uf);
      // n_ = rem;
    /*} else */
    if (rem % 10000 == 0){
      std::cout << "rem: " << rem << ", total_dist: " << total_dist << std::endl;
    }
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
      if (rem > 1){
        std::pair<indexType,distanceType> closest_to_w = NN.nearest_neighbor(w, Points, &uf, rem);
        H.push(std::make_tuple(closest_to_w.second, w, closest_to_w.first));
      }
    }
  }
  t.next("Centroid Done");
  std::cout << "Total Distance: " << total_dist << std::endl;
}


/* 
  Exact CentroidHAC Implementation
  Running time: O(n^2 log n)
*/
template<typename PointRange, typename indexType>
typename PointRange::pT::distanceType CentroidHAC_Exact(PointRange &Points) {
  using Point = typename PointRange::pT;
  using distanceType = typename Point::distanceType;
  using kv = std::tuple<distanceType,indexType,indexType>; // Heap element type
  
  // parlay::internal::timer t("Centroid HAC (Exact)");

  /* Init */
  size_t n = Points.size();
  auto uf = union_find<indexType>(n);
  auto NN = nn_exact<PointRange, indexType>();
  auto h = parlay::sequence<kv>::from_function(n,[&](size_t i){
    auto cur_best = NN.nearest_neighbor(i, Points, &uf, n);
    return std::make_tuple(cur_best.second,i,cur_best.first);
  });
  std::priority_queue<kv, std::vector<kv>, std::greater<kv>> H(h.begin(), h.end());
  // t.next("Init Done");

  /* CentroidHAC Process */
  indexType u, v, w, u_orig, v_orig;
  distanceType dist, total_dist = 0;
  size_t rem = n;
  while (rem > 1){
    if (rem % 10000 == 0){
      std::cout << "rem: " << rem << ", total_dist: " << total_dist << std::endl;
    }
    kv best = H.top();
    H.pop();
    std::tie(dist, u_orig, v_orig) = best;
    u = uf.find_compress(u_orig); // Centroid of u
    v = uf.find_compress(v_orig); // Centroid of v
    // If either u_orig or v_orig is not active, continue
    if (u!= u_orig || v != v_orig){ 
      continue;
    } else{ // merge step
      auto w = NN.merge_clusters(u, v, Points, &uf);
      total_dist += dist;
      rem--;
      if (rem > 1){// Add the nearest neighbor of w to the heap
        std::pair<indexType,distanceType> closest_to_w = NN.nearest_neighbor(w, Points, &uf, rem);
        H.push(std::make_tuple(closest_to_w.second, w, closest_to_w.first));
      }
    }
  }
  // t.next("Centroid Done");
  std::cout << std::fixed << "Total Distance: " << total_dist << std::endl;
  return total_dist;
}