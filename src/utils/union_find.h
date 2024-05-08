#pragma once

#include "parlay/primitives.h"

template <typename indexType>
struct union_find {
  size_t n;
  parlay::sequence<indexType> parents;
  parlay::sequence<indexType> size;
  union_find(size_t n) : n(n) {
    parents = parlay::sequence<indexType>::from_function(n, [&](indexType i) { return i; });
    size = parlay::sequence<indexType>::from_function(n, [&](indexType i) { return 1; });
  }

  indexType find_compress(indexType i) {
    indexType j = i;
    if (parents[j] == j) return j;
    do {
      j = parents[j];
    } while (parents[j] != j);
    indexType tmp;
    while ((tmp = parents[i]) > j) {
      parents[i] = j;
      i = tmp;
    }
    return j;
  }

  // Returns the parent
  indexType unite(indexType u_orig, indexType v_orig) {
    indexType u = u_orig;
    indexType v = v_orig;
    if (u!=v){
      u = find_compress(u);
      v = find_compress(v);
      if (u > v) {
        parents[u] = v;
        size[v]+=size[u];
        return v;
      } else if (v > u) {
        parents[v] = u;
        size[u]+=size[v];
        return u;
      }
    }
    std::cerr << "Uniting two already merged clusters: " << u_orig 
              << ", " << v_orig << std::endl;
    assert(false);
    exit(-1);
  }

  indexType get_size(indexType u) {
    return size[find_compress(u)];
  }
};