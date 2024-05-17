#pragma once

#include "parlay/parallel.h"
#include "parlay/primitives.h"

namespace pairing_heap{
template <class Node>
Node* meld(Node* L, Node* R){
    if (L == nullptr){
        return R;
    } else if (R == nullptr){
        return L;
    } else{
        if (L->get_key() <= R->get_key()){
            R->sibling = L->child;
            L->child = R;
            return L;
        } else {
            L->sibling = R->child;
            R->child = L;
            return R;
        }
    }
}

template <class Node>
Node* two_pass_merge(Node* H){
    if (H == nullptr || H->sibling == nullptr){
        return H;
    } else{
        Node* temp1 = H->sibling;
        Node* temp2 = H->sibling->sibling;
        H->sibling = nullptr;
        temp1->sibling = nullptr;
        return meld(meld(H, temp1), two_pass_merge(temp2));
    }
}

/////////// Pairing Heap

template <typename KeyType>
struct node{
    KeyType key;
    node* child;
    node* sibling;
    
    node(KeyType _key) : key(_key), child(nullptr), sibling(nullptr) {}

    void init(KeyType _key) {
        key = _key; 
        child = nullptr; 
        sibling = nullptr;
    }

    inline KeyType get_key(){
        return key;
    }
};

template <typename KeyType>
struct heap{
    using node_allocator = parlay::type_allocator<node<KeyType>>;
    node<KeyType>* root;
    size_t size;

    heap() : root(nullptr), size(0){}
    
    inline KeyType top(){
        if (root){
            return root->key;
        } else{
            std::cout << "Empty Heap!" << std::endl;
            std::exit(1);
        }
    }

    inline void pop(){
        if (root){
            // node<KeyType>* temp = root;
            root = two_pass_merge(root->child);
            // node_allocator::destroy(temp);
            size--;
        } else{
            std::cout << "Empty Heap!" << std::endl;
            std::exit(1);
        }
    }

    inline bool is_empty(){
        return (size == 0);
    }

    inline void push(KeyType key){
        auto new_node = node_allocator::create(key);
        root = meld(root, new_node);
        size++;
    }

    template <class Seq>
    inline void init(const Seq& A){
        root = heapify_dc(A);
        size = A.size();
    }

    inline void init(node<KeyType>* A, size_t n){
        root = heapify_dc(A, n);
        size = n;
    }

    template <class Seq>
    node<KeyType>* heapify_dc(const Seq& A){
        size_t n = A.size();
        if (n == 0) {return nullptr;}
        else if (n <= 2048){
            // std::cout << "here, size= "<<n << std::endl;
            auto temp = node_allocator::create(A[0]);
            for (size_t i=1; i<n; i++){
                auto new_node = node_allocator::create(A[i]);
                temp = meld(temp, new_node);
            }
            return temp;
        } else {
            node<KeyType> *heap1, *heap2;
            parlay::par_do(
                [&](){heap1 = heapify_dc(A.cut(0, n/2)); },
                [&](){heap2 = heapify_dc(A.cut(n/2, n)); }
            );
            return meld(heap1, heap2);
        }
    }

    node<KeyType>* heapify_dc(node<KeyType>* A, size_t n){
        if (n == 0) {return nullptr;}
        else if (n <= 2048){
            // std::cout << "here, size= "<<n << std::endl;
            auto temp = &A[0];
            for (size_t i=1; i<n; i++){
                temp = meld(temp, &A[i]);
            }
            return temp;
        } else {
            node<KeyType> *heap1, *heap2;
            parlay::par_do(
                [&](){heap1 = heapify_dc(A, n/2); },
                [&](){heap2 = heapify_dc(A+n/2, n-n/2); }
            );
            return meld(heap1, heap2);
        }
    }
};
} // namespace pairing_heap