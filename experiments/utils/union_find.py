class UnionFind:
    def __init__(self, n):
        self.n = n
        self.parents = list(range(2*n-1))

    def find_compress(self, i):
        if self.parents[i] == i:
            return i
        j = self.parents[i]
        while self.parents[j] != j:
            j = self.parents[j]
        tmp = self.parents[i]
        while tmp > j:
            self.parents[i] = j
            i = tmp
            tmp = self.parents[i]
        return j

    def unite(self, u_orig, v_orig):
        u = self.find_compress(u_orig)
        v = self.find_compress(v_orig)
        if u != v:
            if u > v:
                self.parents[u] = v
                return
            else:
                self.parents[v] = u
                return
        raise Exception("Uniting two already merged clusters: " + str(u_orig) + ", " + str(v_orig))
    
    def get_labelling(self):
        return [self.find_compress(i) for i in range(self.n)]