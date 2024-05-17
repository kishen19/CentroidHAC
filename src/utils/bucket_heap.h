#pragma once

template <typename KeyType, typename IndexType>
struct BucketHeap{
    parlay::sequence<parlay::sequence<KeyType>> buckets;
    size_t n;
    size_t cur_bucket;
    double one_plus_eps;

    bucket_heap(parlay::sequence<KeyType> _nodes, size_t _n, double _eps): n(_n), one_plus_eps(1+_eps){
        buckets = parlay::sequence<parlay::sequence<KeyType>>::uninitialized(n);
        auto threshold = parlay::reduce(_nodes, parlay::minm<KeyType>());
        auto bucket_id = parlay::sequence<std::pair<KeyType,IndexType>>::from_function(n, [&](IndexType i){
            auto init_threshold = threshold*one_plus_eps;
            size_t cur = 0;
            while (init_threshold < _nodes[i]){
                init_threshold *= one_plus_eps;
                cur++;
            }
            return {cur,i};
        });
        parlay::sort_inplace(bucket_id);


    }

    void heapify(size_t i){
        size_t l = 2*i+1;
        size_t r = 2*i+2;
        size_t ind = i;
        if (l < size && nodes[l] < nodes[i]){
            ind = l;
        }
        if (r < size && nodes[r] < nodes[ind]){
            ind = r;
        }
        if (ind != i){
            key_type temp = nodes[i];
            nodes[i] = nodes[ind];
            nodes[ind]= temp;
            heapify(ind);
        }
    }

    inline key_type top(){
        if (size > 0){
            return nodes[0];
        } else{
            std::cout << "Empty Heap!" << std::endl;
            std::exit(1);
        }
    }

    inline void pop(){
        if(size == 1){
            size--;
        } else if(size > 1){
            nodes[0] = nodes[size-1];
            size--;
            heapify(0);
        } else{
            std::cout << "Empty Heap!" << std::endl;
            std::exit(1);
        }
    }

    void insert(){

    }
};