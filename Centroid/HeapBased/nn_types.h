#pragma once

#include <queue>

#include "Centroid/ANN/vamana.h"
#include "Centroid/ANN/utils/beamSearch.h"
#include "Centroid/ANN/utils/types.h"
#include "Centroid/ANN/utils/stats.h"
#include "Centroid/utils/graph.h"
#include "Centroid/utils/union_find.h"

template<typename Point, typename PointRange, typename indexType>
struct nn_knn {
  using distanceType = typename Point::distanceType;
  using pid = std::pair<indexType, distanceType>;
  using PR = PointRange;
  using GraphI = Graph<indexType>;
  using findex = knn_index<Point, PointRange, indexType>;

  GraphI G;
  findex *I;
  indexType start_point;
  BuildParams BP;
  QueryParams QP;
  bool graph_built = false;

  nn_knn(){}

  nn_knn(char *gfile, long R, long L, double alpha, bool tp, size_t n){
    if (gfile){
      G = GraphI(gfile);
      graph_built = true;
    } else {
      G = GraphI(R, n);
    }
    BP = BuildParams(R, L, alpha, tp);
    I = new findex(BP);
    I->set_start();
    start_point = I->get_start();
    QP = QueryParams((long)2, L, (double) 10.0, (long) n, (long) R);
  }

  void init(PR &Points, char *ofile){
    if (!graph_built){
      stats<unsigned int> BuildStats(Points.size());
      I->build_index(G, Points, BuildStats);
      if (ofile){
        G.save(ofile);
      }
    }
    auto visited = parlay::sequence<bool>(Points.size(), false);
    std::queue<indexType> q;
    q.push(start_point);
    visited[start_point] = true;
    size_t num = 1;
    while (!q.empty()){
      indexType u = q.front();
      q.pop();
      for (size_t i=0; i<G[u].size(); i++){
        indexType v = G[u][i];
        if (!visited[v]){
          q.push(v);
          visited[v] = true;
          num++;
        }
      }
    }
    std::cout << "num: " << num << std::endl;
    for (int i=0; i<Points.size(); i++){
      if (!visited[i]){
        std::cout << "Unreachable point: " << i << std::endl;
      }
    }
  }

  pid nearest_neighbor(indexType u, PR &Points, union_find<indexType> *uf, size_t rem) {
    // auto starting_points = parlay::sequence<indexType>::uninitialized(2*BP.L);
    // parlay::random_generator gen;
    // std::uniform_int_distribution<long> dis(0, Points.size() - 1);
    // starting_points[0] = start_point;
    // parlay::parallel_for(1, 2*BP.L, [&](size_t i){
    //   auto r = gen[u+i];
    //   starting_points[i] = dis(r);
    // });
    // auto out = beam_search(Points[u], G, Points, starting_points, QP, uf).first.second;
    auto out = beam_search(Points[u], G, Points, start_point, QP, uf).first.second;
    // parlay::sort_inplace(out, [&](auto x, auto y){ return x.second < y.second; });
    //TODO: make this more efficient and parallel
    int ind = 0;
    while (ind < out.size()){
      if (out[ind].first != u){ return out[ind]; }
      ind++;
    }
    // Error Handling
    std::cout << "No NN found for " << u << std::endl;
    std::cout << "rem = " << rem << std::endl; 
    for (int i=0; i< G[u].size(); i++){
      std::cout << G[u][i] << " " << uf->find_compress(G[u][i]) << std::endl; 
    }
    abort();
  }

  indexType merge_clusters(indexType u, indexType v, PR &Points, union_find<indexType> *uf) {
    indexType w = uf->unite(u,v);
    Points[w].centroid(Points[w^u^v]); // Update u (or v) to the centroid
    // auto out = beam_search(Points[w], G, Points, start_point, QP, uf).first.second;
    auto cand = parlay::sequence<indexType>::from_function(G[w].size()+G[w^u^v].size()/*+std::min(BP.L,(long)out.size())*/,
        [&](size_t i){
      if (i < G[w].size()) {
        return uf->find_compress(G[w][i]);
      } else if(i>= G[w].size() && i < G[w].size()+G[w^u^v].size()) {
        return uf->find_compress(G[w^u^v][i-G[w].size()]);
      }// else{
      //   return out[i-G[w].size()-G[w^u^v].size()].first;
      // }
    });
    auto filtered_cand = parlay::filter(cand, [&](indexType x){return x != w;});
    auto new_nbhs = I->robustPrune(w, filtered_cand, G, Points, BP.alpha, false);
    G[w].update_neighbors(new_nbhs);
    return w;
  }

  void rebuild_graph(PR &Points, union_find<indexType> *uf){
    std::cout << "Rebuilding graph...";
    auto alive_mask = parlay::sequence<bool>::from_function(Points.size(), [&](size_t i){
      return uf->find_compress(i) == i;
    });
    auto alive = parlay::pack_index<size_t>(alive_mask);
    auto new_G = GraphI(BP.max_degree(), G.size());
    stats<unsigned int> BuildStats(Points.size());
    parlay::sequence<indexType> inserts = parlay::tabulate(alive.size(), [&] (size_t i){
					    return static_cast<indexType>(alive[i]);});
    if(BP.two_pass) I->batch_insert(inserts, new_G, Points, BuildStats, 1.0, true, 2, .02);
    I->batch_insert(inserts, new_G, Points, BuildStats, BP.alpha, true, 2, .02);
    parlay::parallel_for (0, alive.size(), [&] (long i) {
      auto less = [&] (indexType j, indexType k) {
		    return Points[alive[i]].distance(Points[j]) < Points[alive[i]].distance(Points[k]);};
      G[alive[i]].sort(less);
    });
    G = new_G;
    std::cout << "Done" << std::endl;
  }
};

template<typename Point, typename PointRange, typename indexType>
struct nn_exact {
  using distanceType = typename Point::distanceType;
  using pid = std::pair<indexType, distanceType>;
  using PR = PointRange;

  nn_exact(){}

  void init(PR &Points, char *ofile){}

  pid nearest_neighbor(indexType u, PR &Points, union_find<indexType> *uf, size_t rem) {
    auto dists = parlay::sequence<distanceType>::from_function(Points.size(), [&](size_t i){
      return std::numeric_limits<distanceType>::max();
    });
    parlay::parallel_for(0,Points.size(), [&](size_t i){
      if ((i!=u) && (uf->find_compress(i) == i)){
        dists[i] = Points[u].distance(Points[i]);
      }
    });
    auto min_elem = parlay::min_element(dists);
    return std::make_pair(min_elem-dists.begin(), *min_elem);
  }

  indexType merge_clusters(indexType u, indexType v, PR &Points, union_find<indexType> *uf) {
    indexType w = uf->unite(u,v);
    Points[w].centroid(Points[w^u^v]); // Update u (or v) to the centroid
    return w;
  }

  void rebuild_graph(PR &Points, union_find<indexType> *uf){}
};