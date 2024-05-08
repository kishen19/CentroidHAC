#pragma once

#include <queue>

#include "src/vamana/index.h"
#include "src/vamana/utils/beamSearch.h"
#include "src/vamana/utils/types.h"
#include "src/vamana/utils/stats.h"
#include "src/utils/graph.h"
#include "src/utils/union_find.h"

template<typename PointRange, typename indexType>
struct nn_knn {
  using Point = typename PointRange::pT;
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
    // auto visited = parlay::sequence<bool>(Points.size(), false);
    // std::queue<indexType> q;
    // q.push(start_point);
    // visited[start_point] = true;
    // size_t num = 1;
    // while (!q.empty()){
    //   indexType u = q.front();
    //   q.pop();
    //   for (size_t i=0; i<G[u].size(); i++){
    //     indexType v = G[u][i];
    //     if (!visited[v]){
    //       q.push(v);
    //       visited[v] = true;
    //       num++;
    //     }
    //   }
    // }
    // std::cout << "num: " << num << std::endl;
    // for (int i=0; i<Points.size(); i++){
    //   if (!visited[i]){
    //     std::cout << "Unreachable point: " << i << std::endl;
    //   }
    // }
  }

  pid nearest_neighbor(indexType u, PR &Points, union_find<indexType> *uf) {
    // auto starting_points = parlay::sequence<indexType>::uninitialized(2*BP.L);
    // parlay::random_generator gen;
    // std::uniform_int_distribution<long> dis(0, Points.size() - 1);
    // starting_points[0] = start_point;
    // parlay::parallel_for(1, 2*BP.L, [&](size_t i){
    //   auto r = gen[u+i];
    //   starting_points[i] = dis(r);
    // });
    // auto out = beam_search(Points[u], G, Points, starting_points, QP, uf).first.second;
    parlay::internal::timer t;
    t.start();
    auto out = beam_search(Points[u], G, Points, start_point, QP, uf).first.second;
    t.start();
    size_t ind = 0;
    while (ind < out.size()){
      if (out[ind].first != u){ 
        return out[ind]; 
      }
      ind++;
    }
    rebuild_graph(Points, uf);
    return nearest_neighbor(u, Points, uf);
  }

  indexType merge_clusters(indexType u, indexType v, PR &Points, union_find<indexType> *uf) {
    size_t sz1 = uf->get_size(u);
    size_t sz2 = uf->get_size(v);
    indexType w = uf->unite(u,v);
    indexType t = w^u^v;
    if (w == u){
      Points[w].centroid(Points[t], sz1, sz2); // Update u (or v) to the centroid
    } else{
      Points[w].centroid(Points[t], sz2, sz1); // Update u (or v) to the centroid
    }
    auto cand = parlay::sequence<indexType>::from_function(G[w].size()+G[t].size(),
        [&](size_t i){
      if (i < G[w].size()) {
        return uf->find_compress(G[w][i]);
      } else {
        return uf->find_compress(G[t][i-G[w].size()]);
      }
    });
    auto filtered_cand = parlay::filter(cand, [&](indexType x){return x != w;});
    auto new_nbhs = I->robustPrune(w, filtered_cand, G, Points, BP.alpha, false);
    G[w].update_neighbors(new_nbhs);
    return w;
  }

  void rebuild_graph(PR &Points, union_find<indexType> *uf){
    std::cout << "Rebuilding graph..." << std::endl;
    auto alive_mask = parlay::sequence<bool>::from_function(Points.size(), [&](size_t i){
      return uf->find_compress(i) == i;
    });
    auto alive = parlay::pack_index<size_t>(alive_mask);
    auto new_G = GraphI(BP.max_degree(), G.size());
    stats<unsigned int> BuildStats(Points.size());
    parlay::sequence<indexType> inserts = parlay::tabulate(alive.size(), [&] (size_t i){
					    return static_cast<indexType>(alive[i]);});
    if(BP.two_pass){
      I->batch_insert(inserts, new_G, Points, BuildStats, 1.0, true, 2, .02, false);
    }
    I->batch_insert(inserts, new_G, Points, BuildStats, BP.alpha, true, 2, .02, false);
    parlay::parallel_for (0, alive.size(), [&] (long i) {
      auto less = [&] (indexType j, indexType k) {
		    return Points[alive[i]].distance(Points[j]) < Points[alive[i]].distance(Points[k]);};
      new_G[alive[i]].sort(less);
    });
    G = new_G;
  }
};

template<typename PointRange, typename indexType>
struct nn_exact {
  using Point = typename PointRange::pT;
  using distanceType = typename Point::distanceType;
  using pid = std::pair<indexType, distanceType>;
  using PR = PointRange;

  nn_exact(){}

  void init(PR &Points, char *ofile){}

  pid nearest_neighbor(indexType u, PR &Points, union_find<indexType> *uf) {
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
    size_t sz1 = uf->get_size(u);
    size_t sz2 = uf->get_size(v);
    indexType w = uf->unite(u,v);
    if (w == u){
      Points[w].centroid(Points[v], sz1, sz2); // Update u (or v) to the centroid
    } else{
      Points[w].centroid(Points[u], sz2, sz1); // Update u (or v) to the centroid
    }
    return w;
  }

  void rebuild_graph(PR &Points, union_find<indexType> *uf){}
};