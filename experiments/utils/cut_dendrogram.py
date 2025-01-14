from collections import deque

from utils.union_find import UnionFind

def read_dendrogram(input_file):
    with open(input_file, "r") as f:
        data = f.readlines()
        n = int(data[0])
        parent = [int(data[i]) for i in range(1, 2*n)]
        merge_cost = [float(data[i]) for i in range(2*n, 3*n-1)]
    return n, parent, merge_cost

def make_cuts(input_file, eps = 0.1):
    # Read the dendrogram from the input file
    n, parent, merge_cost = read_dendrogram(input_file)
    uf = UnionFind(n)
    one_plus_eps = 1 + eps
    threshold = 1e-12
    visited = [0]*(n-1)
    visited[-1]=1
    q = deque([i for i in range(n)])
    iter = 0
    rem = 2*n-2
    while rem > 0:
        new_q = deque()
        while q and rem>0:
            i = q.popleft()
            if merge_cost[parent[i]-n] <= threshold:
                uf.unite(i, parent[i])
                rem-=1
                if not visited[parent[i]-n]: # root never added to q
                    visited[parent[i]-n]=1
                    q.append(parent[i])
            else:
                new_q.append(i)
        q = new_q
        labeling  = uf.get_labelling()
        yield iter, labeling
        iter += 1
        threshold *= one_plus_eps

def make_all_cuts(input_file):
    # Read the dendrogram from the input file
    n, parent, merge_cost = read_dendrogram(input_file)
    uf = UnionFind(n)
    seq = sorted([(parent[i],i) for i in range(2*n-1)])
    for i in range(0,2*n-2,2):
        uf.unite(seq[i][1], seq[i][0])
        uf.unite(seq[i+1][1], seq[i+1][0])
        labeling  = uf.get_labelling()
        yield i//2, labeling