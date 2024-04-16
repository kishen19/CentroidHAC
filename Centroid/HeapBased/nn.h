


template<typename Point, typename PointRange, typename indexType>
struct nn_knn {
  using pid = std::pair<indexType, distanceType>;
  using PR = PointRange;
  using GraphI = Graph<indexType>;
  using findex = knn_index<Point, PointRange, indexType>;

  GraphI G;
  findex I;
  indexType start_point;
  QueryParams QP;

  nn_knn(): {

  }

  pid nearest_neighbor(indexType u, PR &Points, union_find<indexType> *uf, size_t &rem) {
    auto out = beam_search(Points[u], G, Points, start_point, QP, uf).first.first;
    parlay::sort_inplace(out, [&](auto x, auto y){ return x.second < y.second; });
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
      std::cout << G[u][i] << " " << uf.find_compress(G[u][i]) << std::endl; 
    }
    abort();
  }

  indexType merge_clusters(indexType u, indexType v, PR &Points, union_find<indexType> *uf) {
    indexType w = uf->unite(u,v);
    Points[w].centroid(Points[w^u^v]); // Update u (or v) to the centroid
    auto cand = parlay::sequence<indexType>::from_function(G[w].size()+G[w^u^v].size(),
        [&](size_t i){
      if (i < G[w].size()) {
        return uf->find_compress(G[w][i]);
      } else {
        return uf->find_compress(G[w^u^v][i-G[w].size()]);
      }
    });
    auto filtered_cand = parlay::filter(cand, [&](indexType x){return x != w;});
    auto new_nbhs = I.robustPrune(w, filtered_cand, G, Points, BP.alpha, false);
    G[w].update_neighbors(new_nbhs);
    return w;
  };
}

template<typename Point, typename PointRange, typename indexType>
struct nn_exact {
  using pid = std::pair<indexType, distanceType>;
  using PR = PointRange;

  nn_exact(){}

  pid nearest_neighbor(indexType u, PR &Points, union_find<indexType> *uf, size_t &rem) {
    auto dists = parlay::sequence<distanceType>::from_function(Points.size(), [&](size_t i){
      return std::numeric_limits<distanceType>::max();
    });
    parlay::parallel_for(0,Points.size(), [&](size_t i){
      if (i!=u && uf->find_compress(i) != i){
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
  };
}